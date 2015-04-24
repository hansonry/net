#ifndef __NET_H__
#define __NET_H__



typedef struct Net_SockAddr_S Net_SockAddr_T;
typedef struct Net_TCPSock_S  Net_TCPSock_T;

#define NET_DONTBLOCK 0
#define NET_BLOCK     1

#ifdef NET_OSINCLUDE

struct Net_SockAddr_S
{
   int valid_addr_flag;
   struct sockaddr_storage address;
   socklen_t len;
};

struct Net_TCPSock_S
{
   int type;
   int socket_file;
   Net_SockAddr_T addr_local;
   Net_SockAddr_T addr_remote;
};


#endif // NET_OSINCLUDE


int Net_Init(void);

void Net_Shutdown(void);

const Net_SockAddr_T * Net_TCPSockGetRemoteAddr(Net_TCPSock_T * sock);

const Net_SockAddr_T * Net_TCPSockGetLocalAddr(Net_TCPSock_T * sock);

int Net_SockAddrIsValid(const Net_SockAddr_T * addr);


size_t Net_AddrToString(const Net_SockAddr_T * addr, char * string, size_t size);

int Net_TCPConnectTo(Net_TCPSock_T * sock, const char * address_str, const char * port_str);

int Net_TCPRecv(Net_TCPSock_T * sock, void * buffer, int buffer_size, int block);

int Net_TCPSend(Net_TCPSock_T * sock, const void * buffer, int buffer_size, int block);

void Net_TCPCloseSocket(Net_TCPSock_T * sock);

void Net_TCPListenOn(Net_TCPSock_T * sock, const char * address_str, const char * port_str, int backlog);

int Net_TCPAccept(Net_TCPSock_T * server, Net_TCPSock_T * new_client, int block);

void Net_GetTCPSocketAddress(Net_SockAddr_T * address, const char * address_str, const char * port_str);

#endif // __NET_H__

