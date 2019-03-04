#include "../lib/udplink.h"
#include <unistd.h>
#include <stdio.h>
 
int main(int argc, char *argv[])
{
   udpLink_t link;
   uint32_t i=0;

   udpInit(&link, "127.0.0.1", 88788, true);
   while(true)
   {
      char buf[10] = {0};
      int ret = udpRecv(&link, buf, sizeof(buf), 500);
      printf(">%d : %s\n", ret, buf);
//      usleep(1000*1000);
   }

   return 0;
}