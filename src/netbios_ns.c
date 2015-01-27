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
#include <fcntl.h>
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

enum name_query_type {
    NAME_QUERY_TYPE_INVALID,
    NAME_QUERY_TYPE_NB,
    NAME_QUERY_TYPE_NBSTAT
};
static char name_query_broadcast[] = NETBIOS_WILDCARD;

static netbios_ns_entry *netbios_ns_inverse_internal(netbios_ns *ns,
                                                     uint32_t ip);

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

static int    ns_open_abort_pipe(netbios_ns *ns)
{
    int flags;

    if (pipe(ns->abort_pipe) == -1)
        return -1;

    if ((flags = fcntl(ns->abort_pipe[0], F_GETFL, 0)) == -1)
        return -1;

    if (fcntl(ns->abort_pipe[0], F_SETFL, flags | O_NONBLOCK) == -1)
        return -1;

    return 0;
}

static void   ns_close_abort_pipe(netbios_ns *ns)
{
    if (ns->abort_pipe[0] != -1 && ns->abort_pipe[1] != -1)
    {
        close(ns->abort_pipe[0]);
        close(ns->abort_pipe[1]);
        ns->abort_pipe[0] = ns->abort_pipe[1] -1;
    }
}

static int netbios_ns_send_name_query(netbios_ns *ns,
                                      uint32_t ip,
                                      enum name_query_type type,
                                      const char *name,
                                      uint16_t query_flag)
{
    struct sockaddr_in  addr;
    ssize_t             sent;
    uint16_t            trn_id;
    const char          *query_type;
    static char         query_type_nb[4] = { 0x00, 0x20, 0x00, 0x01 };
    static char         query_type_nbstat[4] = { 0x00, 0x21, 0x00, 0x01 };
    netbios_query       *q;

    assert(ns != NULL);

    switch (type)
    {
        case NAME_QUERY_TYPE_NB:
            query_type = query_type_nb;
            break;
        case NAME_QUERY_TYPE_NBSTAT:
            query_type = query_type_nbstat; // NBSTAT/IP;
            break;
        default:
            BDSM_dbg("netbios_ns_send_name_query: unknow name_query_type");
            return -1;
    }

    // Prepare packet
    q = netbios_query_new(34 + 4, 1, NETBIOS_OP_NAME_QUERY);
    if (query_flag)
        netbios_query_set_flag(q, query_flag, 1);

    // Append the queried name to the packet
    netbios_query_append(q, name, strlen(name) + 1);

    // Magic footer (i.e. Question type (Netbios) / class (IP)
    netbios_query_append(q, query_type, 4);
    q->packet->queries = htons(1);

    if (ip == 0)
        inet_aton("255.255.255.255", (struct in_addr *)&ip);

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
        BDSM_perror("netbios_ns_send_name_query: ");
        return -1;
    }
    else
        BDSM_dbg("netbios_ns_send_name_query, name query sent for '*'.\n");

    ns->last_trn_id = trn_id; // Remember the last transaction id.
    return 0;
}

netbios_ns  *netbios_ns_new()
{
    netbios_ns  *ns;

    ns = calloc(1, sizeof(netbios_ns));
    if (!ns)
        return NULL;
    ns->abort_pipe[0] = ns->abort_pipe[1] -1;

    if (!ns_open_socket(ns) || ns_open_abort_pipe(ns) == -1)
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

    ns_close_abort_pipe(ns);

    free(ns);
}

static ssize_t    netbios_ns_recv(int sock, int abort_fd,
                                  void *buf, size_t buf_size,
                                  struct timeval *timeout, struct sockaddr *addr,
                                  socklen_t *addr_len)
{
    fd_set        read_fds, error_fds;
    int           res, nfds;

    assert(sock && buf != NULL && buf_size > 0);

    FD_ZERO(&read_fds);
    FD_ZERO(&error_fds);
    FD_SET(sock, &read_fds);
    FD_SET(abort_fd, &read_fds);
    FD_SET(sock, &error_fds);
    nfds = (sock > abort_fd ? sock : abort_fd) + 1;

    res = select(nfds, &read_fds, 0, &error_fds, timeout);

    if (res < 0)
        goto error;
    if (FD_ISSET(sock, &error_fds))
        goto error;

    if (FD_ISSET(abort_fd, &read_fds))
    {
        return (-1);
    }
    else if (FD_ISSET(sock, &read_fds))
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
    char                *encoded_name;
    char                recv_buffer[512]; // Hu ?
    ssize_t             recv;


    assert(ns != NULL);


    if ((cached = netbios_ns_entry_find(ns, name, 0)) != NULL)
    {
        *addr = cached->address.s_addr;
        return (1);
    }

    if ((encoded_name = netbios_name_encode(name, 0, type)) == NULL)
        return (0);

    if (netbios_ns_send_name_query(ns, 0, NAME_QUERY_TYPE_NB, encoded_name,
                                   NETBIOS_FLAG_RECURSIVE |
                                   NETBIOS_FLAG_BROADCAST) == -1)
    {
        free(encoded_name);
        return (0);
    }
    free(encoded_name);

    // Now wait for a reply and pray
    timeout.tv_sec = 2;
    timeout.tv_usec = 420;
    recv = netbios_ns_recv(ns->socket, ns->abort_pipe[0],
                           (void *)recv_buffer, 512, &timeout, 0, 0);

    if (recv < 0)
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
    res = netbios_ns_recv(ns->socket, ns->abort_pipe[0],
                          recv_buffer, 256, timeout,
                          (struct sockaddr *)&recv_addr, &recv_addr_len);
    if (res > 0 && timeout->tv_sec && timeout->tv_usec)
    {
        netbios_ns_discover_rec(ns, timeout, recv_buffer);

        BDSM_dbg("Discover: received a reply from %s\n",
                 inet_ntoa(recv_addr.sin_addr));
        netbios_ns_inverse_internal(ns, recv_addr.sin_addr.s_addr);
    }
}

