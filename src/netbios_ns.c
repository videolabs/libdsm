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
 *         Thomas Guillem <tguillem@videolabs.io>
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

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#ifdef HAVE_SYS_QUEUE_H
# include <sys/queue.h>
#endif
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#ifdef _WIN32
# include <winsock2.h>
# include <ws2tcpip.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

#ifndef _WIN32
# include <sys/types.h>
# ifdef HAVE_IFADDRS_H
#  include <ifaddrs.h>
# endif
# include <net/if.h>
#endif

#include <bdsm/netbios_ns.h>

#include "bdsm_debug.h"
#include "netbios_query.h"
#include "netbios_utils.h"

#include "compat.h"

enum name_query_type {
    NAME_QUERY_TYPE_INVALID,
    NAME_QUERY_TYPE_NB,
    NAME_QUERY_TYPE_NBSTAT
};
static char name_query_broadcast[] = NETBIOS_WILDCARD;

enum ns_entry_flag {
    NS_ENTRY_FLAG_INVALID = 0x00,
    NS_ENTRY_FLAG_VALID_IP = 0x01,
    NS_ENTRY_FLAG_VALID_NAME = 0x02,
};

struct netbios_ns_entry
{
    TAILQ_ENTRY(netbios_ns_entry) next;
    struct in_addr                address;
    char                          name[NETBIOS_NAME_LENGTH + 1];
    char                          group[NETBIOS_NAME_LENGTH + 1];
    char                          type;
    int                           flag;
    time_t                        last_time_seen;
};
typedef TAILQ_HEAD(, netbios_ns_entry) NS_ENTRY_QUEUE;

#define RECV_BUFFER_SIZE 1500 // Max MTU frame size for ethernet

struct netbios_ns
{
    int                 socket;
    struct sockaddr_in  addr;
    uint16_t            last_trn_id;  // Last transaction id used;
    NS_ENTRY_QUEUE      entry_queue;
    uint8_t             buffer[RECV_BUFFER_SIZE];
#ifdef HAVE_PIPE
    int                 abort_pipe[2];
#else
    pthread_mutex_t     abort_lock;
    bool                aborted;
#endif
    unsigned int        discover_broadcast_timeout;
    pthread_t           discover_thread;
    bool                discover_started;
    netbios_ns_discover_callbacks discover_callbacks;
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
    ns->addr.sin_addr.s_addr  = INADDR_ANY;
    if (bind(ns->socket, (struct sockaddr *)&ns->addr, sizeof(ns->addr)) < 0)
        goto error;

    return 1;

error:
    BDSM_perror("netbios_ns_new, open_socket: ");
    return 0;
}

#ifdef HAVE_PIPE

static int    ns_open_abort_pipe(netbios_ns *ns)
{
    int flags;

    if (pipe(ns->abort_pipe) == -1)
        return -1;

#ifndef _WIN32
    if ((flags = fcntl(ns->abort_pipe[0], F_GETFL, 0)) == -1)
        return -1;

    if (fcntl(ns->abort_pipe[0], F_SETFL, flags | O_NONBLOCK) == -1)
        return -1;
#endif

    return 0;
}

static void   ns_close_abort_pipe(netbios_ns *ns)
{
    if (ns->abort_pipe[0] != -1 && ns->abort_pipe[1] != -1)
    {
        closesocket(ns->abort_pipe[0]);
        closesocket(ns->abort_pipe[1]);
        ns->abort_pipe[0] = ns->abort_pipe[1] = -1;
    }
}

static bool   netbios_ns_is_aborted(netbios_ns *ns)
{
    fd_set        read_fds;
    int           res;
    struct timeval timeout = {0, 0};

    FD_ZERO(&read_fds);
    FD_SET(ns->abort_pipe[0], &read_fds);

    res = select(ns->abort_pipe[0] + 1, &read_fds, NULL, NULL, &timeout);

    return (res < 0 || FD_ISSET(ns->abort_pipe[0], &read_fds));
}

