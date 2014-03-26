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
#include "bdsm/smb_defs.h"

int main(int ac, char **av)
{
  struct sockaddr_in  addr;
  bdsm_context_t      *ctx;

  //printf("sizeof(smb_header_t) = %lu", sizeof(smb_header_t));

  ctx = bdsm_context_new();
  assert(ctx);
  addr.sin_addr.s_addr = netbios_ns_resolve(ctx->ns, av[1], NETBIOS_FILESERVER);
  printf("%s's IP address is : %s\n", av[1], inet_ntoa(addr.sin_addr));

  netbios_ns_discover(ctx->ns);

  exit(0);

  netbios_session_t *session;
  session = netbios_session_new(addr.sin_addr.s_addr);
  if (netbios_session_connect(session, "Cerbere"))
    printf("A NetBIOS session with %s has been established\n", av[1]);
  else
    printf("Unable to establish a NetBIOS session with %s\n", av[1]);
  netbios_session_destroy(session);
  bdsm_context_destroy(ctx);
  // struct sockaddr_in  addr;
  // int                 sock;
  // int                 sock_opt = 1;

  // netbios_name_t      nb_name;
  // char                test[64];
  // int                 res;

  // nb_name = netbios_name_encode("Cerbere", 0, NETBIOS_WORKSTATION);
  // res = netbios_name_decode(nb_name, test, 0);

  // printf("Encoded: %s\n", (char *)nb_name + 1);
  // printf("Decoded: %s\n", (char *)test);

  // if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  // {
  //   printf("Unable to create socket: %s\n", strerror(errno));
  //   exit(42);
  // }

  // if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&sock_opt, sizeof(sock_opt)) < 0)
  // {
  //   printf("Unable to set broadcast opt on socket: %s\n", strerror(errno));
  //   close(sock);
  //   exit(42);
  // }

  // sock_opt = 0;
  // if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (void *)&sock_opt, sizeof(sock_opt)) < 0)
  // {
  //   printf("Unable to set broadcast opt on socket: %s\n", strerror(errno));
  //   close(sock);
  //   exit(42);
  // }

  // addr.sin_family = AF_INET;
  // //addr.sin_port = htons(NBT_UDP_PORT);
  // addr.sin_port = htons(0);
  // addr.sin_addr.s_addr = 0;

  // if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  // {
  //   printf("Unable to bind socket: %s\n", strerror(errno));
  //   close(sock);
  //   exit(42);
  // }

  // // Let's build a packet;
  // nbt_ns_packet_t     *packet = malloc(512);
  // unsigned int        payload_size = 34 + 4;
  // nbt_encoded_name_t  encoded_name;

  // nbt_encode_name(av[1], strlen(av[1]), encoded_name);
  // printf("Requested encoded_name = %.32s\n", encoded_name);

  // memset((void *)packet, 0, 512);
  // packet->trn_id      = rand();
  // packet->flags       = htons(0x0110);
  // packet->qd_count    = htons(1);
  // packet->payload[0]  = 0x20;
  // memcpy(packet->payload + 1, encoded_name, 33);
  // packet->payload[34] = 0x00;
  // packet->payload[35] = 0x20;
  // packet->payload[36] = 0x00;
  // packet->payload[37] = 0x01;



  // struct sockaddr_in  bcast_addr;
  // ssize_t             sent;

  // inet_aton("255.255.255.255", &(bcast_addr.sin_addr));
  // bcast_addr.sin_family = AF_INET;
  // bcast_addr.sin_port   = htons(137);

  // sent = sendto(sock, (void *)packet, sizeof(nbt_ns_packet_t) + payload_size, 0,
  //               (struct sockaddr *)&bcast_addr, sizeof(struct sockaddr_in));

  // if (sent < 0)
  // {
  //   perror("sendto");
  //   close(sock);
  //   exit(42);
  // }
  // else
  // {
  //   printf("Packet emitted ! (%lu bytes)\n", sizeof(nbt_ns_packet_t) + payload_size);
  // }

  // struct sockaddr_in  recv_addr;
  // size_t              recv_addr_length;
  // ssize_t             recv;
  // char                buffer[512];

  // recv = recvfrom(sock, (void *)buffer, 512, 0, 0, 0);

  // if (recv < 0)
  // {
  //   perror("recvfrom");
  //   close(sock);
  //   exit(42);
  // }

  // for(size_t i = 0; i < recv; i++)
  // {
  //   printf("%X", buffer[i]);
  // }
  // printf("\n");

  // return (0);
}
