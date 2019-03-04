#include "../lib/udplink.h"
#include <unistd.h>
#include <stdio.h>
 

void onData(const char* data, uint32_t len)
{
   printf(">>>%d : %s\n", len, data);
}

int main(int argc, char *argv[])
{
   udpLink_t link, link_srv;
   uint32_t i=0;
   char recvBuffer[100];

   udpInit(&link, "127.0.0.1", 88788, false);

   udpInitRecvThread(&link_srv, "127.0.0.1", 88788, &onData, 1, recvBuffer, sizeof(recvBuffer));

   while(true)
   {
      char buf[10];
      snprintf(buf, sizeof(buf), "hello %u", i++);
      int ret = udpSend(&link, buf, sizeof(buf));
      printf("<%d : %s\n", ret, buf);
      usleep(100*1000);
   }

   return 0;
}