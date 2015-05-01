#ifndef __NET_OSINCLUDE_H__
#define __NET_OSINCLUDE_H__


#ifdef _WIN32
#include <ws2tcpip.h>
#include <windows.h>
#else // _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#endif // _WIN32

#define NET_OSINCLUDE

#endif // __NET_OSINCLUDE_H__

