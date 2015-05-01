#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Net_OSInclude.h"
#include "Net.h"


void test_addy_cmp(void)
{
   int result;
   Net_SockAddr_T addr1, addr2;
   struct sockaddr_in * ipv4_addr;
   struct sockaddr_in6 * ipv6_addr;
   unsigned char ipv6_addr1[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
   unsigned char ipv6_addr2[] = { 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
   addr1.valid_addr_flag = 1;
   addr2.valid_addr_flag = 1;

   /*
   ipv4_addr = (struct sockaddr_in*)&addr1.address;
   ipv4_addr->sin_family = AF_INET;
   ipv4_addr->sin_port = htons(1231);
   ipv4_addr->sin_addr.S_un.S_addr = htonl(0xC0A80103);
   */

   /*
   ipv4_addr = (struct sockaddr_in*)&addr2.address;
   ipv4_addr->sin_family = AF_INET;
   ipv4_addr->sin_port = htons(1234);
   ipv4_addr->sin_addr.S_un.S_addr = htonl(0xC0A80106);
   */

   ipv6_addr = (struct sockaddr_in6*)&addr1.address;
   ipv6_addr->sin6_family = AF_INET6;
   ipv6_addr->sin6_port = htons(1235);
   memcpy(&ipv6_addr->sin6_addr, ipv6_addr1, 16 );

   
   ipv6_addr = (struct sockaddr_in6*)&addr2.address;
   ipv6_addr->sin6_family = AF_INET6;
   ipv6_addr->sin6_port = htons(1234);
   memcpy(&ipv6_addr->sin6_addr, ipv6_addr2, 16 );
   

   result = Net_SockAddrComp(&addr1, &addr2);
   printf("Addr Result: %i\n", result);

}

void test_TCP_Comm(const char * type, const char * addr, const char * port)
{

   const char * hi = "HI HOW ARE YOU?";
   char l_name[256];
   char r_name[256];
   printf("%s:%s\n", addr, port);
   if(type[0] == 's')
   {
      Net_TCPSock_T * server, *client;
      int size;
      int result;

      printf("Server\n");
      Net_Init();
      server = malloc(sizeof(Net_TCPSock_T));
      client = malloc(sizeof(Net_TCPSock_T));

      Net_TCPListenOn(server, addr, port, 10);
      Net_AddrIPToString(Net_TCPSockGetLocalAddr(server), l_name, 256);
      printf("Bound To: [%s]:%i\n", l_name, Net_AddrPort(Net_TCPSockGetLocalAddr(server)));
      result = 0;
      while(result == 0)
      {
         result = Net_TCPAccept(server, client, NET_DONTBLOCK);
      }
      Net_AddrIPToString(Net_TCPSockGetLocalAddr(client), l_name, 256);
      Net_AddrIPToString(Net_TCPSockGetRemoteAddr(client), r_name, 256);
      printf("connected ([%s]:%i, [%s]:%i)\n", l_name, Net_AddrPort(Net_TCPSockGetLocalAddr(client)), r_name, Net_AddrPort(Net_TCPSockGetRemoteAddr(client)));
      size = Net_TCPSend(client, hi, strlen(hi) + 1, NET_DONTBLOCK);
      printf("Sent: [%i] %s\n", size, hi);
      //fgetc(stdin);
      Net_TCPCloseSocket(client);
      Net_TCPCloseSocket(server);
      Net_Shutdown();
      free(server);
      free(client);
   }
   else
   {
      Net_TCPSock_T * socket;
      int size;
      char resp[512];

      printf("Client\n");
      Net_Init();


      socket = malloc(sizeof(Net_TCPSock_T));
      Net_TCPConnectTo(socket, addr, port);
      Net_AddrIPToString(Net_TCPSockGetLocalAddr(socket), l_name, 256);
      Net_AddrIPToString(Net_TCPSockGetRemoteAddr(socket), r_name, 256);
      printf("connected ([%s]:%i, [%s]:%i)\n", l_name, Net_AddrPort(Net_TCPSockGetLocalAddr(socket)), r_name, Net_AddrPort(Net_TCPSockGetRemoteAddr(socket)));
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
void test_UDP_Comm(const char * type, const char * addr, const char * port)
{

   const char * hi = "HI HOW ARE YOU?";
   char r_buff[256];
   char l_name[256];
   char r_name[256];
   printf("%s:%s\n", addr, port);
   if(type[0] == 's')
   {
      Net_UDPSock_T * server;
      Net_SockAddr_T * recv_addr;
      int size;
      int result;

      printf("Server\n");
      Net_Init();
      server = malloc(sizeof(Net_TCPSock_T));
      recv_addr = malloc(sizeof(Net_SockAddr_T));

      Net_UDPSockCreate(server, addr, port);
      Net_AddrIPToString(Net_UDPSockGetLocalAddr(server), l_name, 256);
      printf("Bound To: [%s]:%i\n", l_name, Net_AddrPort(Net_UDPSockGetLocalAddr(server)));
      result = 0;
      while(result == 0)
      {
         size = Net_UDPRecv(server, recv_addr, r_buff, 256, NET_DONTBLOCK);
         if(size != 0)
         {
            result = 1;
         }

      }
      printf("Recived: [%i] %s\n", size, r_buff);

      size = Net_UDPSend(server, recv_addr, hi, strlen(hi) + 1, NET_DONTBLOCK);
      printf("Sent: [%i] %s\n", size, hi);
      //fgetc(stdin);
      Net_UDPCloseSocket(server);
      Net_Shutdown();
      free(server);
      free(recv_addr);
   }
   else
   {
      Net_UDPSock_T * socket;
      Net_SockAddr_T * send_addr;
      int size;
      char resp[512];

      printf("Client\n");
      Net_Init();


      socket = malloc(sizeof(Net_TCPSock_T));
      send_addr = malloc(sizeof(Net_SockAddr_T));
      Net_UDPSockCreate(socket, "localhost", NULL);
      Net_AddrIPToString(Net_UDPSockGetLocalAddr(socket), l_name, 256);
      printf("connected [%s]:%i\n", l_name, Net_AddrPort(Net_UDPSockGetLocalAddr(socket)));
      
      Net_SockAddrCreateUDP(send_addr, addr, port);
      Net_UDPSend(socket, send_addr, hi, strlen(hi) + 1, NET_DONTBLOCK);
      size = 0;
      while(size == 0)
      {
         size = Net_UDPRecv(socket, NULL, resp, 1023, NET_DONTBLOCK);
      }
      resp[size] = '\0'; 
      printf("Recv: %s\n", resp);

      Net_UDPCloseSocket(socket);
      free(socket);
      free(send_addr);

      Net_Shutdown();
   }
}


int main(int args, char * argc[])
{

   test_addy_cmp();
   if(args == 4)
   {
      const char * type = argc[1];
      const char * addr = argc[2];
      const char * port = argc[3];
      //test_TCP_Comm(type, addr, port);
      test_UDP_Comm(type, addr, port);

   }
   else
   {
      printf("Expected [s|c] <IPAddress> <Port>\n");
   }
   printf("End\n");
   return 0;
}

