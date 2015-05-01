#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "Net_OSInclude.h"
#include "Net.h"


#ifdef _WIN32
#define THESET       struct fd_set
#define CLOSESOCKET  closesocket
#else // _WIN32

#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1
#define CLOSESOCKET     close
#define THESET          fd_set
#endif // _WIN32

#define TYPE_TCPCLIENT 0
#define TYPE_TCPSERVER 1

int Net_Init(void)
{
   int error;
#ifdef _WIN32

   WSADATA wsaData;
   if(WSAStartup(MAKEWORD(2,0), &wsaData) != 0)
   {
      printf("WinSock Failed to Startup\n");
      error = 1;
   }
   else
   {
      error = 0;
   }
#else
   error = 0;
#endif // _WIN32
   return error;
}

void Net_Shutdown(void)
{
#ifdef _WIN32
   WSACleanup();
#endif // _WIN32
}

const Net_SockAddr_T * Net_TCPSockGetRemoteAddr(Net_TCPSock_T * sock)
{
   Net_SockAddr_T * result;
   if(sock == NULL)
   {
      result = NULL;
   }
   else
   {
      result = &sock->addr_remote;
   }
   return result;
}

const Net_SockAddr_T * Net_TCPSockGetLocalAddr(Net_TCPSock_T * sock)
{
   Net_SockAddr_T * result;
   if(sock == NULL)
   {
      result = NULL;
   }
   else
   {
      result = &sock->addr_local;
   }
   return result;
}

int Net_SockAddrIsValid(const Net_SockAddr_T * addr)
{
   int result;
   if(addr == NULL)
   {
      result = 0;
   }
   else
   {
      result = addr->valid_addr_flag;
   }
   return result;
}

#define INVALID_SOCKET_NAME "<INVALID>"
#define NULL_SOCKET_NAME "<NULL>"
#define BUFFER_SIZE 10
#if (BUFFER_SIZE < INET_ADDRSTRLEN)
#undef BUFFER_SIZE
#define BUFFER_SIZE INET_ADDRSTRLEN
#endif 
#if (BUFFER_SIZE < INET6_ADDRSTRLEN)
#undef BUFFER_SIZE
#define BUFFER_SIZE INET6_ADDRSTRLEN
#endif 


size_t Net_AddrIPToString(const Net_SockAddr_T * addr, char * string, size_t size)
{
   size_t out_size, cpy_size;
   char buffer[BUFFER_SIZE];
   if(addr != NULL && addr->valid_addr_flag == 1)
   {

      inet_ntop(addr->address.ss_family, (void *)&addr->address, buffer, BUFFER_SIZE);
   }
   else if(addr == NULL)
   {
      strcpy(buffer, NULL_SOCKET_NAME);
   }
   else
   {
      strcpy(buffer, INVALID_SOCKET_NAME);
   }
   out_size = strlen(buffer) + 1;

   if(out_size > size)
   {
      cpy_size = size;
   }
   else
   {
      cpy_size = out_size;
   }
   if(string != NULL)
   {
      memcpy(string, buffer, cpy_size);
      string[cpy_size - 1] = '\0';
   }

   return out_size;
}

int Net_AddrPort(const Net_SockAddr_T * addr)
{
   int port;
   struct sockaddr_in  *addy_ipv4;
   struct sockaddr_in6 *addy_ipv6;
   if(addr != NULL && addr->valid_addr_flag == 1)
   {
      if(addr->address.ss_family == AF_INET)
      {
         addy_ipv4 = (struct sockaddr_in *)&addr->address;
         addy_ipv6 = NULL;
      }
      else if(addr->address.ss_family == AF_INET6)
      {
         addy_ipv4 = NULL;
         addy_ipv6 = (struct sockaddr_in6 *)&addr->address;
      }
      else
      {
         addy_ipv4 = NULL;
         addy_ipv6 = NULL; 
      }

      if(addy_ipv4 != NULL)
      {
         port = ntohs(addy_ipv4->sin_port);
      }
      else if(addy_ipv6 != NULL)
      {
         port = ntohs(addy_ipv6->sin6_port);
      }
      else
      {
         port = -1;
      }
   }
   else
   {
      port = -1;
   }
   return port;
}

