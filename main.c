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


int main(int args, char * argc[])
{
   const char * hi = "HI HOW ARE YOU?";
   char l_name[256];
   char r_name[256];

   test_addy_cmp();
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
         server = malloc(sizeof(Net_TCPSock_T));
         client = malloc(sizeof(Net_TCPSock_T));

         Net_TCPListenOn(server, addr, port, 10);
         Net_AddrToString(Net_TCPSockGetLocalAddr(server), l_name, 256);
         printf("Bound To: %s\n", l_name);
         result = 0;
         while(result == 0)
         {
            result = Net_TCPAccept(server, client, NET_DONTBLOCK);
         }
         Net_AddrToString(Net_TCPSockGetLocalAddr(client), l_name, 256);
         Net_AddrToString(Net_TCPSockGetRemoteAddr(client), r_name, 256);
         printf("connected (%s, %s)\n", l_name, r_name);
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


         socket = malloc(sizeof(Net_TCPSock_T));
         Net_TCPConnectTo(socket, addr, port);
         Net_AddrToString(Net_TCPSockGetLocalAddr(socket), l_name, 256);
         Net_AddrToString(Net_TCPSockGetRemoteAddr(socket), r_name, 256);
         printf("connected (%s, %s)\n", l_name, r_name);
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

