
#include "system.h"
#include "head.h"
void time(int j,int n,int num)
{
   int tim[2],h[2],m[2],s[2],b[2];
   unsigned int display; 
   j=j/375; 
   tim[0]=j/60;   //min
   tim[1]=j%60;  //s
   
   m[1]=tim[0]%10;
   m[0]=tim[0]/10;
   s[1]=tim[1]%10;
   s[0]=tim[1]/10;
   b[1]=n%10;
   b[0]=n/10;
   h[1]=num%10;
   h[0]=num/10;

   
   display=b[0]*16*16*16*16*16*16*16+b[1]*16*16*16*16*16*16
           +h[0]*16*16*16*16*16+h[1]*16*16*16*16+m[0]*16*16*16+m[1]*16*16+s[0]*16+s[1];

   IOWR(SEG7_BASE,0,display);

}

