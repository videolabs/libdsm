== liBDSM

A small project of SMB protocol client implementation in C, with a lot less
features than Samba but also a more permissive license (WTFL). Final final goal
is to have an iOS VLC integration.

=== How to build

At this stage of the project, you're kind of a masochist... Hey ! you're just at
the right place for this. Simply type 'make', correct Makefile if necessary and
fix any error you have. (Hum... Enjoy!)

=== Goals

Here's a list of features i intend to support in this project :
* NETBIOS
  ** Basic bi-directionnal NETBIOS name resolution [Done]
  ** Hacky LAN SMB servers discovery (Listing all the smb servers on the LAN, no WINS, etc.) [Done]
  ** Basic NETBIOS Session transport layer [Done]
* SMB
  ** Support only required parts of 'NT LM 0.12' (aka CIFS?) dialect.
  ** User based authentication [Done]
  ** List Shares [Partial/Ugly]
  ** Browse folders
  ** Read file [Done, can be improved :)]
  ** No write, lock, RPCs, etc.
