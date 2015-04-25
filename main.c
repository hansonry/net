#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NetOSInclude.h"
#include "Net.h"


int main(int args, char * argc[])
{
   const char * hi = "HI HOW ARE YOU?";
   char l_name[256];
   char r_name[256];
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

