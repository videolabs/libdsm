// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "bdsm/netbios_session.h"
#include "bdsm/netbios_utils.h"

static int        open_socket_and_connect(netbios_session_t *s)
{
  if ((s->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    goto error;
  if (connect(s->socket, (struct sockaddr *)&s->remote_addr, sizeof(s->remote_addr)) <0)
    goto error;

  return (1);

  error:
    perror("netbios_session_new, open_socket: ");
    return (0);
}

netbios_session_t *netbios_session_new(uint32_t ip_addr)
{
  netbios_session_t *session;
  size_t            packet_size;

  session = (netbios_session_t *)malloc(sizeof(netbios_session_t));
  assert(session);
  memset((void *) session, 0, sizeof(netbios_session_t));

  session->packet_payload_size = NETBIOS_SESSION_PAYLOAD;
  packet_size = sizeof(netbios_session_packet_t) + session->packet_payload_size;
  session->packet = (netbios_session_packet_t *)malloc(packet_size);
  assert(session->packet);

  session->remote_addr.sin_family       = AF_INET;
  session->remote_addr.sin_port         = htons(NETBIOS_PORT_SESSION);
  session->remote_addr.sin_addr.s_addr  = ip_addr;
  if (!open_socket_and_connect(session))
  {
    netbios_session_destroy(session);
    return (0);
  }

  return(session);
}

void              netbios_session_destroy(netbios_session_t *s)
{
  if (!s)
    return;
  close(s->socket);
  if (s->packet)
    free(s->packet);
  free(s);
}

int               netbios_session_connect(netbios_session_t *s, char *name)
{
  netbios_session_packet_t  *received;
  ssize_t                   recv_size;
  ssize_t                   recv_payload_size;
  char                      *encoded_name;

  assert(s && s->packet && s->socket);

  // Send the Session Request message
  netbios_session_packet_init(s, NETBIOS_OP_SESSION_REQ);
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
  recv_size = netbios_session_packet_recv(s);
  if (recv_size < sizeof(netbios_session_packet_t))
    goto error;
  recv_payload_size = recv_size - sizeof(netbios_session_packet_t);

  received = (netbios_session_packet_t *)&s->recv_buffer;
  // Reply was negative, we are not connected :(
  if (received->opcode != NETBIOS_OP_SESSION_REQ_OK)
  {
    s->state = NETBIOS_SESSION_REFUSED;
    return (0);
  }

  // Reply was OK, a netbios sessions has been established
  s->state = NETBIOS_SESSION_CONNECTED;
  return(1);

  error:
    s->state = NETBIOS_SESSION_ERROR;
    return (0);
}

void              netbios_session_packet_init(netbios_session_t *s,
                                              uint8_t opcode)
{
  size_t          packet_size;

  assert(s);

  packet_size = s->packet_payload_size + sizeof(netbios_session_packet_t);
  memset((void *)s->packet, 0, packet_size);

  s->packet_cursor = 0;
  s->packet->opcode = opcode;
}

int               netbios_session_packet_append(netbios_session_t *s,
                                                char *data, size_t size)
{
  char  *start;

  assert(s && s->packet);

  if (s->packet_payload_size - s->packet_cursor < size)
    return (-1);

  start = ((char *)&s->packet->payload) + s->packet_cursor;
  memcpy(start, data, size);
  s->packet_cursor += size;

  return (0);
}

int               netbios_session_packet_send(netbios_session_t *s)
{
  ssize_t         to_send;
  ssize_t         sent;

  assert(s && s->packet && s->socket && s->state > 0);

  s->packet->length = htons(s->packet_cursor);
  to_send           = sizeof(netbios_session_packet_t) + s->packet_cursor;
  sent              = send(s->socket, (void *)s->packet, to_send, 0);

  if (sent != to_send)
  {
    perror("netbios_session_packet_send: Unable to send (full) packet");
    return (0);
  }

  return (sent);
}

ssize_t           netbios_session_packet_recv(netbios_session_t *s)
{
  assert(s && s->socket && s->state > 0);

  return (recv(s->socket, (void *)&(s->recv_buffer), NETBIOS_SESSION_BUFFER, 0));
}

