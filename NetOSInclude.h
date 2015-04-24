#ifndef __NETOSINCLUDE_H__
#define __NETOSINCLUDE_H__


#ifdef _WIN32
#include <ws2tcpip.h>
#include <windows.h>
#else // _WIN32
#endif // _WIN32

#define NET_OSINCLUDE

#endif // __NETOSINCLUDE_H__

