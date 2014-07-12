/* Netbios Discover */

#include <stdio.h>

#include <bsdm/bdsm.h>

int main()
{
  netbios_ns        *ns;
  netbios_ns_entry  *entry;

  ns = netbios_ns_new();

  if (!netbios_ns_discover(ns))
  {
    fprintf(stderr, "Error while discovering local network\n");
    exit(42);
  }

  for (int i = 0; i < netbios_ns_entry_count(ns); i++)
  {
    struct in_addr addr;

    entry       = netbios_ns_entry_at(ns, i);
    addr.s_addr = netbios_ns_entry_ip(entry);

    printf("Ip: %s, name: %s<%x> \n",
      inet_ntoa(addr),
      netbios_ns_entry_name(entry),
      netbios_ns_entry_type(entry));
  }

  return (0);
}
