/*****************************************************************************
 *  __________________    _________  _____            _____  .__         ._.
 *  \______   \______ \  /   _____/ /     \          /  _  \ |__| ____   | |
 *   |    |  _/|    |  \ \_____  \ /  \ /  \        /  /_\  \|  _/ __ \  | |
 *   |    |   \|    `   \/        /    Y    \      /    |    |  \  ___/   \|
 *   |______  /_______  /_______  \____|__  / /\   \____|__  |__|\___ |   __
 *          \/        \/        \/        \/  )/           \/        \/   \/
 *
 * This file is part of liBDSM. Copyright © 2014-2015 VideoLabs SAS
 *
 * Author: Julien 'Lta' BALLET <contact@lta.io>
 *
 * liBDSM is released under LGPLv2.1 (or later) and is also available
 * under a commercial license.
 *****************************************************************************
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "bdsm_debug.h"
#include "smb_session.h"
#include "smb_session_msg.h"
#include "smb_fd.h"
#include "smb_ntlm.h"
#include "smb_spnego.h"
#include "smb_transport.h"

static int        smb_negotiate(smb_session *s);

smb_session   *smb_session_new()
{
    smb_session *s;

    s = calloc(1, sizeof(smb_session));
    if (!s)
        return NULL;

    s->guest              = false;

    // Explicitly sets pointer to NULL, insted of 0
    s->spnego_asn1        = NULL;
    s->transport.session  = NULL;
    s->shares             = NULL;

    s->creds.domain       = NULL;
    s->creds.login        = NULL;
    s->creds.password     = NULL;

    smb_buffer_init(&s->xsec_target, NULL, 0);

    // Until we know more, assume server supports everything.
    // s->c

    return s;
}

void            smb_session_destroy(smb_session *s)
{
    assert(s != NULL);

    smb_session_share_clear(s);

    // FIXME Free smb_share and smb_file
    if (s->transport.session != NULL)
    {
        s->transport.destroy(s->transport.session);
        s->transport.session = NULL;
    }

    if (s->spnego_asn1 != NULL)
        asn1_delete_structure(&s->spnego_asn1);

    smb_buffer_free(&s->xsec_target);

    // Free stored credentials.
    free(s->creds.domain);
    free(s->creds.login);
    free(s->creds.password);
    free(s);
}

void            smb_session_set_creds(smb_session *s, const char *domain,
                                      const char *login, const char *password)
{
    assert(s != NULL);

    if (domain != NULL)
    {
        if (s->creds.domain != NULL)
            free(s->creds.domain);
        s->creds.domain = strndup(domain, SMB_CREDS_MAXLEN);
    }

    if (login != NULL)
    {
        if (s->creds.login != NULL)
            free(s->creds.login);
        s->creds.login = strndup(login, SMB_CREDS_MAXLEN);
    }

    if (password != NULL)
    {
        if (s->creds.password != NULL)
            free(s->creds.password);
        s->creds.password = strndup(password, SMB_CREDS_MAXLEN);
    }
}

int             smb_session_connect(smb_session *s, const char *name,
                                    uint32_t ip, int transport)
{
    assert(s != NULL && name != NULL);

    if (s->transport.session != NULL)
        s->transport.destroy(s->transport.session);

    switch (transport)
    {
        case SMB_TRANSPORT_TCP:
            smb_transport_tcp(&s->transport);
            break;
        case SMB_TRANSPORT_NBT:
            smb_transport_nbt(&s->transport);
            break;
        default:
            return DSM_ERROR_GENERIC;
    }

    if ((s->transport.session = s->transport.new(SMB_DEFAULT_BUFSIZE)) == NULL)
        return DSM_ERROR_GENERIC;
    if (!s->transport.connect(ip, s->transport.session, name))
        return DSM_ERROR_NETWORK;

    memcpy(s->srv.name, name, strlen(name) + 1);

    return smb_negotiate(s);
}

static int        smb_negotiate(smb_session *s)
{
    const char          *dialects[] = SMB_DIALECTS;
    smb_message         *msg = NULL;
    smb_message         answer;
    smb_nego_resp       *nego;
    uint16_t payload_size;

    assert(s != NULL);

    msg = smb_message_new(SMB_CMD_NEGOTIATE);
    if (!msg)
        return DSM_ERROR_GENERIC;

    smb_message_put8(msg, 0);   // wct
    smb_message_put16(msg, 0);  // bct, will be updated later

    for (unsigned i = 0; dialects[i] != NULL; i++)
        smb_message_append(msg, dialects[i], strlen(dialects[i]) + 1);
    payload_size = msg->cursor - 3;
    memcpy(msg->packet->payload + 1, &payload_size, sizeof(payload_size));

    if (!smb_session_send_msg(s, msg))
    {
        smb_message_destroy(msg);
        return DSM_ERROR_NETWORK;
    }
    smb_message_destroy(msg);

    if (!smb_session_recv_msg(s, &answer))
        return DSM_ERROR_NETWORK;

    if (answer.payload_size < sizeof(smb_nego_resp))
    {
        BDSM_dbg("[smb_negotiate]Malformed message\n");
        return DSM_ERROR_NETWORK;
    }

    nego = (smb_nego_resp *)answer.packet->payload;
    if (!smb_session_check_nt_status(s, &answer))
        return DSM_ERROR_NT;
    if (nego->wct != 0x11)
        return DSM_ERROR_NETWORK;

    s->srv.dialect        = nego->dialect_index;
    s->srv.security_mode  = nego->security_mode;
    s->srv.caps           = nego->caps;
    s->srv.ts             = nego->ts;
    s->srv.session_key    = nego->session_key;

    // Copy SPNEGO supported mechanisms  token for later usage (login_gss())
    if (smb_session_supports(s, SMB_SESSION_XSEC))
        BDSM_dbg("Server is supporting extended security\n");
    else
        s->srv.challenge      = nego->challenge;

    return DSM_SUCCESS;
}


static int        smb_session_login_ntlm(smb_session *s, const char *domain,
        const char *user, const char *password)
{
    smb_message           answer;
    smb_message           *msg = NULL;
    smb_session_req       req;
    uint8_t               *ntlm2 = NULL;
    smb_ntlmh             hash_v2;
    uint64_t              user_challenge;

    assert(s != NULL);

    msg = smb_message_new(SMB_CMD_SETUP);
    if (!msg)
        return DSM_ERROR_GENERIC;

    // this struct will be set at the end when we know the payload size
    SMB_MSG_ADVANCE_PKT(msg, smb_session_req);

    user_challenge = smb_ntlm_generate_challenge();

    // LM2 Response
    smb_ntlm2_hash(user, password, domain, hash_v2);
    ntlm2 = smb_lm2_response(hash_v2, s->srv.challenge, user_challenge);
    smb_message_append(msg, ntlm2, 16 + 8);
    free(ntlm2);

    if (msg->cursor / 2) // Padding !
        smb_message_put8(msg, 0);

    smb_message_put_utf16(msg, user, strlen(user));
    smb_message_put16(msg, 0);
    smb_message_put_utf16(msg, domain, strlen(domain));
    smb_message_put16(msg, 0);
    smb_message_put_utf16(msg, SMB_OS, strlen(SMB_OS));
    smb_message_put16(msg, 0);
    smb_message_put_utf16(msg, SMB_LANMAN, strlen(SMB_LANMAN));
    smb_message_put16(msg, 0);

    SMB_MSG_INIT_PKT_ANDX(req);
    req.wct              = 13;
    req.max_buffer       = SMB_SESSION_MAX_BUFFER;
    req.mpx_count        = 16; // XXX ?
    req.vc_count         = 1;
    //req.session_key      = s->srv.session_key; // XXX Useless on the wire?
    req.caps             = s->srv.caps; // XXX caps & our_caps_mask
    req.oem_pass_len = 16 + SMB_LM2_BLOB_SIZE;
    req.uni_pass_len = 0; //16 + blob_size; //SMB_NTLM2_BLOB_SIZE;
    req.payload_size = msg->cursor - sizeof(smb_session_req);
    SMB_MSG_INSERT_PKT(msg, 0, req);

    if (!smb_session_send_msg(s, msg))
    {
        smb_message_destroy(msg);
        BDSM_dbg("Unable to send Session Setup AndX message\n");
        return DSM_ERROR_NETWORK;
    }
    smb_message_destroy(msg);

    if (smb_session_recv_msg(s, &answer) == 0)
    {
        BDSM_dbg("Unable to get Session Setup AndX reply\n");
        return DSM_ERROR_NETWORK;
    }

    if (answer.payload_size < sizeof(smb_session_resp))
    {
        BDSM_dbg("[smb_negotiate]Malformed message\n");
        return DSM_ERROR_NETWORK;
    }

    smb_session_resp *r = (smb_session_resp *)answer.packet->payload;
    if (!smb_session_check_nt_status(s, &answer))
    {
        BDSM_dbg("Session Setup AndX : failure.\n");
        return DSM_ERROR_NT;
    }

    if (r->action & 0x0001)
        s->guest = true;

    s->srv.uid  = answer.packet->header.uid;
    s->logged = true;

    return DSM_SUCCESS;
}

int             smb_session_login(smb_session *s)
{
    assert(s != NULL);

    if (s->creds.domain == NULL
        || s->creds.login == NULL
        || s->creds.password == NULL)
      return DSM_ERROR_GENERIC;

    if (smb_session_supports(s, SMB_SESSION_XSEC))
        return (smb_session_login_spnego(s, s->creds.domain, s->creds.login,
                                         s->creds.password));
    else
        return (smb_session_login_ntlm(s, s->creds.domain, s->creds.login,
                                       s->creds.password));
}

int        smb_session_logoff(smb_session *s)
{
    smb_message    *msg = NULL;

    assert(s != NULL);

    msg = smb_message_new(SMB_CMD_LOGOFF);
    if (!msg)
        return DSM_ERROR_GENERIC;

    smb_message_put8(msg, 0x02);   // wct=2
    smb_message_put8(msg, 0xff); //andx: no further commands
    smb_message_put8(msg, 0); //reserved
    smb_message_put16(msg, 0);  // andx offset
    smb_message_put16(msg, 0);  // bct

    if (!smb_session_send_msg(s, msg))
    {
        smb_message_destroy(msg);
        BDSM_dbg("Unable to send Session Logoff AndX message\n");
        return DSM_ERROR_NETWORK;
    }
    smb_message_destroy(msg);

    s->srv.uid  = 0;
    s->logged = false;
    s->guest = false;

    return DSM_SUCCESS;
}

int             smb_session_is_guest(smb_session *s)
{
    assert(s != NULL);

    // We're not logged in yet.
    if (s->logged != true)
        return -1;

    // We're logged in as guest
    if (s->guest)
        return 1;

    // We're logged in as regular user
    return 0;
}

const char      *smb_session_server_name(smb_session *s)
{
    assert(s != NULL);

    return s->srv.name;
}

int             smb_session_supports(smb_session *s, int what)
{
    assert(s != NULL);

    switch (what)
    {
        case SMB_SESSION_XSEC:
            return s->srv.caps & SMB_CAPS_XSEC;
        default:
            return 0;
    }
}

uint32_t        smb_session_get_nt_status(smb_session *s)
{
    assert(s != NULL);

    return s->nt_status;
}

bool smb_session_check_nt_status(smb_session *s, smb_message *msg)
{
    assert(s != NULL && msg != NULL);

    if (msg->packet->header.status != NT_STATUS_SUCCESS)
    {
        s->nt_status = msg->packet->header.status;
        return false;
    }
    return true;
}