int Net_SockAddrComp(const Net_SockAddr_T * addr1, const Net_SockAddr_T * addr2)
{
   int diff, p_diff;
   struct sockaddr_in  *addy1_ipv4, *addy2_ipv4;
   struct sockaddr_in6 *addy1_ipv6, *addy2_ipv6;
   uint32_t ip1_ipv4, ip2_ipv4;
   uint16_t port1, port2;
   int i;
   if(addr1->valid_addr_flag == 1 && addr1->valid_addr_flag == 1)
   {
      diff = addr1->address.ss_family - addr2->address.ss_family;

      if(diff == 0)
      {
         if(addr1->address.ss_family == AF_INET)
         {
            addy1_ipv4 = (struct sockaddr_in *)&addr1->address;
            addy2_ipv4 = (struct sockaddr_in *)&addr2->address;
            addy1_ipv6 = NULL;
            addy2_ipv6 = NULL;
         }
         else if(addr1->address.ss_family == AF_INET6)
         {
            addy1_ipv4 = NULL;
            addy2_ipv4 = NULL;
            addy1_ipv6 = (struct sockaddr_in6 *)&addr1->address;
            addy2_ipv6 = (struct sockaddr_in6 *)&addr2->address;
         }
         else
         {
            addy1_ipv4 = NULL;
            addy2_ipv4 = NULL;
            addy1_ipv6 = NULL;
            addy2_ipv6 = NULL; 
         }
         
         if(addy1_ipv4 != NULL)
         {
            ip1_ipv4 = ntohl(addy1_ipv4->sin_addr.s_addr);
            ip2_ipv4 = ntohl(addy2_ipv4->sin_addr.s_addr);
            diff = (int)(ip1_ipv4 - ip2_ipv4);
         }
         else if(addy1_ipv6 != NULL)
         {
            diff = 0;
            for(i = 0; i < 16; i++)
            {
               p_diff = addy1_ipv6->sin6_addr.s6_addr[i] - 
                        addy2_ipv6->sin6_addr.s6_addr[i];

               if(diff == 0)
               {
                  if(p_diff < 0)
                  {
                     diff = -i;
                  }
                  else if(p_diff > 0)
                  {
                     diff = i;
                  }
               }
            }
         }
         else
         {
            diff = -1;
         }

         if(diff == 0)
         {

            if(addy1_ipv4 != NULL)
            {
               port1 = ntohs(addy1_ipv4->sin_port);
               port2 = ntohs(addy2_ipv4->sin_port);
            }
            else if(addy1_ipv6 != NULL)
            {
               port1 = ntohs(addy1_ipv6->sin6_port);
               port2 = ntohs(addy2_ipv6->sin6_port);
            }
            else
            {
               port1 = 0;
               port2 = 1;
            }
            diff = (int)(port1 - port2);
         }
      }
   }
   else
   {
      diff = -1;
   }
   return diff;

}

