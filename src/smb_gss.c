//---------------------------------------------------------------------------
//  __________________    _________  _____            _____  .__         ._.
//  \______   \______ \  /   _____/ /     \          /  _  \ |__| ____   | |
//   |    |  _/|    |  \ \_____  \ /  \ /  \        /  /_\  \|  _/ __ \  | |
//   |    |   \|    `   \/        /    Y    \      /    |    |  \  ___/   \|
//   |______  /_______  /_______  \____|__  / /\   \____|__  |__|\___ |   __
//          \/        \/        \/        \/  )/           \/        \/   \/
//
// This file is part of libdsm. Copyright © 2014 VideoLabs SAS
//
// Author: Julien 'Lta' BALLET <contact@lta.io>
//
// This program is free software. It comes without any warranty, to the extent
// permitted by applicable law. You can redistribute it and/or modify it under
// the terms of the Do What The Fuck You Want To Public License, Version 2, as
// published by Sam Hocevar. See the COPYING file for more details.
//----------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "bdsm/debug.h"
#include "bdsm/smb_session.h"
#include "bdsm/smb_ntlm.h"

void            smb_gss_display_status(OM_uint32 major,
                                       OM_uint32 minor,
                                       const gss_OID mech_type)
{
  OM_uint32 msg_ctx = 0, local_minor;
  gss_buffer_desc maj_str, min_str;

  do {
    local_minor = minor;
    gss_display_status(&local_minor, major, GSS_C_GSS_CODE,
                       mech_type, &msg_ctx, &maj_str);
    gss_display_status(&local_minor, minor, GSS_C_MECH_CODE,
                       mech_type, &msg_ctx, &min_str);

    BDSM_dbg("smb_gss_status: (0x%x, 0x%x) %.*s: %.*s\n", major, minor,
             (int)maj_str.length,
             (char *)maj_str.value,
             (int)min_str.length,
             (char *)min_str.value);

    gss_release_buffer(&local_minor, &maj_str);
    gss_release_buffer(&local_minor, &min_str);

   } while (msg_ctx != 0);
}

int             smb_session_login_gss(smb_session *s, const char *domain,
                                      const char *user, const char *password)
{
  OM_uint32         major_status, minor_status;
  gss_name_t        gss_user, gss_server;
  gss_OID_set_desc  mechs, *mechsp = GSS_C_NO_OID_SET;
  gss_buffer_desc   input_buf, output_buf, tmp_buf;
  gss_OID           req_mech_type, actual_mech_type;
  gss_cred_id_t     creds;


  assert(s != NULL && user != NULL && password != NULL);

  //tmp_buf.value  = user;
  asprintf(&tmp_buf.value, "%s\\%s", domain, user);
  tmp_buf.length = strlen(tmp_buf.value);
  gss_import_name(&minor_status, &tmp_buf, GSS_C_NT_USER_NAME, &gss_user);

  //tmp_buf.value  = domain;
  asprintf(&tmp_buf.value, "%s@%s", "smb", domain);
  tmp_buf.length = strlen(tmp_buf.value);
  gss_import_name(&minor_status, &tmp_buf, GSS_C_NT_HOSTBASED_SERVICE, &gss_server);

  // We are using SPNEGO, so set this mech OID
  mechs.elements = gss_mech_spnego;
  mechs.count = 1;
  mechsp = &mechs;

  tmp_buf.value   = password;
  tmp_buf.length  = strlen(password);

  // major_status = gss_acquire_cred_with_password(&minor_status,
  //                                               gss_user,
  //                                               &tmp_buf,
  //                                               GSS_C_INDEFINITE,
  //                                               GSS_C_NO_OID_SET,
  //                                               GSS_C_INITIATE,
  //                                               &creds, NULL, NULL);
  // smb_gss_display_status(major_status, minor_status, GSS_C_NO_OID);

  major_status = gss_acquire_cred(&minor_status,
                                  GSS_C_NO_NAME,
                                  0,
                                  GSS_C_NO_OID_SET,
                                  GSS_C_INITIATE,
                                  &creds, NULL, NULL);
  smb_gss_display_status(major_status, minor_status, GSS_C_NO_OID);

  input_buf.value   = NULL;
  input_buf.length  = 0;

  gss_OID_set supported;

  req_mech_type = gss_mech_spnego;

  gss_inquire_names_for_mech(&minor_status, gss_mech_spnego, &supported);

  //gss_acquire_cred(cname, GSS_C_INITIATE);

  major_status = gss_init_sec_context(&minor_status,
    GSS_C_NO_CREDENTIAL,
    &s->gss.ctx,
    gss_server,
    req_mech_type,
    GSS_C_MUTUAL_FLAG | GSS_C_DELEG_FLAG,
    GSS_C_INDEFINITE,
    GSS_C_NO_CHANNEL_BINDINGS,
    &input_buf,//&s->gss.spnego,
    &actual_mech_type,
    &output_buf,
    NULL, NULL);
  //major_status  = gss_accept_sec_conten

  smb_gss_display_status(major_status, minor_status, req_mech_type);
}

int             smb_session_login_gss2(smb_session *s, const char *domain,
                                      const char *user, const char *password)
{
  OM_uint32         major_status, minor_status;
  gss_buffer_desc   user_buf, pass_buf;
  gss_name_t        gss_user = GSS_C_NO_NAME;
  gss_OID_set_desc  mechs, *mechsp = GSS_C_NO_OID_SET;


  assert(s != NULL && user != NULL && password != NULL);

  // We are using SPNEGO, so set this mech OID
  mechs.elements = gss_mech_spnego;
  mechs.count = 1;
  mechsp = &mechs;

  // Filling buffers with user and pass
  user_buf.value  = user;
  user_buf.length = strlen(user);
  pass_buf.value  = password;
  pass_buf.length = strlen(password);

  major_status = gss_import_name(&minor_status, &user_buf,
                                 (gss_OID) GSS_C_NT_USER_NAME, &gss_user);
  if (major_status == GSS_S_COMPLETE)
    BDSM_dbg("GSSAPI: Imported name\n");
  else
  {
    BDSM_dbg("GSSAPI: Unable to import name\n");
    return (0);
  }

  s->gss.credentials = GSS_C_NO_CREDENTIAL;
  major_status = gss_acquire_cred_with_password(&minor_status,
                                                gss_user,
                                                &pass_buf, 0,
                                                mechsp, GSS_C_ACCEPT,
                                                &s->gss.credentials,
                                                NULL, NULL);
  if (major_status == GSS_S_COMPLETE)
    BDSM_dbg("GSSAPI: Acquired credentials\n");
  else
  {
    BDSM_dbg("GSSAPI: Unable to acquire credentials: 0x%x, 0x%x\n",
             major_status, minor_status);
    return (0);
  }

  return(1);
}
