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

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "bdsm_debug.h"
#include "netbios_session.h"
#include "netbios_utils.h"

static int        open_socket_and_connect(netbios_session *s)
{
    if ((s->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        goto error;
    if (connect(s->socket, (struct sockaddr *)&s->remote_addr, sizeof(s->remote_addr)) <0)
        goto error;

    return (1);

error:
    BDSM_perror("netbios_session_new, open_socket: ");
    return (0);
}

static int        session_buffer_realloc(netbios_session *s, size_t new_size)
{
    void        *new_ptr;

    assert(s != NULL);

    /* BDSM_dbg("session_buffer_realloc: from %ld bytes to %ld bytes\n", */
    /*          s->packet_recv_payload_size, new_size); */

    new_ptr  = realloc(s->packet_recv, new_size);
    if (new_ptr != NULL)
    {
        s->packet_recv_payload_size = new_size;
        s->packet_recv = new_ptr;
        return (1);
    } else
        free(s->packet_recv);
    return (0);
}

netbios_session *netbios_session_new(size_t buf_size)
{
    netbios_session   *session;
    size_t            packet_size;

    session = (netbios_session *)calloc(1, sizeof(netbios_session));
    if (!session)
        return NULL;

    session->packet_send_payload_size = buf_size;
    packet_size = sizeof(netbios_session_packet) + session->packet_send_payload_size;
    session->packet_send = (netbios_session_packet *)malloc(packet_size);
    if (!session->packet_send) {
        free(session);
        return NULL;
    }

    session->packet_recv_payload_size = buf_size;
    packet_size = sizeof(netbios_session_packet) + session->packet_recv_payload_size;
    session->packet_recv = (netbios_session_packet *)malloc(packet_size);
    if (!session->packet_recv) {
        free(session->packet_send);
        free(session);
        return NULL;
    }

    return (session);
}

void              netbios_session_destroy(netbios_session *s)
{
    if (!s)
        return;
    close(s->socket);

    free(s->packet_send);
    free(s->packet_recv);
    free(s);
}

int               netbios_session_connect(struct in_addr *addr,
        netbios_session *s,
        const char *name,
        int direct_tcp)
{
    ssize_t                   recv_size;
    char                      *encoded_name;

    assert(s != NULL && s->packet_send != NULL && s->packet_recv != NULL);

    if (direct_tcp)
        s->remote_addr.sin_port       = htons(NETBIOS_PORT_DIRECT);
    else
        s->remote_addr.sin_port       = htons(NETBIOS_PORT_SESSION);
    s->remote_addr.sin_family       = AF_INET;
    s->remote_addr.sin_addr.s_addr  = addr->s_addr;
    if (!open_socket_and_connect(s))
        goto error;

    if (!direct_tcp)
    {
        // Send the Session Request message
        netbios_session_packet_init(s);
        s->packet_send->opcode = NETBIOS_OP_SESSION_REQ;
        encoded_name = netbios_name_encode(name, 0, NETBIOS_FILESERVER);
        netbios_session_packet_append(s, encoded_name, strlen(encoded_name) + 1);
        free(encoded_name);
        encoded_name = netbios_name_encode("LIBDSM", 0, NETBIOS_WORKSTATION);
        netbios_session_packet_append(s, encoded_name, strlen(encoded_name) + 1);
        free(encoded_name);

        s->state = NETBIOS_SESSION_CONNECTING;
        if (!netbios_session_packet_send(s))
            goto error;

        // Now receiving the reply from the server.
        recv_size = netbios_session_packet_recv(s, NULL);
        if (recv_size < 0)
            goto error;

        // Reply was negative, we are not connected :(
        if (s->packet_recv->opcode != NETBIOS_OP_SESSION_REQ_OK)
        {
            s->state = NETBIOS_SESSION_REFUSED;
            return (0);
        }
    }

    // Reply was OK or DirectTCP, a session has been established
    s->state = NETBIOS_SESSION_CONNECTED;
    return (1);

error:
    s->state = NETBIOS_SESSION_ERROR;
    return (0);
}

void              netbios_session_packet_init(netbios_session *s)
{
    assert(s != NULL);

    s->packet_send_cursor        = 0;
    s->packet_send->flags        = 0;
    s->packet_send->opcode       = NETBIOS_OP_SESSION_MSG;
    s->packet_recv_next_offset   = 0;
    s->packet_recv_size_next_msg = 0;
    s->packet_recv->flags        = 0;
    s->packet_recv->opcode       = NETBIOS_OP_SESSION_MSG;
}

int               netbios_session_packet_append(netbios_session *s,
        const char *data, size_t size)
{
    char  *start;

    assert(s && s->packet_send);

    if (s->packet_send_payload_size - s->packet_send_cursor < size)
        return (0);

    start = ((char *)&s->packet_send->payload) + s->packet_send_cursor;
    memcpy(start, data, size);
    s->packet_send_cursor += size;

    return (1);
}

int               netbios_session_packet_send(netbios_session *s)
{
    ssize_t         to_send;
    ssize_t         sent;

    assert(s && s->packet_send && s->socket && s->state > 0);

    s->packet_send->length = htons(s->packet_send_cursor);
    to_send                = sizeof(netbios_session_packet) + s->packet_send_cursor;
    sent                   = send(s->socket, (void *)s->packet_send, to_send, 0);

    if (sent != to_send)
    {
        BDSM_perror("netbios_session_packet_send: Unable to send (full?) packet");
        return (0);
    }

    return (sent);
}

static ssize_t    netbios_session_get_data_from_socket(netbios_session *s, ssize_t offset)
{
    ssize_t         res;
    size_t          total, sofar;

    assert(s != NULL && s->packet_recv != NULL && s->socket && s->state > 0);

    res = recv(s->socket, (void *)(s->packet_recv) + offset, s->packet_recv_payload_size - offset, 0);
    if (res < 0)
    {
        BDSM_perror("netbios_session_packet_recv: ");
        return (-1);
    }

    total  = ntohs(s->packet_recv->length);
    total |= (s->packet_recv->flags & 0x01) << 16;
    sofar  = offset + res - sizeof(netbios_session_packet);

    if (total + sizeof(netbios_session_packet) > s->packet_recv_payload_size)
        if (!session_buffer_realloc(s, total + sizeof(netbios_session_packet)))
            return (-1);

    //BDSM_dbg("Total = %ld, sofar = %ld\n", total, sofar);

    while (sofar < total)
    {
        res = recv(s->socket, (void *)(s->packet_recv) + 4 + sofar, total - sofar, 0);
//        BDSM_dbg("Total = %ld, sofar = %ld, res = %ld\n", total, sofar, res);

        if (res < 0)
        {
            BDSM_perror("netbios_session_packet_recv: ");
            return (-1);
        }
        sofar += res;
    }

    // Check if we did received bytes of next message
    if (sofar > total)
    {
        s->packet_recv_next_offset = total + sizeof(netbios_session_packet);
        s->packet_recv_size_next_msg = sofar - total;
        BDSM_dbg("netbios_session_packet_recv: Received %ld bytes of next message\n",
                 s->packet_recv_size_next_msg);
    }
    else
    {
        s->packet_recv_next_offset = 0;
        s->packet_recv_size_next_msg = 0;
    }
    return sofar;
}

static ssize_t    netbios_session_get_next_packet(netbios_session *s)
{
    size_t     size;
    size_t     total;

    if (s->packet_recv_size_next_msg != 0)
    {
        // Move previously received parts of the next message at the beginning of buffer
        memmove((void *)s->packet_recv,
                ((void *)s->packet_recv) + s->packet_recv_next_offset,
                s->packet_recv_size_next_msg);

        // Check if next message if complete or not
        total  = ntohs(s->packet_recv->length);
        total |= (s->packet_recv->flags & 0x01) << 16;
        if (total + sizeof(netbios_session_packet) > s->packet_recv_size_next_msg)
        {
            // We don't have full message in buffer, we need to get data from socket
            size = netbios_session_get_data_from_socket(s, s->packet_recv_size_next_msg);
        }
        else
        {
            // Next message is complete, we don't need to get data from socket !
            size = total;

            // Update info about possible next message
            s->packet_recv_next_offset = size + sizeof(netbios_session_packet);
            s->packet_recv_size_next_msg -= s->packet_recv_next_offset;
        }
    }
    else
        size = netbios_session_get_data_from_socket(s, s->packet_recv_size_next_msg);

    return (size);
}

ssize_t           netbios_session_packet_recv(netbios_session *s, void **data)
{
    ssize_t         size;

    size = netbios_session_get_next_packet(s);

    // If received message is a keepalive, ignore message and get next one
    while (s->packet_recv->opcode == NETBIOS_OP_SESSION_KEEPALIVE)
    {
        size = netbios_session_get_next_packet(s);
    }

    if ((size > 0) && (data != NULL))
        *data = (void *) s->packet_recv->payload;

    return (size);
}
