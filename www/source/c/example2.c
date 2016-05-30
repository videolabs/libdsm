/* Netbios Discover */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <bdsm/bdsm.h>

static void print_entry(const char *what, void *p_opaque,
                        netbios_ns_entry *entry)
{
  struct in_addr addr;
  addr.s_addr = netbios_ns_entry_ip(entry);

  printf("%s(%p): Ip: %s, name: %s/%s<%x>\n",
    what,
    p_opaque,
    inet_ntoa(addr),
    netbios_ns_entry_group(entry),
    netbios_ns_entry_name(entry),
    netbios_ns_entry_type(entry));
}

static void on_entry_added(void *p_opaque,
                           netbios_ns_entry *entry)
{
  print_entry("added", p_opaque, entry);
}

static void on_entry_removed(void *p_opaque,
                             netbios_ns_entry *entry)
{
  print_entry("removed", p_opaque, entry);
}

int main()
{
  netbios_ns *ns;
  netbios_ns_discover_callbacks callbacks;

  ns = netbios_ns_new();

  callbacks.p_opaque = (void*)0x42;
  callbacks.pf_on_entry_added = on_entry_added;
  callbacks.pf_on_entry_removed = on_entry_removed;

  printf("Discovering...\nPress Enter to quit\n");
  if (!netbios_ns_discover_start(ns,
                                 4, // broadcast every 4 sec
                                 &callbacks))
  {
    fprintf(stderr, "Error while discovering local network\n");
    exit(42);
  }

  getchar();

  netbios_ns_discover_stop(ns);

  return (0);
}