int Net_TCPConnectTo(Net_TCPSock_T * sock, const char * address_str, const char * port_str)
{  

   struct addrinfo hints, *results, *loop;
   int gai_result;
   int gsn_result;
   int c_result;
   int result;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags    = AI_CANONNAME | AI_ALL;
   hints.ai_family   = AF_UNSPEC;   
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   sock->socket_file = INVALID_SOCKET;
   sock->addr_local.valid_addr_flag  = 0;
   sock->addr_remote.valid_addr_flag = 0;

   gai_result = getaddrinfo(address_str, port_str, &hints, &results);
   if(gai_result)
   {
      printf("Error Getting TCP Socket Address (%s, %s) reason: %s\n", address_str, port_str, gai_strerror(gai_result));
   }
   else
   {
      loop = results;
      while(loop != NULL && sock->socket_file == INVALID_SOCKET)
      {
         sock->socket_file = socket(loop->ai_family, loop->ai_socktype, loop->ai_protocol);
         if(sock->socket_file == INVALID_SOCKET)
         {
            // Move on to the next Address to try
         }
         else
         {
            c_result = connect(sock->socket_file, loop->ai_addr, loop->ai_addrlen);
            if(c_result == -1)
            {
               CLOSESOCKET(sock->socket_file); // Windows only
               sock->socket_file = INVALID_SOCKET;
            }
            else
            {
               sock->type = TYPE_TCPCLIENT;
               memcpy(&sock->addr_remote.address, loop->ai_addr, loop->ai_addrlen);
               sock->addr_remote.len = loop->ai_addrlen;
               sock->addr_remote.valid_addr_flag = 1;

               sock->addr_local.len = sizeof(struct sockaddr_storage);
               gsn_result = getsockname(sock->socket_file, (struct sockaddr * )&sock->addr_local.address, &sock->addr_local.len);
               if(gsn_result == 0)
               {
                  sock->addr_local.valid_addr_flag = 1;
               }
               else
               {
                  sock->addr_local.valid_addr_flag = 0;
               }
               // Exit the loop
            }
         }


         loop = loop->ai_next;
      }
      freeaddrinfo(results);

      if(sock->socket_file == INVALID_SOCKET)
      {
         printf("Error Connecting to socket address (%s, %s)\n", address_str, port_str);
      }
   }
   if(sock->socket_file != INVALID_SOCKET)
   {
      result = 1;
   }
   else
   {
      result = 0;
   }
   return result;
}

int Net_TCPRecv(Net_TCPSock_T * sock, void * buffer, int buffer_size, int block)
{
   int result;
   int run_cmd;
   int s_result;
   THESET sock_set;
   struct timeval zero_time;

   if(sock->socket_file == INVALID_SOCKET || sock->type != TYPE_TCPCLIENT)
   {
      result = -1;
   }
   else
   {
      if(block == NET_BLOCK) 
      {
         run_cmd = 1;
      }
      else
      {
         zero_time.tv_sec  = 0;
         zero_time.tv_usec = 0;
         FD_ZERO(&sock_set);
         FD_SET(sock->socket_file, &sock_set);
         s_result = select(sock->socket_file + 1, &sock_set, NULL, NULL, &zero_time);
         if(s_result != SOCKET_ERROR && s_result != 0)
         {
            run_cmd = 1;
         }
         else
         {
            run_cmd = 0;
         }
      }

      if(run_cmd == 1)
      {
         result = recv(sock->socket_file, buffer, buffer_size, 0);
      }
      else
      {
         result = 0;
      }
   }
   return result;
}

int Net_TCPSend(Net_TCPSock_T * sock, const void * buffer, int buffer_size, int block)
{
   int result;
   int run_cmd;
   int s_result;
   THESET sock_set;
   struct timeval zero_time;

   if(sock->socket_file == INVALID_SOCKET || sock->type != TYPE_TCPCLIENT)
   {
      result = -1;
   }
   else
   {
      if(block == NET_BLOCK) 
      {
         run_cmd = 1;
      }
      else
      {
         zero_time.tv_sec  = 0;
         zero_time.tv_usec = 0;
         FD_ZERO(&sock_set);
         FD_SET(sock->socket_file, &sock_set);
         s_result = select(sock->socket_file + 1, NULL, &sock_set, NULL, &zero_time);
         if(s_result != SOCKET_ERROR && s_result != 0)
         {
            run_cmd = 1;
         }
         else
         {
            run_cmd = 0;
         }
      }
      
      if(run_cmd == 1)
      {
         result = send(sock->socket_file, buffer, buffer_size, 0);
      }
      else
      {
         result = 0;
      }
   }
   return result;
}

