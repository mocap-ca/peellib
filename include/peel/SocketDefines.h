#ifndef PEEL_SOCKET_DEFINES_H
#define PEEL_SOCKET_DEFINES_H

// Include platform sepecific headers and create definitions
// for platform specific values
#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <Ws2tcpip.h>
#  include <winsock2.h>
#  include "Wininet.h"
#  define SOCKET_WOULDBLOCK WSAEWOULDBLOCK
#  define SOCKET_CONNRESET  WSAECONNRESET
#  define SOCKET_NULL       NULL
#  define CLOSESOCKET       closesocket
#  define SOCKLENTYPE       int
#else
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <errno.h>
#  include <unistd.h>
#  include <fcntl.h>

#  define SOCKET             int
#  define SOCKET_WOULDBLOCK  EWOULDBLOCK
#  define SOCKET_NULL        0
#  define INVALID_SOCKET     -1
#  define SOCKET_ERROR       -1
#  define CLOSESOCKET        close
#  define SOCKADDR           sockaddr
#  define SOCKLENTYPE        socklen_t
#endif

#endif
