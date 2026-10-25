/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* Regression checks for libdsm's actual NetBIOS code. No NAS files are used. */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <arpa/inet.h>
#undef NDEBUG
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <bdsm/netbios_ns.h>
#include <bdsm/netbios_defs.h>
#include "netbios_utils.h"
#include "netbios_session.h"

static int held[4096];
static int held_count;

static netbios_session *session_pair(int *peer)
{
    int pair[2];
    assert(socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == 0);
    netbios_session *session = netbios_session_new(128);
    assert(session != NULL);
    session->socket = pair[0];
    session->state = NETBIOS_SESSION_CONNECTED;
    *peer = pair[1];
    return session;
}

static void destroy_session(netbios_session *session)
{
    netbios_session_disconnect(session);
    netbios_session_destroy(session);
}

static void *receive_packet(void *opaque)
{
    netbios_session *session = opaque;
    assert(netbios_session_packet_recv(session, NULL) == -1);
    return NULL;
}

int main(int argc, char **argv)
{
    alarm(30);
    int minimum = argc > 1 ? atoi(argv[1]) : 1100;
    assert(minimum >= 0 && minimum < 3000);
    struct rlimit limit;
    assert(getrlimit(RLIMIT_NOFILE, &limit) == 0);
    if (limit.rlim_cur < (rlim_t)minimum + 32)
    {
        limit.rlim_cur = (rlim_t)minimum + 32;
        if (limit.rlim_cur > limit.rlim_max || setrlimit(RLIMIT_NOFILE, &limit))
        {
            puts("SKIP: descriptor limit too low");
            return 77;
        }
    }
    while (held_count == 0 || held[held_count - 1] < minimum)
    {
        assert(held_count < (int)(sizeof held / sizeof held[0]));
        held[held_count] = open("/dev/null", O_RDONLY);
        assert(held[held_count] >= 0);
        held_count++;
    }
    printf("Highest held descriptor: %d\n", held[held_count - 1]);
    fflush(stdout);

    struct netbios_abort_ctx abort_ctx;
    assert(netbios_abort_ctx_init(&abort_ctx) == 0);
    assert(!netbios_abort_ctx_is_aborted(&abort_ctx));
    netbios_abort_ctx_abort(&abort_ctx);
    assert(netbios_abort_ctx_is_aborted(&abort_ctx));
    netbios_abort_ctx_destroy(&abort_ctx);
    puts("PASS abort pipe inspection");

    int peer;
    netbios_session *session = session_pair(&peer);
    assert(netbios_wait(&session->abort_ctx, session->socket, true, NULL) == 1);
    struct timeval timeout = {0, 30000};
    assert(netbios_wait(&session->abort_ctx, session->socket, false, &timeout) == 0);
    assert(timeout.tv_sec == 0 && timeout.tv_usec == 0);
    assert(netbios_wait(&session->abort_ctx, session->socket, false, &timeout) == 0);
    errno = 0;
    assert(netbios_wait(&session->abort_ctx, -1, false, NULL) == -1 && errno == EBADF);
    puts("PASS writable socket, finite timeout, and invalid descriptor");

    netbios_session_packet_init(session);
    assert(netbios_session_packet_append(session, "abc", 3) == 1);
    assert(netbios_session_packet_send(session) == 7);
    unsigned char frame[7];
    assert(recv(peer, frame, sizeof frame, MSG_WAITALL) == sizeof frame);
    const unsigned char expected[] = {0, 0, 0, 3, 'a', 'b', 'c'};
    assert(memcmp(frame, expected, sizeof frame) == 0);
    assert(send(peer, expected, sizeof expected, 0) == sizeof expected);
    void *data;
    assert(netbios_session_packet_recv(session, &data) == 3);
    assert(memcmp(data, "abc", 3) == 0);
    close(peer);
    assert(netbios_session_packet_recv(session, NULL) == -1);
    destroy_session(session);
    puts("PASS session send, receive, and peer closure");

    session = session_pair(&peer);
    pthread_t receiver;
    assert(pthread_create(&receiver, NULL, receive_packet, session) == 0);
    usleep(20000);
    netbios_session_abort(session);
    assert(pthread_join(receiver, NULL) == 0);
    close(peer);
    destroy_session(session);
    puts("PASS cancellation wakes a blocked session receive");

    netbios_ns *ns = netbios_ns_new();
    assert(ns != NULL);
    /* Cancel before querying, so no packets are sent to the local network. */
    netbios_ns_abort(ns);
    assert(netbios_ns_inverse(ns, htonl(INADDR_LOOPBACK)) == NULL);
    netbios_ns_destroy(ns);
    puts("PASS cancelled name query");

    /* A closed descriptor must report an error instead of waiting forever. */
    session = session_pair(&peer);
    int closed = dup(peer);
    assert(closed >= 0);
    close(closed);
    errno = 0;
    assert(netbios_wait(&session->abort_ctx, closed, false, NULL) == -1);
    assert(errno == EBADF);
    /* An already cancelled context wins even when the socket is writable. */
    netbios_session_abort(session);
    assert(netbios_wait(&session->abort_ctx, session->socket, true, NULL) == -1);
    close(peer);
    destroy_session(session);
    puts("PASS closed descriptor and cancellation before wait");

    /* Find an unused loopback port: connecting must fail. */
    int server = socket(AF_INET, SOCK_STREAM, 0);
    assert(server >= 0);
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    assert(bind(server, (struct sockaddr *)&address, sizeof(address)) == 0);
    socklen_t size = sizeof(address);
    assert(getsockname(server, (struct sockaddr *)&address, &size) == 0);
    close(server);
    int client = socket(AF_INET, SOCK_STREAM, 0);
    assert(client >= 0);
    assert(fcntl(client, F_SETFL, O_NONBLOCK) == 0);
    int connected = connect(client, (struct sockaddr *)&address, size);
    assert(connected == -1);
    if (errno == EINPROGRESS)
    {
        assert(netbios_abort_ctx_init(&abort_ctx) == 0);
        timeout = (struct timeval){1, 0};
        assert(netbios_wait(&abort_ctx, client, true, &timeout) == 1);
        int error = 0;
        size = sizeof(error);
        assert(getsockopt(client, SOL_SOCKET, SO_ERROR, &error, &size) == 0);
        assert(error == ECONNREFUSED);
        netbios_abort_ctx_destroy(&abort_ctx);
    }
    else
        assert(errno == ECONNREFUSED);
    close(client);
    puts("PASS failed nonblocking connection");

    while (held_count > 0)
        close(held[--held_count]);
    puts("All NetBIOS descriptor checks passed");
    return 0;
}
