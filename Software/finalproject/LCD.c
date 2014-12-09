#include "head.h"
#include "system.h"
#define lcd_write_cmd(base, data)                     IOWR(base, 0, data)
#define lcd_read_cmd(base)                            IORD(base, 1)
#define lcd_write_data(base, data)                    IOWR(base, 2, data)
#define lcd_read_data(base)                           IORD(base, 3) 
//-------------------------------------------------------------------------
void LCD_Init()
{
  lcd_write_cmd(LCD_BASE,0x38);
  usleep(2000);
  lcd_write_cmd(LCD_BASE,0x0C);
  usleep(2000);
  lcd_write_cmd(LCD_BASE,0x01);
  usleep(2000);
  lcd_write_cmd(LCD_BASE,0x06);
  usleep(2000);
  lcd_write_cmd(LCD_BASE,0x80);
  usleep(2000);
}
//-------------------------------------------------------------------------
void LCD_Show_Text(char* Text)
{
  int i;
  for(i=0;i<strlen(Text);i++)
  {
    lcd_write_data(LCD_BASE,Text[i]);
    usleep(2000);
  }
}
//-------------------------------------------------------------------------
void LCD_Line2()
{
  lcd_write_cmd(LCD_BASE,0xC0);
  usleep(2000);
}
//-------------------------------------------------------------------------
void LCD_begin()
{
  char line1[16] = "Welcome to Our ";
  char line2[16] = "Final Project";
  //  Initialize the LCD
  LCD_Init();
  LCD_Show_Text(line1);
  LCD_Line2();
  LCD_Show_Text(line2);
}