void Net_TCPCloseSocket(Net_TCPSock_T * sock)
{
   if(sock->socket_file != INVALID_SOCKET)
   {
      CLOSESOCKET(sock->socket_file);
      sock->socket_file = INVALID_SOCKET;
   }

   sock->addr_local.valid_addr_flag  = 0;
   sock->addr_remote.valid_addr_flag = 0;
}

void Net_TCPListenOn(Net_TCPSock_T * sock, const char * address_str, const char * port_str, int backlog)
{
   struct addrinfo hints, *results, *loop;
   int gai_result;
   int b_result, s_result;
   int yes;
   yes = 1;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags    = AI_PASSIVE | AI_ALL;
   hints.ai_family   = AF_UNSPEC;   
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   sock->socket_file = INVALID_SOCKET;
   sock->addr_local.valid_addr_flag  = 0;
   sock->addr_remote.valid_addr_flag = 0;

   gai_result = getaddrinfo(address_str, port_str, &hints, &results);
   if(gai_result)
   {
      printf("Error Getting TCP Socket Address (%s, %s)  for listening reason: %s\n", address_str, port_str, gai_strerror(gai_result));
   }
   else
   {
      loop = results;
      while(loop != NULL && sock->socket_file == INVALID_SOCKET)
      {
         sock->socket_file = socket(loop->ai_family, loop->ai_socktype, loop->ai_protocol);
         if(sock->socket_file == INVALID_SOCKET)
         {
            // Move on to the next Address to try
         }
         else
         {
            s_result = setsockopt(sock->socket_file, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(int));
            if(s_result == -1)
            {
               printf("Error setting server socket to Resue address\n");
               CLOSESOCKET(sock->socket_file);
               sock->socket_file = INVALID_SOCKET;
            }
            else
            {
               b_result = bind(sock->socket_file, loop->ai_addr, loop->ai_addrlen);
               if(b_result == -1)
               {
                  CLOSESOCKET(sock->socket_file); // Windows only
                  sock->socket_file = INVALID_SOCKET;
               }
               else
               {
                  // Exit the loop
                  memcpy(&sock->addr_local.address, loop->ai_addr, loop->ai_addrlen);
                  sock->addr_local.len = loop->ai_addrlen;
                  sock->addr_local.valid_addr_flag = 1;
                  sock->addr_remote.valid_addr_flag = 0;
               }
            }
         }


         loop = loop->ai_next;
      }
      freeaddrinfo(results);

      if(sock->socket_file == INVALID_SOCKET)
      {
         printf("Error Listening on socket address (%s, %s)\n", address_str, port_str);
      }
      else
      {
         if(listen(sock->socket_file, backlog) == -1)
         {
            printf("Failed To Setup Backlog on socket address (%s, %s)\n", address_str, port_str);
            CLOSESOCKET(sock->socket_file);
            sock->socket_file = INVALID_SOCKET;
         }
         else
         {
            sock->type = TYPE_TCPSERVER;
         }
      }
   }

}

