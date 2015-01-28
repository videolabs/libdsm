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
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include <bdsm/netbios_ns.h>

#include "bdsm_debug.h"
#include "netbios_query.h"
#include "netbios_utils.h"

enum name_query_type {
    NAME_QUERY_TYPE_INVALID,
    NAME_QUERY_TYPE_NB,
    NAME_QUERY_TYPE_NBSTAT
};
static char name_query_broadcast[] = NETBIOS_WILDCARD;

struct netbios_ns_entry
{
    netbios_ns_entry              *next;
    struct in_addr                address;
    char                          name[NETBIOS_NAME_LENGTH + 1];
    char                          group[NETBIOS_NAME_LENGTH + 1];
    char                          type;
    time_t                        last_time_seen;
};

#define RECV_BUFFER_SIZE 1500 // Max MTU frame size for ethernet

struct netbios_ns
{
    int                 socket;
    struct sockaddr_in  addr;
    uint16_t            last_trn_id;  // Last transaction id used;
    netbios_ns_entry    *entries;     // NS entries cache, mainly used by discover()
    uint8_t             buffer[RECV_BUFFER_SIZE];
    int                 abort_pipe[2];
};

typedef struct netbios_ns_name_query netbios_ns_name_query;
struct netbios_ns_name_query
{
    enum name_query_type type;
    union {
        struct {
            uint32_t ip;
        } nb;
        struct {
            const char *name;
            const char *group;
            char type;
        } nbstat;
    }u;
};

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

static uint16_t query_type_nb = 0x2000;
static uint16_t query_type_nbstat = 0x2100;
static uint16_t query_class_in = 0x0100;

