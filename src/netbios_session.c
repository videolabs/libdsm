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

#include "../xcode/config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#import <sys/poll.h>

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>

#include "smb_defs.h"
#include "bdsm_debug.h"
#include "compat.h"
#include "netbios_session.h"
#include "netbios_utils.h"

//Set blocking IO on the socket
static void set_blocking_io(netbios_session *s)
{
    int arg = fcntl(s->socket, F_GETFL, NULL);
    arg &= (~O_NONBLOCK);
    fcntl(s->socket, F_SETFL, arg);
}

static int open_socket_and_connect(netbios_session *s)
{
    
    bool restore_blocking_io = false;
    
    if ((s->socket = socket(s->remote_addr->ai_family, SOCK_STREAM, 0)) < 0){
        goto error;
    }
    
    // Prevent SIGPIPE signals
    int nosigpipe = 1;
    setsockopt(s->socket, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(nosigpipe));
    
    // Enable non-blocking IO on the socket
    int result = fcntl(s->socket, F_SETFL, O_NONBLOCK);
    
    if (result == -1){
        goto error;
    }
    else{
        restore_blocking_io = true;
    }
    
    connect(s->socket, s->remote_addr->ai_addr , s->remote_addr->ai_addrlen);
    
    fd_set fdset;
    struct timeval tv;
    FD_ZERO(&fdset);
    FD_SET(s->socket, &fdset);
    tv.tv_sec = DSM_CONNECT_TIMEOUT;
    tv.tv_usec = 0;
    
    if (select((s->socket) + 1, NULL, &fdset, NULL, &tv) == 1){
        int so_error;
        socklen_t len = sizeof so_error;
        getsockopt(s->socket, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error == 0) {
            ;
        }
        else{
            goto error;
        }
    }
    else{
        goto error;
    }
    
    set_blocking_io(s);
    
    return DSM_SUCCESS;

error:
    
    if(restore_blocking_io){
        set_blocking_io(s);
    }
    
    BDSM_perror("netbios_session_new, open_socket: ");
    return DSM_ERROR_NETWORK;
}



static int        session_buffer_realloc(netbios_session *s, size_t new_size)
{
    void        *new_ptr;

    bdsm_assert(s != NULL);
    
    if(s != NULL){

        /* BDSM_dbg("session_buffer_realloc: from %ld bytes to %ld bytes\n", */
        /*          s->packet_payload_size, new_size); */

        new_ptr  = realloc(s->packet, new_size);
        if (new_ptr != NULL)
        {
            s->packet_payload_size = new_size;
            s->packet = new_ptr;
            return 1;
        }
        free(s->packet);
        s->packet = NULL;
        return 0;
        
    }
    return 0;
}

netbios_session *netbios_session_new(size_t buf_size)
{
    netbios_session   *session;
    size_t            packet_size;

    session = (netbios_session *)calloc(1, sizeof(netbios_session));
    if (!session)
        return NULL;

    session->packet_payload_size = buf_size;
    packet_size = sizeof(netbios_session_packet) + session->packet_payload_size;
    session->packet = (netbios_session_packet *)malloc(packet_size);
    if (!session->packet) {
        free(session);
        return NULL;
    }
    session->socket = -1;

    return session;
}

void              netbios_session_destroy(netbios_session *s)
{
    if (!s)
        return;
    if (s->socket != -1)
        closesocket(s->socket);

    free(s->packet);
    free(s);
}

