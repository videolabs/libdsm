// This file is part of libdsm
// Author: Julien 'Lta' BALLET <contact@lta.io>
// Copyright VideoLabs 2014
// License: MIT License

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "bdsm/smb_message.h"

smb_message_t   *smb_message_new(uint8_t cmd, size_t payload_size)
{
  const char    magic[4] = SMB_MAGIC;
  smb_message_t *msg;

  msg = (smb_message_t *)malloc(sizeof(smb_message_t));
  assert(msg != NULL);
  memset((void *)msg, 0, sizeof(smb_message_t));

  msg->packet = (smb_packet_t *)malloc(sizeof(smb_message_t) + payload_size);
  assert(msg != NULL);
  memset((void *)msg->packet, 0, sizeof(smb_message_t) + payload_size);

  msg->payload_size = payload_size;
  msg->cursor = 0;

  for (unsigned i = 0; i < 4; i++)
    msg->packet->header.magic[i] = magic[i];
  msg->packet->header.command  = cmd;

  return (msg);
}

void            smb_message_destroy(smb_message_t *msg)
{
  if (msg != NULL)
  {
    if (msg->packet != NULL)
      free(msg->packet);
    free(msg);
  }
}

int             smb_message_append(smb_message_t *msg, const void *data,
                                   size_t data_size)
{
  assert(msg != NULL && data != NULL);

  if (data_size > msg->payload_size - msg->cursor)
    return (0);

  memcpy(msg->packet->payload + msg->cursor, data, data_size);
  msg->cursor += data_size;

  return (1);
}

int             smb_message_put8(smb_message_t *msg, uint8_t data)
{
  return(smb_message_append(msg, (void *)&data, 1));
}

int             smb_message_put16(smb_message_t *msg, uint16_t data)
{
    return(smb_message_append(msg, (void *)&data, 2));
}

int             smb_message_put32(smb_message_t *msg, uint32_t data)
{
    return(smb_message_append(msg, (void *)&data, 4));
}

int             smb_message_put64(smb_message_t *msg, uint32_t data)
{
    return(smb_message_append(msg, (void *)&data, 8));
}
