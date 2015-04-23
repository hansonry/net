#ifdef _WIN32
#include <ws2tcpip.h>
#include <windows.h>
//#include <winsock.h>
#else // _WIN32
#endif // _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// H file
typedef struct Net_TCPSockAddr_S Net_TCPSockAddr_T;
typedef struct Net_TCPSock_S     Net_TCPSock_T;
extern size_t Net_TCPSockAddrSize;
extern size_t Net_TCPSockSize;

#define NET_DONTBLOCK 0
#define NET_BLOCK     1

// C file
struct Net_TCPSockAddr_S
{
   int valid_addr_flag;
   struct sockaddr_storage address;
};

struct Net_TCPSock_S
{
   int type;
   int socket_file;
};

size_t Net_TCPSockAddrSize = sizeof(Net_TCPSockAddr_T);
size_t Net_TCPSockSize     = sizeof(Net_TCPSock_T);

#define TYPE_TCPCLIENT 0
#define TYPE_TCPSERVER 1

int Net_Init(void)
{
   int error;

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
   return error;
}

void Net_Shutdown(void)
{
   WSACleanup();
}



int Net_TCPConnectTo(Net_TCPSock_T * sock, const char * address_str, const char * port_str)
{  

   struct addrinfo hints, *results, *loop;
   int gai_result;
   int c_result;
   int result;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags    = AI_CANONNAME | AI_ALL;
   hints.ai_family   = AF_UNSPEC;   
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   gai_result = getaddrinfo(address_str, port_str, &hints, &results);
   if(gai_result)
   {
      printf("Error Getting TCP Socket Address (%s, %s) reason: %s\n", address_str, port_str, gai_strerror(gai_result));
   }
   else
   {
      sock->socket_file = INVALID_SOCKET;
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
               closesocket(sock->socket_file); // Windows only
               sock->socket_file = INVALID_SOCKET;
            }
            else
            {
               sock->type = TYPE_TCPCLIENT;
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
   struct fd_set sock_set;
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
   struct fd_set sock_set;
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
      closesocket(sock->socket_file);
      sock->socket_file = INVALID_SOCKET;
   }
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

   gai_result = getaddrinfo(address_str, port_str, &hints, &results);
   if(gai_result)
   {
      printf("Error Getting TCP Socket Address (%s, %s)  for listening reason: %s\n", address_str, port_str, gai_strerror(gai_result));
   }
   else
   {
      sock->socket_file = INVALID_SOCKET;
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
               closesocket(sock->socket_file);
               sock->socket_file = INVALID_SOCKET;
            }
            else
            {
               b_result = bind(sock->socket_file, loop->ai_addr, loop->ai_addrlen);
               if(b_result == -1)
               {
                  closesocket(sock->socket_file); // Windows only
                  sock->socket_file = INVALID_SOCKET;
               }
               else
               {
                  sock->type = TYPE_TCPCLIENT;
                  // Exit the loop
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
            closesocket(sock->socket_file);
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
   struct fd_set sock_set;
   struct timeval zero_time;

   new_client->socket_file = INVALID_SOCKET;
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
         new_client->socket_file = accept(server->socket_file, NULL, NULL);
         if(new_client->socket_file != INVALID_SOCKET)
         {
            result = 1;
            new_client->type = TYPE_TCPCLIENT;
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

void Net_GetTCPSocketAddress(Net_TCPSockAddr_T * address, const char * address_str, const char * port_str)
{
   struct addrinfo hints, *results;
   int gai_result;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags    = AI_CANONNAME | AI_ALL;
   hints.ai_family   = AF_UNSPEC;   
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   gai_result = getaddrinfo(address_str, port_str, &hints, &results);
   if(gai_result)
   {
      printf("Error Getting TCP Socket Address (%s, %s) reason: %s\n", address_str, port_str, gai_strerror(gai_result));
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



int main(int args, char * argc[])
{
   const char * hi = "HI HOW ARE YOU?";
   if(args == 4)
   {
      const char * type = argc[1];
      const char * addr = argc[2];
      const char * port = argc[3];

      printf("%s:%s\n", addr, port);
      if(type[0] == 's')
      {
         Net_TCPSock_T * server, *client;
         int size;
         int result;

         printf("Server\n");
         Net_Init();
         server = malloc(Net_TCPSockSize);
         client = malloc(Net_TCPSockSize);

         Net_TCPListenOn(server, addr, port, 10);
         result = 0;
         while(result == 0)
         {
            result = Net_TCPAccept(server, client, NET_DONTBLOCK);
         }
         size = Net_TCPSend(client, hi, strlen(hi) + 1, NET_DONTBLOCK);
         printf("Sent: [%i] %s\n", size, hi);
         //fgetc(stdin);
         Net_TCPCloseSocket(client);
         Net_TCPCloseSocket(server);
         Net_Shutdown();
      }
      else
      {
         Net_TCPSock_T * socket;
         int size;
         char resp[512];

         printf("Client\n");
         Net_Init();


         socket = malloc(Net_TCPSockSize);
         Net_TCPConnectTo(socket, addr, port);
         printf("Connected\n");
         size = 0;
         while(size == 0)
         {
            size = Net_TCPRecv(socket, resp, 1023, NET_DONTBLOCK);
         }
         resp[size] = '\0'; 
         printf("Recv: %s\n", resp);

         Net_TCPCloseSocket(socket);
         free(socket);

         Net_Shutdown();
      }
   }
   else
   {
      printf("Expected [s|c] <IPAddress> <Port>\n");
   }
   printf("End\n");
   return 0;
}