static void netbios_ns_abort(netbios_ns *ns)
{
    uint8_t buf = '\0';
    write(ns->abort_pipe[1], &buf, sizeof(uint8_t));
}

#else

static int    ns_open_abort_pipe(netbios_ns *ns)
{
    return pthread_mutex_init(&ns->abort_lock, NULL);
}

static void   ns_close_abort_pipe(netbios_ns *ns)
{
    pthread_mutex_destroy(&ns->abort_lock);
}

static bool   netbios_ns_is_aborted(netbios_ns *ns)
{
    pthread_mutex_lock(&ns->abort_lock);
    bool res = ns->aborted;
    pthread_mutex_unlock(&ns->abort_lock);
    return res;
}

static void netbios_ns_abort(netbios_ns *ns)
{
    pthread_mutex_lock(&ns->abort_lock);
    ns->aborted = true;
    pthread_mutex_unlock(&ns->abort_lock);
}

#endif

static uint16_t query_type_nb = 0x2000;
static uint16_t query_type_nbstat = 0x2100;
static uint16_t query_class_in = 0x0100;

static ssize_t netbios_ns_send_packet(netbios_ns* ns, netbios_query* q, uint32_t ip)
{
    struct sockaddr_in  addr;

    addr.sin_addr.s_addr  = ip;
    addr.sin_family       = AF_INET;
    addr.sin_port         = htons(NETBIOS_PORT_NAME);

    BDSM_dbg("Sending netbios packet to %s\n", inet_ntoa(addr.sin_addr));
    return sendto(ns->socket, (void *)q->packet,
                  sizeof(netbios_query_packet) + q->cursor, 0,
                  (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
}

#ifndef _WIN32

#ifdef HAVE_GETIFADDRS

static void netbios_ns_broadcast_packet(netbios_ns* ns, netbios_query* q)
{
    struct ifaddrs *addrs;
    if (getifaddrs(&addrs) != 0)
        return;
    for (struct ifaddrs *a = addrs; a != NULL; a = a->ifa_next)
    {
        if ((a->ifa_flags & IFF_BROADCAST) == 0 || (a->ifa_flags & IFF_UP) == 0)
            continue;
        if (a->ifa_addr->sa_family != PF_INET)
            continue;
        struct sockaddr_in* sin = (struct sockaddr_in*)a->ifa_broadaddr;

        uint32_t ip = sin->sin_addr.s_addr;
        if (netbios_ns_send_packet(ns, q, ip) == -1)
            BDSM_perror("Failed to broadcast");
    }
    freeifaddrs(addrs);
}

#else

static void netbios_ns_broadcast_packet(netbios_ns* ns, netbios_query* q)
{
    netbios_ns_send_packet(ns, q, INADDR_BROADCAST);
}

#endif

#else

static void netbios_ns_broadcast_packet(netbios_ns* ns, netbios_query* q)
{
    INTERFACE_INFO infolist[16];
    DWORD dwBytesReturned = 0;
    if (WSAIoctl(ns->socket, SIO_GET_INTERFACE_LIST, NULL, 0, (void*)infolist, sizeof(infolist), &dwBytesReturned, NULL, NULL) != 0)
        return;
    unsigned int dwNumInterfaces = dwBytesReturned / sizeof(INTERFACE_INFO);

    for (unsigned int index = 0; index < dwNumInterfaces; index++)
    {
        if (infolist[index].iiAddress.Address.sa_family == AF_INET)
        {
            uint32_t broadcast = infolist[index].iiAddress.AddressIn.sin_addr.s_addr & infolist[index].iiNetmask.AddressIn.sin_addr.s_addr;
            broadcast |= ~ infolist[index].iiNetmask.AddressIn.sin_addr.S_un.S_addr;
            if (netbios_ns_send_packet(ns, q, broadcast) == -1)
                BDSM_perror("Failed to broadcast");
        }
    }
}

#endif

static int netbios_ns_send_name_query(netbios_ns *ns,
                                      uint32_t ip,
                                      enum name_query_type type,
                                      const char *name,
                                      uint16_t query_flag)
{
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

    // Increment transaction ID, not to reuse them
    q->packet->trn_id = htons(ns->last_trn_id + 1);

    if (ip != 0)
    {
        ssize_t sent = netbios_ns_send_packet(ns, q, ip);
        if (sent < 0)
        {
            BDSM_perror("netbios_ns_send_name_query: ");
            netbios_query_destroy(q);
            return -1;
        }
        else
            BDSM_dbg("netbios_ns_send_name_query, name query sent for '*'.\n");
    }
    else
    {
        netbios_ns_broadcast_packet(ns, q);
    }

    netbios_query_destroy(q);

    ns->last_trn_id++; // Remember the last transaction id.
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
        out_name_query->type = NAME_QUERY_TYPE_NB;
        out_name_query->u.nb.ip = recv_ip;
    } else if (type == query_type_nbstat) {
        uint8_t name_count;
        const char *names = NULL;
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
    int sock;

    assert(ns != NULL);

    sock = ns->socket;
#ifdef HAVE_PIPE
    int abort_fd =  ns->abort_pipe[0];
#else
    int abort_fd = -1;
#endif

    if (out_name_query)
        out_name_query->type = NAME_QUERY_TYPE_INVALID;

    while (true)
    {
        fd_set read_fds, error_fds;
        int res, nfds;

        FD_ZERO(&read_fds);
        FD_ZERO(&error_fds);
        FD_SET(sock, &read_fds);
#ifdef HAVE_PIPE
        FD_SET(abort_fd, &read_fds);
#endif
        FD_SET(sock, &error_fds);
        nfds = (sock > abort_fd ? sock : abort_fd) + 1;

        res = select(nfds, &read_fds, 0, &error_fds, timeout);

        if (res < 0)
            goto error;
        if (FD_ISSET(sock, &error_fds))
            goto error;

#ifdef HAVE_PIPE
        if (FD_ISSET(abort_fd, &read_fds))
            return -1;
#else
        if (netbios_ns_is_aborted(ns))
            return -1;
#endif

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
            return 0;
    }

error:
    BDSM_perror("netbios_ns_recv: ");
    return -1;
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

static void netbios_ns_entry_set_name(netbios_ns_entry *entry,
                                      const char *name, const char *group,
                                      char type)
{
    if (name != NULL)
        netbios_ns_copy_name(entry->name, name);
    if (group != NULL)
        netbios_ns_copy_name(entry->group, group);

    entry->type = type;
    entry->flag |= NS_ENTRY_FLAG_VALID_NAME;
}

static netbios_ns_entry *netbios_ns_entry_add(netbios_ns *ns, uint32_t ip)
{
    netbios_ns_entry  *entry;

    entry = calloc(1, sizeof(netbios_ns_entry));
    if (!entry)
        return NULL;

    entry->address.s_addr = ip;
    entry->flag |= NS_ENTRY_FLAG_VALID_IP;

    TAILQ_INSERT_HEAD(&ns->entry_queue, entry, next);

    return entry;
}

// Find an entry in the list. Search by name if name is not NULL,
// or by ip otherwise
static netbios_ns_entry *netbios_ns_entry_find(netbios_ns *ns, const char *by_name,
                                               uint32_t ip)
{
    netbios_ns_entry  *iter;

    assert(ns != NULL);

    TAILQ_FOREACH(iter, &ns->entry_queue, next)
    {
        if (by_name != NULL)
        {
            if (iter->flag & NS_ENTRY_FLAG_VALID_NAME
                && !strncmp(by_name, iter->name, NETBIOS_NAME_LENGTH))
                return iter;
        }
        else if (iter->flag & NS_ENTRY_FLAG_VALID_IP
                 && iter->address.s_addr == ip)
            return iter;
    }

    return NULL;
}

static void netbios_ns_entry_clear(netbios_ns *ns)
{
    netbios_ns_entry  *entry, *entry_next;

    assert(ns != NULL);

    for (entry = TAILQ_FIRST(&ns->entry_queue);
         entry != NULL; entry = entry_next)
    {
        entry_next = TAILQ_NEXT(entry, next);
        TAILQ_REMOVE(&ns->entry_queue, entry, next);
        free(entry);
    }
}

netbios_ns  *netbios_ns_new()
{
    netbios_ns  *ns;

    ns = calloc(1, sizeof(netbios_ns));
    if (!ns)
        return NULL;

#ifdef HAVE_PIPE
    // Don't initialize this in ns_open_abort_pipe, as it would lead to
    // fd 0 to be closed (twice) in case of ns_open_socket error
    ns->abort_pipe[0] = ns->abort_pipe[1] = -1;
#endif

    if (!ns_open_socket(ns) || ns_open_abort_pipe(ns) == -1)
    {
        netbios_ns_destroy(ns);
        return NULL;
    }

    TAILQ_INIT(&ns->entry_queue);
    ns->last_trn_id   = rand();

    return ns;
}

void          netbios_ns_destroy(netbios_ns *ns)
{
    if (!ns)
        return;

    netbios_ns_entry_clear(ns);

    if (ns->socket != -1)
        closesocket(ns->socket);

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

    assert(ns != NULL && !ns->discover_started);

    if ((cached = netbios_ns_entry_find(ns, name, 0)) != NULL)
    {
        *addr = cached->address.s_addr;
        return 0;
    }

    if ((encoded_name = netbios_name_encode(name, 0, type)) == NULL)
        return -1;

    if (netbios_ns_send_name_query(ns, 0, NAME_QUERY_TYPE_NB, encoded_name,
                                   NETBIOS_FLAG_RECURSIVE |
                                   NETBIOS_FLAG_BROADCAST) == -1)
    {
        free(encoded_name);
        return -1;

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
            return 0;
        } else
            BDSM_dbg("netbios_ns_resolve, wrong query type received\n");
    }

    return -1;
}

// Perform inverse name resolution. Grap an IP and return the first <20> field
// returned by the host
static netbios_ns_entry *netbios_ns_inverse_internal(netbios_ns *ns, uint32_t ip)
{
    netbios_ns_entry  *cached;
    struct timeval      timeout;
    ssize_t             recv;
    netbios_ns_name_query name_query;
    netbios_ns_entry *entry;

    if ((cached = netbios_ns_entry_find(ns, NULL, ip)) != NULL)
        return cached;

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

    entry = netbios_ns_entry_add(ns, ip);
    if (entry)
        netbios_ns_entry_set_name(entry, name_query.u.nbstat.name,
                                  name_query.u.nbstat.group,
                                  name_query.u.nbstat.type);
    return entry;
error:
    BDSM_perror("netbios_ns_inverse: ");
    return NULL;
}

const char *netbios_ns_inverse(netbios_ns *ns, uint32_t ip)
{
    assert(ns != NULL && ip != 0 && !ns->discover_started);
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

static void *netbios_ns_discover_thread(void *opaque)
{
    netbios_ns *ns = (netbios_ns *) opaque;
    while (true)
    {
        struct timespec tp;
        const int remove_timeout = 5 * ns->discover_broadcast_timeout;
        netbios_ns_entry  *entry, *entry_next;

        if (netbios_ns_is_aborted(ns))
            return NULL;

        // check if cached entries timeout, the timeout value is 5 times the
        // broadcast timeout.
        clock_gettime(CLOCK_REALTIME, &tp);
        for (entry = TAILQ_FIRST(&ns->entry_queue);
             entry != NULL; entry = entry_next)
        {
            entry_next = TAILQ_NEXT(entry, next);
            if (tp.tv_sec - entry->last_time_seen > remove_timeout)
            {
                if (entry->flag & NS_ENTRY_FLAG_VALID_NAME)
                {
                    BDSM_dbg("Discover: on_entry_removed: %s\n", entry->name);
                    ns->discover_callbacks.pf_on_entry_removed(
                            ns->discover_callbacks.p_opaque, entry);
                }
                TAILQ_REMOVE(&ns->entry_queue, entry, next);
                free(entry);
            }
        }

        // send broadbast
        if (netbios_ns_send_name_query(ns, 0, NAME_QUERY_TYPE_NB,
                                       name_query_broadcast, 0) == -1)
            return NULL;

        while (true)
        {
            struct timeval      timeout;
            struct sockaddr_in  recv_addr;
            int                 res;
            netbios_ns_name_query name_query;

            timeout.tv_sec = ns->discover_broadcast_timeout;
            timeout.tv_usec = 0;

            // receive NB or NBSTAT answers
            res = netbios_ns_recv(ns, ns->discover_broadcast_timeout == 0 ?
                                  NULL : &timeout,
                                  &recv_addr,
                                  false, 0, &name_query);
            // error or abort
            if (res == -1)
                return NULL;

            // timeout reached, broadcast again
            if (res == 0)
                break;

            clock_gettime(CLOCK_REALTIME, &tp);

            if (name_query.type == NAME_QUERY_TYPE_NB)
            {
                uint32_t ip = name_query.u.nb.ip;
                entry = netbios_ns_entry_find(ns, NULL, ip);

                if (!entry)
                {
                    entry = netbios_ns_entry_add(ns, ip);
                    if (!entry)
                        return NULL;
                }
                entry->last_time_seen = tp.tv_sec;

                // if entry is already valid, don't send NBSTAT query
                if (entry->flag & NS_ENTRY_FLAG_VALID_NAME)
                    continue;

                // send NBSTAT query
                if (netbios_ns_send_name_query(ns, ip, NAME_QUERY_TYPE_NBSTAT,
                                               name_query_broadcast, 0) == -1)
                    return NULL;

            }
            else if (name_query.type == NAME_QUERY_TYPE_NBSTAT)
            {
                bool send_callback;

                entry = netbios_ns_entry_find(ns, NULL,
                                              recv_addr.sin_addr.s_addr);

                // ignore NBSTAT answers that didn't answered to NB query first.
                if (!entry)
                    continue;

                entry->last_time_seen = tp.tv_sec;

                send_callback = !(entry->flag & NS_ENTRY_FLAG_VALID_NAME);

                netbios_ns_entry_set_name(entry, name_query.u.nbstat.name,
                                          name_query.u.nbstat.group,
                                          name_query.u.nbstat.type);
                if (send_callback)
                    ns->discover_callbacks.pf_on_entry_added(
                            ns->discover_callbacks.p_opaque, entry);
            }
        }
        if (ns->discover_broadcast_timeout == 0)
            break;
    }
    return NULL;
}

int netbios_ns_discover_start(netbios_ns *ns,
                              unsigned int broadcast_timeout,
                              netbios_ns_discover_callbacks *callbacks)
{
    if (ns->discover_started || !callbacks)
        return -1;

    ns->discover_callbacks = *callbacks;
    ns->discover_broadcast_timeout = broadcast_timeout;
    if (pthread_create(&ns->discover_thread, NULL,
                       netbios_ns_discover_thread, ns) != 0)
        return -1;
    ns->discover_started = true;

    return 0;
}

int netbios_ns_discover_stop(netbios_ns *ns)
{
    if (ns->discover_started)
    {
        netbios_ns_abort(ns);
        pthread_join(ns->discover_thread, NULL);
        ns->discover_started = false;

        return 0;
    }
    else
        return -1;
}