static int netbios_ns_send_name_query(netbios_ns *ns,
                                      uint32_t ip,
                                      enum name_query_type type,
                                      const char *name,
                                      uint16_t query_flag)
{
    struct sockaddr_in  addr;
    ssize_t             sent;
    uint16_t            trn_id;
    uint16_t            query_type;
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
    netbios_query_append(q, (const char *)&query_type, 2);
    netbios_query_append(q, (const char *)&query_class_in, 2);
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

static int netbios_ns_handle_query(netbios_ns *ns, size_t size,
                                   bool check_trn_id, uint32_t recv_ip,
                                   netbios_ns_name_query *out_name_query)
{
    netbios_query_packet *q;
    uint8_t name_size;
    uint16_t *p_type, type;
    uint16_t *p_data_length, data_length;
    char     *p_data;

    // check for packet size
    if (size < sizeof(netbios_query_packet))
    {
        BDSM_dbg("netbios_ns_handle_query, invalid size !\n");
        return -1;
    }

    q = (netbios_query_packet *)ns->buffer;
    if (check_trn_id)
    {
        // check if trn_id corresponds
        if (ntohs(q->trn_id) != ns->last_trn_id) {
            BDSM_dbg("netbios_ns_handle_query, invalid trn_id: %d vs %d\n",
                     ntohs(q->trn_id), ns->last_trn_id);
            return -1;
        }
    }

    if (!out_name_query)
        return 0;

    // get Name size, should be 0x20
    if (size < sizeof(netbios_query_packet) + 1)
        return -1;
    name_size = q->payload[0];
    if (name_size != 0x20)
        return -1;

    // get type and data_length
    if (size < sizeof(netbios_query_packet) + name_size + 11)
        return -1;
    p_type = (uint16_t *) (q->payload + name_size + 2);
    type = *p_type;
    p_data_length = (uint16_t *) (q->payload + name_size + 10);
    data_length = ntohs(*p_data_length);

    if (size < sizeof(netbios_query_packet) + name_size + 12 + data_length)
        return -1;
    p_data = q->payload + name_size + 12;

    if (type == query_type_nb) {
        uint32_t ip;

        // get ip addr
        if (data_length < 6)
            ip = recv_ip;
        else
        {
            uint32_t *p_ip = (uint32_t *) (p_data + 2);
            ip = *p_ip;
        }

        out_name_query->type = NAME_QUERY_TYPE_NB;
        out_name_query->u.nb.ip = ip;
    } else if (type == query_type_nbstat) {
        uint8_t name_count;
        const char *names = p_data +  1;
        const char *group = NULL, *name = NULL;;

        // get the number of names
        if (data_length < 1)
            return -1;
        name_count = *(p_data);
        names = p_data + 1;

        if (data_length < name_count * 18)
            return -1;

        // first search for a group in the name list
        for (uint8_t name_idx = 0; name_idx < name_count; name_idx++)
        {
            const char *current_name = names + name_idx * 18;
            uint16_t current_flags = (current_name[16] << 8) | current_name[17];
            if (current_flags & NETBIOS_NAME_FLAG_GROUP) {
                group = current_name;
                break;
            }
        }
        // then search for file servers
        for (uint8_t name_idx = 0; name_idx < name_count; name_idx++)
        {
            const char *current_name = names + name_idx * 18;
            char current_type = current_name[15];
            uint16_t current_flags = (current_name[16] << 8) | current_name[17];

            if (current_flags & NETBIOS_NAME_FLAG_GROUP)
                continue;
            if (current_type == NETBIOS_FILESERVER)
            {
                name = current_name;
                BDSM_dbg("netbios_ns_handle_query, Found name: '%.*s' in group: '%.*s'\n",
                         NETBIOS_NAME_LENGTH, name, NETBIOS_NAME_LENGTH, group);
                break;
            }
        }

        if (name)
        {
            out_name_query->type = NAME_QUERY_TYPE_NBSTAT;
            out_name_query->u.nbstat.name = name;
            out_name_query->u.nbstat.group = group;
            out_name_query->u.nbstat.type = NETBIOS_FILESERVER;
        }
    }

    return 0;
}

static ssize_t netbios_ns_recv(netbios_ns *ns,
                               struct timeval *timeout,
                               struct sockaddr_in *out_addr,
                               bool check_trn_id,
                               uint32_t wait_ip,
                               netbios_ns_name_query *out_name_query)
{
    int sock, abort_fd;

    assert(ns != NULL);

    sock = ns->socket;
    abort_fd =  ns->abort_pipe[0];

    if (out_name_query)
        out_name_query->type = NAME_QUERY_TYPE_INVALID;

    while (true)
    {
        fd_set read_fds, error_fds;
        int res, nfds;

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
            return -1;

        else if (FD_ISSET(sock, &read_fds))
        {
            struct sockaddr_in addr;
            socklen_t addr_len = sizeof(struct sockaddr_in);
            ssize_t size;

            size = recvfrom(sock, ns->buffer, RECV_BUFFER_SIZE, 0,
                           (struct sockaddr *)&addr, &addr_len);
            if (size < 0)
                return -1;
            if (wait_ip != 0 && addr_len >= sizeof(struct sockaddr_in))
            {
                // wait for a reply from a specific ip
                if (wait_ip != addr.sin_addr.s_addr)
                {
                    BDSM_dbg("netbios_ns_recv, invalid ip");
                    continue;
                }
            }

            if (netbios_ns_handle_query(ns, (size_t)size, check_trn_id,
                                        addr.sin_addr.s_addr,
                                        out_name_query) == -1)
            {
                BDSM_dbg("netbios_ns_recv, invalid query\n");
                continue;
            }

            if (out_addr)
                *out_addr = addr;
            return size;
        }
        else
            return (0);
    }

error:
    BDSM_perror("netbios_ns_recv: ");
    return (-1);
}

static void netbios_ns_copy_name(char *dest, const char *src)
{
    memcpy(dest, src, NETBIOS_NAME_LENGTH);
    dest[NETBIOS_NAME_LENGTH] = 0;

    for (int i = 1; i < NETBIOS_NAME_LENGTH; i++ )
      if (dest[NETBIOS_NAME_LENGTH - i] == ' ')
        dest[NETBIOS_NAME_LENGTH - i] = 0;
      else
        break;
}

static netbios_ns_entry *netbios_ns_entry_add(netbios_ns *ns, const char *name,
                                              const char *group,
                                              char type, uint32_t ip)
{
    netbios_ns_entry  *entry;

    entry = calloc(1, sizeof(netbios_ns_entry));
    if (!entry)
        return NULL;

    if (name != NULL)
        netbios_ns_copy_name(entry->name, name);
    if (group != NULL)
        netbios_ns_copy_name(entry->group, group);

    entry->type           = type;
    entry->address.s_addr = ip;
    entry->next           = ns->entries;

    ns->entries = entry;

    return (ns->entries);
}

// Find an entry in the list. Search by name if name is not NULL,
// or by ip otherwise
static netbios_ns_entry *netbios_ns_entry_find(netbios_ns *ns, const char *by_name,
                                               uint32_t ip)
{
    netbios_ns_entry  *found = NULL;
    netbios_ns_entry  *iter;

    assert(ns != NULL);

    iter = ns->entries;
    while (iter != NULL && found == NULL)
    {
        if (by_name != NULL)
        {
            if (!strncmp(by_name, iter->name, NETBIOS_NAME_LENGTH))
                found = iter;
        }
        else if (iter->address.s_addr == ip)
            found = iter;

        iter = iter->next;
    }

    return (found);
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

int      netbios_ns_resolve(netbios_ns *ns, const char *name, char type, uint32_t *addr)
{
    netbios_ns_entry    *cached;
    struct timeval      timeout;
    char                *encoded_name;
    ssize_t             recv;
    netbios_ns_name_query name_query;

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
    recv = netbios_ns_recv(ns, &timeout, NULL, true, 0, &name_query);

    if (recv < 0)
        BDSM_perror("netbios_ns_resolve:");
    else
    {
        if (name_query.type == NAME_QUERY_TYPE_NB)
        {
            *addr = name_query.u.nb.ip;
            BDSM_dbg("netbios_ns_resolve, received a reply for '%s', ip: 0x%X!\n", name, *addr);
            return (1);
        } else
            BDSM_dbg("netbios_ns_resolve, wrong query type received\n");
    }

    return (0);
}

// We have a small recursive function for discovery, to stack received reply
// when descending, and performing reverse lookup when ascending
static void netbios_ns_discover_rec(netbios_ns *ns, struct timeval *timeout )
{
    struct sockaddr_in  recv_addr;
    int                 res;

    res = netbios_ns_recv(ns, timeout, &recv_addr, true, 0, NULL);
    if (res > 0 && timeout->tv_sec && timeout->tv_usec)
    {
        netbios_ns_discover_rec(ns, timeout);

        BDSM_dbg("Discover: received a reply from %s\n",
                 inet_ntoa(recv_addr.sin_addr));
        netbios_ns_inverse_internal(ns, recv_addr.sin_addr.s_addr);
    }
}

int           netbios_ns_discover(netbios_ns *ns)
{
    struct timeval      timeout;

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
    netbios_ns_discover_rec(ns, &timeout);

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
    ssize_t             recv;
    netbios_ns_name_query name_query;

    if ((cached = netbios_ns_entry_find(ns, NULL, ip)) != NULL)
        return (cached);

    if (netbios_ns_send_name_query(ns, ip, NAME_QUERY_TYPE_NBSTAT,
                                   name_query_broadcast, 0) == -1)
        goto error;

    // Now wait for a reply and pray
    timeout.tv_sec = 1;
    timeout.tv_usec = 500;
    recv = netbios_ns_recv(ns, &timeout, NULL, true, ip, &name_query);

    if (recv <= 0)
        goto error;
    
    if (name_query.type != NAME_QUERY_TYPE_NBSTAT)
    {
        BDSM_dbg("netbios_ns_inverse, wrong query type received\n");
        goto error;
    }
    else
        BDSM_dbg("netbios_ns_inverse, received a reply for '%s' !\n",
                 inet_ntoa(*(struct in_addr *)&ip));

    return netbios_ns_entry_add(ns, name_query.u.nbstat.name,
                                name_query.u.nbstat.group,
                                name_query.u.nbstat.type, ip);
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

const char *netbios_ns_entry_name(netbios_ns_entry *entry)
{
    return entry ? entry->name : NULL;
}

const char *netbios_ns_entry_group(netbios_ns_entry *entry)
{
    return entry ? entry->group : NULL;
}

uint32_t netbios_ns_entry_ip(netbios_ns_entry *entry)
{
    return entry ? entry->address.s_addr : 0;
}

char netbios_ns_entry_type(netbios_ns_entry *entry)
{
    return entry ? entry->type : -1;
}

void                netbios_ns_clear(netbios_ns *ns)
{
    netbios_ns_entry  *next;

    assert(ns != NULL);

    while (ns->entries != NULL)
    {
        next = ns->entries->next;
        free(ns->entries);
        ns->entries = next;
    }
}

int             netbios_ns_entry_count(netbios_ns *ns)
{
    netbios_ns_entry  *iter;
    int                 res;

    assert(ns != NULL);

    iter  = ns->entries;
    res   = 0;
    while (iter != NULL)
    {
        res++;
        iter = iter->next;
    }

    return (res);
}

netbios_ns_entry  *netbios_ns_entry_at(netbios_ns *ns, int pos)
{
    netbios_ns_entry  *iter = NULL;
    int                 i = 0;

    assert(ns != NULL);

    iter = ns->entries;
    while (i < pos && iter != NULL)
    {
        i++;
        iter = iter->next;
    }

    return (iter);
}
