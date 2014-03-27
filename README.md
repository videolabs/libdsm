== liBDSM

A small project of SMB protocol client implementation in C. The goal is to
have a less restrictive license that Samba. (Currenyly MIT, might be WTFL)
Final final goal is to have an iOS VLC integration.

=== How to build

At this stage of the project, you're kind of a masochist... Hey ! you're just at
the right place for this. Simply type 'make', correct Makefile if necessary and
fix any error you have. (Hum... Enjoy!)

=== Goals

Here's a list of features i intend to support in this project :
* NETBIOS
  ** Basic bi-directionnal NETBIOS name resolution
  ** Hacky LAN SMB servers discovery (Listing all the smb servers on the LAN, no WINS, etc.)
  ** Basic NETBIOS Session transport layer
* SMB
  ** Support only required parts of 'NT LM 0.12' (aka CIFS) dialect.
  ** User based authentication
  ** List Shares
  ** Browse folders
  ** Read file
  ** No write, lock, RPCs, etc.
