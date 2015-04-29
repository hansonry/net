#ifndef __NET_H__
#define __NET_H__



typedef struct Net_SockAddr_S Net_SockAddr_T;
typedef struct Net_TCPSock_S  Net_TCPSock_T;
typedef struct Net_UDPSock_S  Net_UDPSock_T;

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

struct Net_UDPSock_S
{
   int socket_file;
   Net_SockAddr_T addr_local;
};


#endif // NET_OSINCLUDE


int Net_Init(void);

void Net_Shutdown(void);


int Net_SockAddrIsValid(const Net_SockAddr_T * addr);


size_t Net_AddrToString(const Net_SockAddr_T * addr, char * string, size_t size);

int Net_SockAddrComp(const Net_SockAddr_T * addr1, const Net_SockAddr_T * addr2);


// TCP

const Net_SockAddr_T * Net_TCPSockGetRemoteAddr(Net_TCPSock_T * sock);

const Net_SockAddr_T * Net_TCPSockGetLocalAddr(Net_TCPSock_T * sock);

int Net_TCPConnectTo(Net_TCPSock_T * sock, const char * address_str, const char * port_str);

int Net_TCPRecv(Net_TCPSock_T * sock, void * buffer, int buffer_size, int block);

int Net_TCPSend(Net_TCPSock_T * sock, const void * buffer, int buffer_size, int block);

void Net_TCPCloseSocket(Net_TCPSock_T * sock);

void Net_TCPListenOn(Net_TCPSock_T * sock, const char * address_str, const char * port_str, int backlog);

int Net_TCPAccept(Net_TCPSock_T * server, Net_TCPSock_T * new_client, int block);


// UDP

const Net_SockAddr_T * Net_UDPSockGetLocalAddr(Net_UDPSock_T * sock);

int Net_UDPSockCreate(Net_UDPSock_T * sock, const char * address_str, const char * port_str);

int Net_UDPRecv(Net_UDPSock_T * sock, Net_SockAddr_T * addr_from, void * buffer, int buffer_size, int block);

int Net_UDPSend(Net_UDPSock_T * sock, Net_SockAddr_T * addr_to, const void * buffer, int buffer_size, int block);

void Net_UDPCloseSocket(Net_UDPSock_T * sock);

void Net_SockAddrCreateUDP(Net_SockAddr_T * address, const char * address_str, const char * port_str);

#endif // __NET_H__

