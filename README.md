# liBDSM

lib Defective SMb (__libDSM__) is a [SMB protocol](https://en.wikipedia.org/wiki/Server_Message_Block) client implementation in pure old __C__, with
a _lot_ less features than [Samba](https://www.samba.org/) but is simpler, and
has with a more permissive license (currently __LGPL__ + __proprietary__).

The initial goal of this project is to have a library that can access most SMB shares to
read files and that has with a license compatible with the iOS/Android/WinRT appstores in
order to integrate it into _VLC for iOS_ and _VLC for Android_.

This library is also licensable under a proprietary license, if LGPL is not good enough
for you.

The lib is provided as a static library or as a dynamically linked library.
A few useless (yet) utils are also provided, mostly for testing purpose.

## Features

Here's a list of the currently supported features:
* NETBIOS
  * Basic bi-directionnal NETBIOS name resolution
  * Hacky LAN SMB servers discovery (Listing all the smb servers on the LAN, no WINS, etc.)
  * Basic NETBIOS Session transport layer
* SMB
  * Support only required parts of 'NT LM 0.12' (aka CIFS?) dialect.
  * User based authentication
  * List Shares
  * Browse folders
  * Read file
  * Write file
  * Delete file
  * Delete empty folder
  * Move and rename file/folder
  * Create new folder
  * No copy, lock, RPCs, etc. [Hum... yet]

## Support

liBDSM has been tested/reported to work with the following devices/OSes:

* Windows 7
* Windows 8/8.1
* Samba
* smbX (OSX new smb implementation)
* QNAP TS-212
* A cheap NAS whose name i can't remember :)

Feel free to contribute items to this list (or network trace of not working devices)

## Building

### Requirements

* A Unix system with a bash-compatible shell
* C99 C compiler
* (GNU) Make
* Autotools
* libc with iconv
* getopt_long
* GNU tasn1 compiler/support library

The build dependencies can be installed on Debian(-based) systems using

    sudo apt-get install build-essential autoconf libtool pkg-config libtasn1-3-dev libtasn1-3-bin libbsd-dev

### HowTo

    $> ./bootstrap
    $> ./configure --prefix=/your/prefix
    $> make
    $> make install # maybe


## TODO

* HEAVILY refactor. Any help is welcome.
* Support more of the SMBv2 and v3 protocols.

## Contributing

* Fork videolabs/libdsm
* Make a feature branch
* Commits your work there
* Make a pull request
* ...
* Profit !