int Net_TCPAccept(Net_TCPSock_T * server, Net_TCPSock_T * new_client, int block)
{
   int result;
   int run_cmd;
   int s_result;
   int info_result;
   THESET sock_set;
   struct timeval zero_time;

   new_client->socket_file = INVALID_SOCKET;
   new_client->addr_remote.valid_addr_flag = 0;
   new_client->addr_local.valid_addr_flag  = 0;

   if(server->type == TYPE_TCPSERVER)
   {
      if(block == NET_BLOCK) 
      {
         run_cmd = 1;
      }
      else
      {
         zero_time.tv_sec  = 0;
         zero_time.tv_usec = 0;
         FD_ZERO(&sock_set);
         FD_SET(server->socket_file, &sock_set);
         s_result = select(server->socket_file + 1, &sock_set, NULL, NULL, &zero_time);
         if(s_result != SOCKET_ERROR && s_result != 0)
         {
            run_cmd = 1;
         }
         else
         {
            run_cmd = 0;
         }
      }

      if(run_cmd == 1)
      {
         new_client->addr_remote.len = sizeof(struct sockaddr_storage);
         new_client->socket_file = accept(server->socket_file, (struct sockaddr *)&new_client->addr_remote.address, &new_client->addr_remote.len);
         if(new_client->socket_file != INVALID_SOCKET)
         {
            result = 1;
            new_client->type = TYPE_TCPCLIENT;
            new_client->addr_remote.valid_addr_flag = 1;

            new_client->addr_local.len = sizeof(struct sockaddr_storage);
            info_result = getsockname(new_client->socket_file, 
                                      (struct sockaddr *)&new_client->addr_local.address, 
                                      &new_client->addr_local.len);
            printf("info_result: %i\n", info_result);
            if(info_result == 0)
            {
               new_client->addr_local.valid_addr_flag = 1;
               
            }
            
         }
         else
         {
            result = 0;
         }
      }
      else
      {
         result = 0;
      }

   }
   else
   {
      result = 0;
   }


   return result;
}

// UDP


const Net_SockAddr_T * Net_UDPSockGetLocalAddr(Net_UDPSock_T * sock)
{
   Net_SockAddr_T * result;
   if(sock == NULL)
   {
      result = NULL;
   }
   else
   {
      result = &sock->addr_local;
   }
   return result;
}

void Net_SockAddrCreateUDP(Net_SockAddr_T * address, const char * address_str, const char * port_str)
{
   struct addrinfo hints, *results;
   int gai_result;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags    = AI_CANONNAME | AI_ALL;
   hints.ai_family   = AF_UNSPEC;   
   hints.ai_socktype = SOCK_DGRAM;
   hints.ai_protocol = IPPROTO_UDP;

   gai_result = getaddrinfo(address_str, port_str, &hints, &results);
   if(gai_result)
   {
      printf("Error Getting UDP Socket Address (%s, %s) reason: %s\n", address_str, port_str, gai_strerror(gai_result));
      address->valid_addr_flag = 0;
   }
   else
   {

      //inet_ntop(loop->ai_family, loop->ai_addr, name, 128);
      //printf("result %s %s\n", loop->ai_canonname, name);
      // Copy the address
      memcpy(&address->address, results->ai_addr, results->ai_addrlen);
      address->valid_addr_flag = 1;
      freeaddrinfo(results);
   }

}



int Net_UDPSockCreate(Net_UDPSock_T * sock, const char * address_str, const char * port_str)
{

   struct addrinfo hints, *results, *loop;
   int gai_result;
   int gsn_result;
   int b_result;
   int result;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags    = AI_PASSIVE | AI_ALL;
   hints.ai_family   = AF_UNSPEC;   
   hints.ai_socktype = SOCK_DGRAM;
   hints.ai_protocol = IPPROTO_UDP;

   sock->socket_file = INVALID_SOCKET;
   sock->addr_local.valid_addr_flag  = 0;

   gai_result = getaddrinfo(address_str, port_str, &hints, &results);
   if(gai_result)
   {
      printf("Error Getting UDP Socket Address (%s, %s) reason: %s\n", address_str, port_str, gai_strerror(gai_result));
   }
   else
   {
      loop = results;
      while(loop != NULL && sock->socket_file == INVALID_SOCKET)
      {
         sock->socket_file = socket(loop->ai_family, loop->ai_socktype, loop->ai_protocol);
         if(sock->socket_file == INVALID_SOCKET)
         {
            // Move on to the next Address to try
         }
         else
         {
            if(port_str != NULL)
            {
               b_result = bind(sock->socket_file, loop->ai_addr, loop->ai_addrlen);
            }
            else
            {
               b_result = 0; // No port so the bind was successful
            }

            if(b_result == -1)
            {
               CLOSESOCKET(sock->socket_file);
               sock->socket_file = INVALID_SOCKET;
            }
            else
            {
               memcpy(&sock->addr_local.address, loop->ai_addr, loop->ai_addrlen);
               sock->addr_local.len = loop->ai_addrlen;
               sock->addr_local.valid_addr_flag = 1;

               // Exit the loop
            }
         }


         loop = loop->ai_next;
      }
      freeaddrinfo(results);

      if(sock->socket_file == INVALID_SOCKET)
      {
         printf("Error Connecting to socket address (%s, %s)\n", address_str, port_str);
      }
   }
   if(sock->socket_file != INVALID_SOCKET)
   {
      result = 1;
   }
   else
   {
      result = 0;
   }
   return result;
}


