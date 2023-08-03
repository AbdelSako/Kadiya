/*
                    GNU GENERAL PUBLIC LICENSE
                       Version 2, June 1991
*/


/* Definition of types and variable that can be used on both Linux and Windows */
#ifndef __TCPstuff
#define __TCPstuff

#ifdef _WIN32

#define SHUT_RDWR SD_BOTH
#define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT //Socket type not supported
#define ssize_t INT64
#define ioctl ioctlsocket
#define SIGPIPE EPIPE
#define EWOULDBLOCK WSAEWOULDBLOCK

#endif
#endif
