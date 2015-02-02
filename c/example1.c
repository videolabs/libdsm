/* SMB Connect, login and read file example */

#include <arpa/inet.h>
#include <stdio.h>

#include <bsdm/bdsm.h>

int main()
{
  struct in_addr  addr;
  smb_session   *session;
  smb_tid     tid;
  smb_fd      fd;

  session = smb_session_new();
  if (session == NULL)
    exit(1);

  inet_aton("127.0.0.1", &addr.sin_addr);

  if (smb_session_connect(session, "MYNAME", 
      addr.sin_addr.s_addr, SMB_TRANSPORT_TCP))
  {
    printf("Unable to connect to host\n");
    exit(2);
  }

  smb_session_set_creds(session, "MYNAME", "login", 
              "password");
  if (smb_session_login(session))
  {
    if (session->guest)
      printf("Logged in as GUEST \n");
    else
      printf("Successfully logged in\n");
  }
  else
  {
    printf("Auth failed\n");
    exit(3);
  }

  tid = smb_tree_connect(session, "MyShare");
  if (!tid)
  {
    printf("Unable to connect to share\n");
    exit(4);
  }

  fd = smb_fopen(session, tid, "\\My\\File");
  if (!fd)
  {
    printf("Unable to open file\n");
    exit(5);
  }

  char buffer[512];
  smb_fread(session, fd, buffer, 512);

  /* Use data */

  smb_fclose(session, fd);
  smb_tree_disconnect(session, tid);
  smb_session_destroy(session);

  return(0);
}