int Net_UDPRecv(Net_UDPSock_T * sock, Net_SockAddr_T * addr_from, void * buffer, int buffer_size, int block)
{
   int result;
   int run_cmd;
   int s_result;
   THESET sock_set;
   struct timeval zero_time;

   if(addr_from != NULL)
   {
      addr_from->valid_addr_flag = 0;
   }

   if(sock->socket_file == INVALID_SOCKET)
   {
      result = -1;
   }
   else
   {
      if(block == NET_BLOCK) 
      {
         run_cmd = 1;
      }
      else
      {
         zero_time.tv_sec  = 0;
         zero_time.tv_usec = 0;
         FD_ZERO(&sock_set);
         FD_SET(sock->socket_file, &sock_set);
         s_result = select(sock->socket_file + 1, &sock_set, NULL, NULL, &zero_time);
         if(s_result != SOCKET_ERROR && s_result != 0)
         {
            run_cmd = 1;
         }
         else
         {
            run_cmd = 0;
         }
      }

      if(run_cmd == 1)
      {
         if(addr_from != NULL)
         {
            addr_from->len = sizeof(struct sockaddr_storage);
            result = recvfrom(sock->socket_file, buffer, buffer_size, 0, (struct sockaddr *)&addr_from->address, &addr_from->len);
            addr_from->valid_addr_flag = 1;
         }
         else
         {
            result = recvfrom(sock->socket_file, buffer, buffer_size, 0, NULL, NULL);
         }
      }
      else
      {
         result = 0;
      }
   }
   return result;
}

int Net_UDPSend(Net_UDPSock_T * sock, Net_SockAddr_T * addr_to, const void * buffer, int buffer_size, int block)
{
   int result;
   int run_cmd;
   int s_result;
   THESET sock_set;
   struct timeval zero_time;

   if(sock->socket_file == INVALID_SOCKET || addr_to->valid_addr_flag != 1)
   {
      result = -1;
      printf("here\n");
   }
   else
   {
      if(block == NET_BLOCK) 
      {
         run_cmd = 1;
      }
      else
      {
         zero_time.tv_sec  = 0;
         zero_time.tv_usec = 0;
         FD_ZERO(&sock_set);
         FD_SET(sock->socket_file, &sock_set);
         s_result = select(sock->socket_file + 1, NULL, &sock_set, NULL, &zero_time);
         if(s_result != SOCKET_ERROR && s_result != 0)
         {
            run_cmd = 1;
         }
         else
         {
            run_cmd = 0;
         }
      }
      
      if(run_cmd == 1)
      {
         result = sendto(sock->socket_file, buffer, buffer_size, 0, (struct sockaddr *)&addr_to->address, addr_to->len);
      }
      else
      {
         result = 0;
      }
   }
   return result;
}

void Net_UDPCloseSocket(Net_UDPSock_T * sock)
{
   if(sock->socket_file != INVALID_SOCKET)
   {
      CLOSESOCKET(sock->socket_file);
      sock->socket_file = INVALID_SOCKET;
   }

   sock->addr_local.valid_addr_flag  = 0;
}



