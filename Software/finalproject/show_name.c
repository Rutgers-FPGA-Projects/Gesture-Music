
#include "head.h"
#include "system.h"
#define lcd_write_cmd(base, data)                     IOWR(base, 0, data)
#define lcd_read_cmd(base)                            IORD(base, 1)
#define lcd_write_data(base, data)                    IOWR(base, 2, data)
#define lcd_read_data(base)                           IORD(base, 3) 
//-------------------------------------------------------------------------
void show_name(char *name)
{
  UINT16 i,j,length_temp,length_n;
  
  LCD_Init(); 
  length_temp=strlen(name);
  if(length_temp<=32)
  {
      length_n=1;
  }
  else
        length_n=(length_temp-32)/16+1;
  for(i=0;i<length_n;i++)
  {
         lcd_write_cmd(LCD_BASE,0x80);
         usleep(100);
         for(j=0;j<16;j++)
         {
          lcd_write_data(LCD_BASE,name[j+i*16]);
          usleep(100);
         }           
         lcd_write_cmd(LCD_BASE,0xC0);
         usleep(100);
         for(j=0;j<16;j++)
         {
          lcd_write_data(LCD_BASE,name[j+i*16+16]);
          usleep(100);
         }  
         if(length_n>1)
         usleep(1000000);  
         else
         usleep(300000);     
  }  
}

//-------------------------------------------------------------------------
