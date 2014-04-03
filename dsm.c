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
#include <assert.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define NBT_UDP_PORT        138
#define NBT_TCP_PORT        139

#include "bdsm.h"

#include <openssl/md4.h>
#include <openssl/md5.h>

int main(int ac, char **av)
{
  struct sockaddr_in  addr;
  bdsm_context_t      *ctx;

  ctx = bdsm_context_new();
  assert(ctx);
  addr.sin_addr.s_addr = netbios_ns_resolve(ctx->ns, av[1], NETBIOS_FILESERVER);
  printf("%s's IP address is : %s\n", av[1], inet_ntoa(addr.sin_addr));

  //netbios_ns_discover(ctx->ns);
  //exit(0);

  // netbios_session_t *session;
  // session = netbios_session_new(addr.sin_addr.s_addr);
  // if (netbios_session_connect(session, "Cerbere"))
  //   printf("A NetBIOS session with %s has been established\n", av[1]);
  // else
  // {
  //   printf("Unable to establish a NetBIOS session with %s\n", av[1]);
  //   exit(21);
  // }

  // netbios_session_destroy(session);

  smb_session_t *session;
  session = smb_session_new();
  if (smb_session_connect(session, av[1], addr.sin_addr.s_addr))
    printf("Successfully connected to %s\n", av[1]);
  else
  {
    printf("Unable to connect to %s\n", av[1]);
    exit(42);
  }
  if (smb_negotiate(session))
  {
    fprintf(stderr, "Dialect/Security Mode negotation success.\n");
    fprintf(stderr, "Session key is 0x%x\n", session->srv.session_key);
    fprintf(stderr, "Challenge key is 0x%lx\n", session->srv.challenge);
  }
  else
  {
    printf("Unable to negotiate SMB Dialect\n");
    exit(42);
  }


  if (smb_authenticate(session, av[1], av[2], av[3]))
  {
    if (session->guest)
      printf("Login FAILED but we were logged in as GUEST \n");
    else
      printf("Successfully logged in as %s\\%s\n", av[1], av[2]);
  }
  else
  {
    printf("Authentication FAILURE.\n");
    exit(42);
  }

  smb_tid ipc  = smb_tree_connect(session, "\\\\CERBERE\\IPC$");

  if (ipc == 0)
  {
    fprintf(stderr, "Unable to connect to IPC$ share\n");
    exit(42);
  }
  fprintf(stderr, "Connected to IPC$ share\n");

  smb_tid test = smb_tree_connect(session, "\\\\CERBERE\\TEST");
  if (test)
    fprintf(stderr, "Connected to Test share\n");
  else
  {
    fprintf(stderr, "Unable to connect to Test share\n");
    exit(42);
  }

  smb_fd fd = smb_fopen(session, test, "\\test.txt", SMB_MOD_RO);
  if (fd)
    fprintf(stderr, "Successfully opened \\test.txt: fd = 0x%.8x\n", fd);
  else
  {
    fprintf(stderr, "Unable to open \\test.txt\n");
    exit(42);
  }

  smb_fclose(session, fd);

  smb_session_destroy(session);
  bdsm_context_destroy(ctx);

  return (0);
}
