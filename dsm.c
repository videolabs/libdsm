// Test file for libevent

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
#include "bdsm/netbios_utils.h"
#include "bdsm/netbios_session.h"
#include "bdsm/smb_session.h"

int main(int ac, char **av)
{
  struct sockaddr_in  addr;
  bdsm_context_t      *ctx;
  const char          *dialects[]= SMB_DIALECTS;

  printf("sizeof(smb_header_t) = %lu", sizeof(smb_header_t));

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
  if (smb_session_negotiate_protocol(session))
    printf("cool\n");
  else
    printf("Unable to negotiate SMB Dialect\n");

  smb_session_destroy(session);
  bdsm_context_destroy(ctx);

  return (0);
}