int netbios_session_connect(const char *ip, const char *user_port, netbios_session *s,
                            const char *name, int direct_tcp)
{
    ssize_t                   recv_size;
    char                      *encoded_name = NULL;
    char                      *ports[3];
    unsigned int              nb_ports;
    bool                      opened = false;

    bdsm_assert(s != NULL && s->packet != NULL);

    if(s != NULL && s->packet != NULL){
    
        if (direct_tcp)
        {
            ports[0] = NETBIOS_PORT_DIRECT;
            ports[1] = NETBIOS_PORT_DIRECT_SECONDARY;
            nb_ports = 2;
            if(user_port!=NULL){
                ports[2] = (char *)user_port;
                nb_ports++;
            }
        }
        else
        {
            ports[0] = NETBIOS_PORT_SESSION;
            nb_ports = 1;
            if(user_port!=NULL){
                ports[1] = (char *)user_port;
                nb_ports++;
            }
        }
        
        for (unsigned int i = 0; i < nb_ports && !opened; ++i)
        {

            struct addrinfo hints;
            struct addrinfo *rp, *result;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_flags = AI_ADDRCONFIG;
            hints.ai_socktype = SOCK_STREAM;
            
            int getaddrinfoerror = getaddrinfo(ip, ports[i], &hints, &result);
            if (getaddrinfoerror) {
                goto error;
            }
            
            for (rp = result; rp; rp = rp->ai_next) {
                s->remote_addr = rp;
                opened = (open_socket_and_connect(s) == DSM_SUCCESS);
                if(opened){
                    break;
                }
            }
            
            freeaddrinfo(result);
            
        }

        if (!opened)
            goto error;

        if (!direct_tcp)
        {
            // Send the Session Request message
            netbios_session_packet_init(s);
            s->packet->opcode = NETBIOS_OP_SESSION_REQ;
            encoded_name = netbios_name_encode(name, 0, NETBIOS_FILESERVER);
            if (!netbios_session_packet_append(s, encoded_name, strlen(encoded_name) + 1))
                goto error;
            free(encoded_name);
            encoded_name = netbios_name_encode("LIBDSM", 0, NETBIOS_WORKSTATION);
            if (!netbios_session_packet_append(s, encoded_name, strlen(encoded_name) + 1))
                goto error;
            free(encoded_name);
            encoded_name = NULL;

            s->state = NETBIOS_SESSION_CONNECTING;
            if (!netbios_session_packet_send(s))
                goto error;

            // Now receiving the reply from the server.
            recv_size = netbios_session_packet_recv(s, NULL);
            if (recv_size < 0)
                goto error;

            // Reply was negative, we are not connected :(
            if (s->packet->opcode != NETBIOS_OP_SESSION_REQ_OK)
            {
                s->state = NETBIOS_SESSION_REFUSED;
                return 0;
            }
        }

        // Reply was OK or DirectTCP, a session has been established
        s->state = NETBIOS_SESSION_CONNECTED;
        return 1;

    error:
        free(encoded_name);
        s->state = NETBIOS_SESSION_ERROR;
        return 0;
    }
    
    return 0;
}

void              netbios_session_packet_init(netbios_session *s)
{
    bdsm_assert(s != NULL);

    if(s!=NULL){
        s->packet_cursor  = 0;
        s->packet->flags  = 0;
        s->packet->opcode = NETBIOS_OP_SESSION_MSG;
    }
}

int               netbios_session_packet_append(netbios_session *s,
        const char *data, size_t size)
{
    char  *start;

    bdsm_assert(s && s->packet);

    if(s && s->packet){
    
        if (s->packet_payload_size - s->packet_cursor < size)
            if (!session_buffer_realloc(s, size + s->packet_cursor))
                return 0;

        start = ((char *)&s->packet->payload) + s->packet_cursor;
        memcpy(start, data, size);
        s->packet_cursor += size;

        return 1;
    }
    return 0;
}