int           netbios_ns_discover(netbios_ns *ns)
{
    struct timeval      timeout;
    char                recv_buffer[256];

    assert(ns != NULL);

    //
    // First step, we broadcast a packet to receive a message from every
    // NETBIOS nodes on the local network
    //
    if (netbios_ns_send_name_query(ns, 0, NAME_QUERY_TYPE_NB,
                                   name_query_broadcast, 0) == -1)
        return (0);

    //
    // Second step, we list every IP that answered to our broadcast.
    //
    timeout.tv_sec = 2;
    timeout.tv_usec = 420;
    netbios_ns_discover_rec(ns, &timeout, (void *)recv_buffer);

    return (1);
}

void          netbios_ns_abort(netbios_ns *ns)
{
    uint8_t buf = '\0';
    write(ns->abort_pipe[1], &buf, sizeof(uint8_t));
}

// Perform inverse name resolution. Grap an IP and return the first <20> field
// returned by the host
static netbios_ns_entry *netbios_ns_inverse_internal(netbios_ns *ns, uint32_t ip)
{
    netbios_ns_entry  *cached;
    struct timeval      timeout;
    char                recv_buffer[512]; // Hu ?
    ssize_t             recv;

    if ((cached = netbios_ns_entry_find(ns, NULL, ip)) != NULL)
        return (cached);

    if (netbios_ns_send_name_query(ns, ip, NAME_QUERY_TYPE_NBSTAT,
                                   name_query_broadcast, 0) == -1)
        goto error;

    // Now wait for a reply and pray
    timeout.tv_sec = 1;
    timeout.tv_usec = 500;
    recv = netbios_ns_recv(ns->socket, ns->abort_pipe[0],
                           (void *)recv_buffer, 512, &timeout, 0, 0);

    if (recv <= 0)
        goto error;
    else
        BDSM_dbg("netbios_ns_inverse, received a reply for '%s' !\n",
                 inet_ntoa(*(struct in_addr *)&ip));


    // Now we've got something, let's find the <20>/<0> name
    netbios_query_packet  *p = (netbios_query_packet *)recv_buffer;
    uint8_t                 name_count;
    uint8_t                 name_idx;
    const char              *names;
    const char              *group = NULL;
    netbios_ns_entry        *entry = NULL;

    BDSM_dbg("Queried name length: %u\n", p->payload[0]);
    name_count = p->payload[p->payload[0] + 12];
    BDSM_dbg("Number of names: %hhu\n", name_count);
    names = p->payload + p->payload[0] + 13;

    // first search for a group in the name list
    for (name_idx = 0; name_idx < name_count; name_idx++)
    {
        const char *current_name = names + name_idx * 18;
        uint16_t current_flags = (current_name[16] << 8) | current_name[17];
        if (current_flags & NETBIOS_NAME_FLAG_GROUP) {
            group = current_name;
            BDSM_dbg("Found group: %s\n", group);
            break;
        }
    }
    // then search for file servers
    for (name_idx = 0; name_idx < name_count; name_idx++)
    {
        const char *current_name = names + name_idx * 18;
        char current_type = current_name[15];
        uint16_t current_flags = (current_name[16] << 8) | current_name[17];

        if (current_flags & NETBIOS_NAME_FLAG_GROUP)
            continue;
        BDSM_dbg("Found name : %s (type == 0x%x)\n", current_name, current_type);
        if (current_type == NETBIOS_FILESERVER)
            entry = netbios_ns_entry_add(ns, current_name, group, current_type, ip);
    }

    return (entry);

error:
    BDSM_perror("netbios_ns_inverse: ");
    return (NULL);
}

const char *netbios_ns_inverse(netbios_ns *ns, uint32_t ip)
{
    assert(ns != NULL && ip != 0);
    netbios_ns_entry *entry = netbios_ns_inverse_internal(ns, ip);
    return entry ? entry->name : NULL;
}
