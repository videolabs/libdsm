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
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "bdsm_debug.h"
#include "netbios_ns.h"
#include "netbios_query.h"
#include "netbios_utils.h"


static int    ns_open_socket(netbios_ns *ns)
{
    int sock_opt;

    if ((ns->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        goto error;

    sock_opt = 1;
    if (setsockopt(ns->socket, SOL_SOCKET, SO_BROADCAST,
                   (void *)&sock_opt, sizeof(sock_opt)) < 0)
        goto error;

    sock_opt = 0;
    if (setsockopt(ns->socket, IPPROTO_IP, IP_MULTICAST_LOOP,
                   (void *)&sock_opt, sizeof(sock_opt)) < 0)
        goto error;

    ns->addr.sin_family       = AF_INET;
    ns->addr.sin_port         = htons(0);
    ns->addr.sin_addr.s_addr  = 0;
    if (bind(ns->socket, (struct sockaddr *)&ns->addr, sizeof(ns->addr)) < 0)
        goto error;

    return (1);

error:
    BDSM_perror("netbios_ns_new, open_socket: ");
    return (0);
}

netbios_ns  *netbios_ns_new()
{
    netbios_ns  *ns;

    assert(ns = malloc(sizeof(netbios_ns)));
    memset((void *)ns, 0, sizeof(netbios_ns));

    if (!ns_open_socket(ns))
    {
        netbios_ns_destroy(ns);
        return (0);
    }

    ns->entries       = NULL;
    ns->last_trn_id   = rand();

    return (ns);
}

void          netbios_ns_destroy(netbios_ns *ns)
{
    if (!ns)
        return;

    netbios_ns_clear(ns);

    close(ns->socket);
    free(ns);
}

static int        netbios_ns_send_query(netbios_ns *ns, netbios_query *q,
                                        uint32_t ip)
{
    struct sockaddr_in  addr;
    ssize_t             sent;
    uint16_t            trn_id;

    assert(ns != NULL && q != NULL);

    trn_id = ns->last_trn_id + 1; // Increment transaction ID, not to reuse them
    q->packet->trn_id = htons(trn_id);

    addr.sin_addr.s_addr  = ip;
    addr.sin_family       = AF_INET;
    addr.sin_port         = htons(NETBIOS_PORT_NAME);

    sent = sendto(ns->socket, (void *)q->packet,
                  sizeof(netbios_query_packet) + q->cursor, 0,
                  (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    netbios_query_destroy(q);

    if (sent < 0)
    {
        BDSM_perror("netbios_ns_send_query: ");
        return (0);
    }

    ns->last_trn_id = trn_id; // Remember the last transaction id.
    return (1);
}

static ssize_t    netbios_ns_recv(int sock, void *buf, size_t buf_size,
                                  struct timeval *timeout, struct sockaddr *addr,
                                  socklen_t *addr_len)
{
    fd_set        read_fds, error_fds;
    int           res;

    assert(sock && buf != NULL && buf_size > 0);

    FD_ZERO(&read_fds);
    FD_ZERO(&error_fds);
    FD_SET(sock, &read_fds);
    FD_SET(sock, &error_fds);

    res = select(sock + 1, &read_fds, 0, &error_fds, timeout);

    if (res < 0)
        goto error;
    if (FD_ISSET(sock, &error_fds))
        goto error;

    if (FD_ISSET(sock, &read_fds))
    {
        return (recvfrom(sock, buf, buf_size, 0, addr, addr_len));
    }
    else
        return (0);

error:
    BDSM_perror("netbios_ns_recv: ");
    return (-1);
}

int      netbios_ns_resolve(netbios_ns *ns, const char *name, char type, uint32_t *addr)
{
    netbios_ns_entry    *cached;
    struct timeval      timeout;
    netbios_query       *q;
    char                *encoded_name;
    char                footer[4] = { 0x00, 0x20, 0x00, 0x01 };
    char                recv_buffer[512]; // Hu ?
    ssize_t             recv;
    uint32_t            ip;


    assert(ns != NULL);


    if ((cached = netbios_ns_entry_find(ns, name, 0)) != NULL)
    {
        *addr = cached->address.s_addr;
        return (1);
    }

    if ((encoded_name = netbios_name_encode(name, 0, type)) == NULL)
        return (0);

    // Prepare packet
    q = netbios_query_new(34 + 4, 1, NETBIOS_OP_NAME_QUERY);
    netbios_query_set_flag(q, NETBIOS_FLAG_RECURSIVE, 1);
    netbios_query_set_flag(q, NETBIOS_FLAG_BROADCAST, 1);

    // Append the queried name to the packet
    netbios_query_append(q, encoded_name, strlen(encoded_name) + 1);

    // Magic footer (i.e. Question type (Netbios) / class (IP)
    netbios_query_append(q, footer, 4);
    q->packet->queries = htons(1);

    // We broadcast this query
    inet_aton("255.255.255.255", (struct in_addr *)&ip);

    // Let's send it
    if (!netbios_ns_send_query(ns, q, ip))
        return (0);
    else
        BDSM_dbg("netbios_ns_resolve, name query sent for '%s' !\n", name);

    free(encoded_name);

    // Now wait for a reply and pray
    timeout.tv_sec = 2;
    timeout.tv_usec = 420;
    recv = netbios_ns_recv(ns->socket, (void *)recv_buffer, 512, &timeout, 0, 0);

    if (recv <= 0)
        BDSM_perror("netbios_ns_resolve:");
    else if (recv == 0)
        BDSM_dbg("netbios_ns_resolve, received NO reply for '%s' !\n", name);
    else
    {
        BDSM_dbg("netbios_ns_resolve, received a reply for '%s' !\n", name);
        *addr = (*(uint32_t *)(recv_buffer + recv - 4));
        return (1);
    }

    return (0);
}

// We have a small recursive function for discovery, to stack received reply
// when descending, and performing reverse lookup when ascending
static void   netbios_ns_discover_rec(netbios_ns *ns, struct timeval *timeout,
                                      void *recv_buffer)
{
    struct sockaddr_in  recv_addr;
    socklen_t           recv_addr_len;
    int                 res;

    recv_addr_len = sizeof(recv_addr);
    res = netbios_ns_recv(ns->socket, recv_buffer, 256, timeout,
                          (struct sockaddr *)&recv_addr, &recv_addr_len);
    if (res > 0 && timeout->tv_sec && timeout->tv_usec)
    {
        netbios_ns_discover_rec(ns, timeout, recv_buffer);

        BDSM_dbg("Discover: received a reply from %s\n",
                 inet_ntoa(recv_addr.sin_addr));
        netbios_ns_inverse(ns, recv_addr.sin_addr.s_addr);
    }
}

int           netbios_ns_discover(netbios_ns *ns)
{
    const char  broadcast_name[] = NETBIOS_WILDCARD;
    char        footer[4]        = { 0x00, 0x20, 0x00, 0x01 };

    struct timeval      timeout;
    netbios_query     *q;
    char                recv_buffer[256]; // Hu ?
    uint32_t            ip;

    assert(ns != NULL);

    //
    // First step, we broadcast a packet to receive a message from every
    // NETBIOS nodes on the local network
    //
    q = netbios_query_new(34 + 4, 1, NETBIOS_OP_NAME_QUERY);
    // Append the queried name to the packet
    netbios_query_append(q, broadcast_name, strlen(broadcast_name) + 1);
    // Magic footer (i.e. Question type (Netbios) / class (IP)
    netbios_query_append(q, footer, 4);
    q->packet->queries = htons(1);

    // We broadcast this query
    inet_aton("255.255.255.255", (struct in_addr *)&ip);

    // Let's send it
    if (!netbios_ns_send_query(ns, q, ip))
    {
        BDSM_dbg("Unable to send netbios 'discovery query'.\n");
        return (0);
    }
    else
        BDSM_dbg("netbios_ns_discover, name query sent for '*'.\n");



    //
    // Second step, we list every IP that answered to our broadcast.
    //
    timeout.tv_sec = 2;
    timeout.tv_usec = 420;
    netbios_ns_discover_rec(ns, &timeout, (void *)recv_buffer);

    return (1);
}

// Perform inverse name resolution. Grap an IP and return the first <20> field
// returned by the host
const char        *netbios_ns_inverse(netbios_ns *ns, uint32_t ip)
{
    const char  broadcast_name[] = NETBIOS_WILDCARD;
    char        footer[4]        = { 0x00, 0x21, 0x00, 0x01 }; // NBSTAT/IP

    netbios_ns_entry  *cached;
    struct timeval      timeout;
    netbios_query     *q;
    char                recv_buffer[512]; // Hu ?
    ssize_t             recv;

    assert(ns != NULL && ip != 0);

    if ((cached = netbios_ns_entry_find(ns, NULL, ip)) != NULL)
        return (cached->name);

    // Prepare NBSTAT query packet
    q = netbios_query_new(34 + 4, 1, NETBIOS_OP_NAME_QUERY);
    netbios_query_append(q, broadcast_name, strlen(broadcast_name) + 1);
    netbios_query_append(q, footer, 4);
    q->packet->queries = htons(1);

    // Let's send it
    if (!netbios_ns_send_query(ns, q, ip))
        return (NULL);
    else
        BDSM_dbg("netbios_ns_inverse, reverse name query sent for '%s' !\n",
                 inet_ntoa(*(struct in_addr *)&ip));

    // Now wait for a reply and pray
    timeout.tv_sec = 1;
    timeout.tv_usec = 500;
    recv = netbios_ns_recv(ns->socket, (void *)recv_buffer, 512, &timeout, 0, 0);

    if (recv <= 0)
        goto error;
    else
        BDSM_dbg("netbios_ns_inverse, received a reply for '%s' !\n",
                 inet_ntoa(*(struct in_addr *)&ip));


    // Now we've got something, let's find the <20>/<0> name
    netbios_query_packet  *p = (netbios_query_packet *)recv_buffer;
    uint8_t                 name_count;
    uint8_t                 name_idx;
    char                    *names;
    char                    *current_name;
    char                    current_type;
    netbios_ns_entry      *entry = NULL, *res = NULL;

    BDSM_dbg("Queried name length: %u\n", p->payload[0]);
    name_count = p->payload[p->payload[0] + 12];
    BDSM_dbg("Number of names: %hhu\n", name_count);
    names = p->payload + p->payload[0] + 13;

    for (name_idx = 0; name_idx < name_count; name_idx++)
    {
        current_name = names + name_idx * 18;
        current_type = current_name[15];

        BDSM_dbg("Found name : %s (type == 0x%x)\n", current_name, current_type);
        if (current_type == 0x20 || current_type == 0)
            entry = netbios_ns_entry_add(ns, current_name, current_type, ip);
        if (current_type == 0x20)
            res = entry;
    }

    if (res)        // We prefer a <20> name.
        return (res->name);
    else if (entry) // Did we found a <0> or <20> name ?
        return (entry->name);
    else
        return (NULL);

error:
    BDSM_perror("netbios_ns_inverse: ");
    return (NULL);
}

