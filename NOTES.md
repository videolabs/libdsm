# SMB/NBT Notes.

I will here throw various notes regarding SMB protocol implementation and
compatibility issues, with the hope it could be usefull to and reduce of pain
of somebody else. There are no particular order.

Most (if not all) of these notes apply mostly to Win7 (the network i work on has
only Win7 boxes except my Linux one)

## Discovering network hosts.

Maybe this information is well known around the net, but it found it by randomly
experimenting with NBNS packets. There a special case of NetBIOS name which is
the wildcard `0x 2a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00` (before
Netbios encoding), that is a '*' followed by 15 zeros. I guess all the machines
on the network are supposed to reply when they see this broadcasted, but in
practice they don't. They don't reply, except if you remove the recursive and
broadcast flag from the NBNS packet, but still broadcast it. In that case,
they'll reply with an error. That's already something :) Now you've got their
IPs, you'll be able to issue a NBSTATE command to each one of them, so you'll
have thei're netbios name. Cool, you know everyone on the LAN.

PS: I've no idea (yet) of how Samba, older versions of Windows and/or NAS are
behaving regarding this.

## Windows 7 NTLM2

Although Win7 is supposed to support the old NTLM2 authentication, it seems
they actually doesn't handle it oustide of GSSAPI/SPNEGO, which i would like
to avoid, being unnecessarily complicated for my little goals.

I've been unable to build a packet that would satisfy it, it answers with a BAD
PARAMETER (or something in that area). smbclient exhibits the same behavior.
But, it you just omit the UnicodePassword field with the NTLM2 response and just
provide the simpler LM2 response, it works nicely. Pay attention that in that
case, you MUST uppercase the user name.