int               netbios_session_packet_send(netbios_session *s)
{
    ssize_t         to_send;
    ssize_t         sent;

    bdsm_assert(s && s->packet && s->socket >= 0 && s->state > 0);

    if(s && s->packet && s->socket >= 0 && s->state > 0){
    
        s->packet->length = htons(s->packet_cursor);
        to_send           = sizeof(netbios_session_packet) + s->packet_cursor;
        
        // Set write timeout
        struct timeval write_tv;
        write_tv.tv_sec = DSM_WRITE_TIMEOUT;
        write_tv.tv_usec = 0;
        ;
        
        if(setsockopt(s->socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&write_tv, sizeof write_tv)<0)
        {
            BDSM_perror("netbios_session_packet_send: error setting send timeout");
            return 0;
        }
        
        sent = send(s->socket, (void *)s->packet, to_send, 0);

        if (sent != to_send)
        {
            BDSM_perror("netbios_session_packet_send: Unable to send (full?) packet");
            return 0;
        }

        return (int)sent;
         
    }
    return 0;
}

int socket_set_recv_timeout(netbios_session *s)
{
    //set read timeout
    struct timeval read_tv;
    read_tv.tv_sec = DSM_READ_TIMEOUT;
    read_tv.tv_usec = 0;
    
    if(setsockopt(s->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&read_tv, sizeof read_tv)<0){
        return -1;
    }
    
    int socketFD = s->socket;
    int result = -1;
    
    struct pollfd pollfd[1];
    pollfd->fd = socketFD;
    pollfd->events = POLLIN;
    pollfd->revents = 0;
    
    //pool: wait for available data
    result = poll(pollfd, 1, DSM_READ_TIMEOUT*1000);

    if(result < 0) {
        BDSM_perror("netbios_session_packet_recv: error during poll()");
        return -1;
    }
    
    if(result == 0) {
        BDSM_perror("netbios_session_packet_recv: timeout during poll()");
        return -1;
    }
    
    return 0;
}

static ssize_t    netbios_session_get_next_packet(netbios_session *s)
{
    ssize_t         res;
    size_t          total, sofar;

    bdsm_assert(s != NULL && s->packet != NULL && s->socket >= 0 && s->state > 0);

    if(s != NULL && s->packet != NULL && s->socket >= 0 && s->state > 0){
        
        // Only get packet header and analyze it to get only needed number of bytes
        // needed for the packet. This will prevent losing a part of next packet
        total = sizeof(netbios_session_packet);
        sofar = 0;
        
        if(socket_set_recv_timeout(s)==-1){
            return -1;
        }
        
        while (sofar < total)
        {
            res = recv(s->socket, (uint8_t *)(s->packet) + sofar, total - sofar, 0);
            if (res <= 0)
            {
                BDSM_perror("netbios_session_packet_recv: ");
                return -1;
            }
            sofar += res;
        }

        total  = ntohs(s->packet->length);
        total |= (s->packet->flags & 0x01) << 16;
        sofar  = 0;

        if (total + sizeof(netbios_session_packet) > s->packet_payload_size
         && !session_buffer_realloc(s, total + sizeof(netbios_session_packet)))
            return -1;

        //BDSM_dbg("Total = %ld, sofar = %ld\n", total, sofar);
        
        while (sofar < total)
        {
            res = recv(s->socket, (uint8_t *)(s->packet) + sizeof(netbios_session_packet)
                       + sofar, total - sofar, 0);
            //BDSM_dbg("Total = %ld, sofar = %ld, res = %ld\n", total, sofar, res);

            if (res <= 0)
            {
                BDSM_perror("netbios_session_packet_recv: ");
                return -1;
            }
            sofar += res;
        }

        if (sofar > total)
        {
            BDSM_dbg("netbios_session_packet_recv: Packet size mismatch (%ld/%ld)\n",
                     sofar, total);
            return -1;
        }

        return sofar;
        
    }
    return -1;
}

ssize_t           netbios_session_packet_recv(netbios_session *s, void **data)
{
    ssize_t         size;

    // ignore keepalive messages if needed
    do
    {
        size = netbios_session_get_next_packet(s);
    } while (size >= 0 && s->packet->opcode == NETBIOS_OP_SESSION_KEEPALIVE);

    if ((size >= 0) && (data != NULL))
        *data = (void *) s->packet->payload;

    return size;
}
