#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <bdsm/bdsm.h>

int main()
{
  struct in_addr addr;
  smb_session    *session;
  smb_tid        tid;
  smb_fd         fd;
  int            result;

  session = smb_session_new();
  if (session == NULL)
    exit(1);

  inet_aton("127.0.0.1", &addr);

  if (smb_session_connect(session, "EXAMPLE-PC", 
      addr.s_addr, SMB_TRANSPORT_TCP))
  {
    printf("Unable to connect to host\n");
    exit(2);
  }

  smb_session_set_creds(session, "EXAMPLE-PC", "EXAMPLE-USER", 
			"EXAMPLE-PASSWORD");
  
  result = smb_session_login(session);
  if (result == 0)
  {
    if (smb_session_is_guest(session))
      printf("Logged in as GUEST \n");
    else
      printf("Successfully logged in\n");
  }
  else
  {
    printf("Auth failed\n");
    exit(3);
  }

  result = smb_tree_connect(session, "sharedfolder", &tid);
  if (!tid || result != 0)
  {
    printf("Unable to connect to share\n");
    exit(4);
  }
  
  result = smb_fopen(session, tid, "\\helloworld.txt",
                     SMB_MOD_READ, &fd);
  if (!fd || result != 0)
  {
    printf("Unable to open file\n");
    exit(5);
  }

  char buffer[12];
  smb_fread(session, fd, buffer, 12);
  printf("%s\n" ,buffer);

  smb_fclose(session, fd);
  smb_tree_disconnect(session, tid);
  smb_session_destroy(session);

  return(0);
}
