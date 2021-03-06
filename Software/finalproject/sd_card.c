#include"head.h"
#include "system.h"
BYTE response_R(BYTE);
BYTE send_cmd(BYTE *);
BYTE SD_read_lba(BYTE *,UINT32,UINT32);
BYTE SD_card_init(void);
// new edition----------
BYTE write_status;  
//--- new over----
//-------------------------------------------------------------------------
BYTE read_status;
BYTE response_buffer[20];
BYTE RCA[2];
BYTE cmd_buffer[5];
//-------------------------------------------------------------------------
const BYTE cmd0[5]   = {0x40,0x00,0x00,0x00,0x00};//initial and reset sd card
const BYTE cmd55[5]  = {0x77,0x00,0x00,0x00,0x00};//start initial
const BYTE cmd2[5]   = {0x42,0x00,0x00,0x00,0x00};//obtain the CID information of SD card
const BYTE cmd3[5]   = {0x43,0x00,0x00,0x00,0x00};//obtain the related address of SD card
const BYTE cmd7[5]   = {0x47,0x00,0x00,0x00,0x00};//choose RCA
const BYTE cmd9[5]   = {0x49,0x00,0x00,0x00,0x00};//obtain the storage information of SD card
const BYTE cmd16[5]  = {0x50,0x00,0x00,0x02,0x00};//set the size of block
const BYTE cmd17[5]  = {0x51,0x00,0x00,0x00,0x00};// read data from one block
// -----  new ------
const BYTE cmd24[5]  = {0x58,0x00,0x00,0x00,0x00};//write data to block
// ----- new over ----
const BYTE acmd6[5]  = {0x46,0x00,0x00,0x00,0x02};//set the width of bus
const BYTE acmd41[5] = {0x69,0x0f,0xf0,0x00,0x00};//obtain the voltage of sd card
const BYTE acmd51[5] = {0x73,0x00,0x00,0x00,0x00};//obtain SCR register value
//-------------------------------------------------------------------------

//  SD Card Set I/O Direction
#define SD_CMD_IN   IOWR(SD_CMD_BASE, 1, 0)
#define SD_CMD_OUT  IOWR(SD_CMD_BASE, 1, 1)
#define SD_DAT_IN   IOWR(SD_DAT_BASE, 1, 0)
#define SD_DAT_OUT  IOWR(SD_DAT_BASE, 1, 1)
//  SD Card Output High/Low
#define SD_CMD_LOW  IOWR(SD_CMD_BASE, 0, 0)
#define SD_CMD_HIGH IOWR(SD_CMD_BASE, 0, 1)
#define SD_DAT_LOW  IOWR(SD_DAT_BASE, 0, 0)
#define SD_DAT_HIGH IOWR(SD_DAT_BASE, 0, 1)
#define SD_CLK_LOW  IOWR(SD_CLK_BASE, 0, 0)
#define SD_CLK_HIGH IOWR(SD_CLK_BASE, 0, 1)
//  SD Card Input Read
#define SD_TEST_CMD IORD(SD_CMD_BASE, 0)
#define SD_TEST_DAT IORD(SD_DAT_BASE, 0)
void Ncr(void)
{
  SD_CMD_IN;
  SD_CLK_LOW;
  SD_CLK_HIGH;
  SD_CLK_LOW;
  SD_CLK_HIGH;
} 
//-------------------------------------------------------------------------
void Ncc(void)
{
  int i;
  for(i=0;i<8;i++)
  {
    SD_CLK_LOW;
    SD_CLK_HIGH;
  }
}
//-------------------------------------------------------------------------
BYTE SD_card_init(void)
{
    BYTE x,y;
    SD_CMD_OUT;
    SD_DAT_IN;
    SD_CLK_HIGH;
    SD_CMD_HIGH;
    SD_DAT_LOW;
    read_status=0;
    for(x=0;x<40;x++)
    Ncr();
    for(x=0;x<5;x++)
    cmd_buffer[x]=cmd0[x];
    y = send_cmd(cmd_buffer);
    do
    {
      for(x=0;x<40;x++);
      Ncc();
      for(x=0;x<5;x++)
      cmd_buffer[x]=cmd55[x];
      y = send_cmd(cmd_buffer);
      Ncr();
      if(response_R(1)>1) //response too long or crc error
      return 1;
      Ncc();
      for(x=0;x<5;x++)
      cmd_buffer[x]=acmd41[x];
      y = send_cmd(cmd_buffer);
      Ncr();      
    } while(response_R(3)==1);
    Ncc();
    for(x=0;x<5;x++)
    cmd_buffer[x]=cmd2[x];
    y = send_cmd(cmd_buffer);
    Ncr();
    if(response_R(2)>1)
    return 1;
    Ncc();
    for(x=0;x<5;x++)
    cmd_buffer[x]=cmd3[x];
    y = send_cmd(cmd_buffer);
    Ncr();
    if(response_R(6)>1)
    return 1;         
    RCA[0]=response_buffer[1];
    RCA[1]=response_buffer[2];
    Ncc();
    for(x=0;x<5;x++)
    cmd_buffer[x]=cmd9[x];
    cmd_buffer[1] = RCA[0];
    cmd_buffer[2] = RCA[1];  
    y = send_cmd(cmd_buffer);
    Ncr();
    if(response_R(2)>1)
    return 1; 
    Ncc();
    for(x=0;x<5;x++)
    cmd_buffer[x]=cmd7[x];
    cmd_buffer[1] = RCA[0];
    cmd_buffer[2] = RCA[1];
    y = send_cmd(cmd_buffer);
    Ncr();
    if(response_R(1)>1)
    return 1; 
    Ncc();
    for(x=0;x<5;x++)
    cmd_buffer[x]=cmd16[x];
    y = send_cmd(cmd_buffer);  
    Ncr();
    if(response_R(1)>1)
    return 1;   //not ready
    read_status =1; //sd card ready
    return 0;
}
//-------------------------------------------------------------------------
BYTE SD_read_lba(BYTE *buff,UINT32 lba,UINT32 seccnt)
{
  BYTE c=0;
  UINT32  i,j;
  //lba+=101;
  for(j=0;j<seccnt;j++)
  {
    {
      Ncc();
      cmd_buffer[0] = cmd17[0];
      cmd_buffer[1] = (lba>>15)&0xff;
      cmd_buffer[2] = (lba>>7)&0xff;
      cmd_buffer[3] = (lba<<1)&0xff;
      cmd_buffer[4] = 0;
      lba++;
      send_cmd(cmd_buffer); 
      Ncr();

    } //LCD_Show_Text("ee");
    while(1)
    {
      SD_CLK_LOW;
      SD_CLK_HIGH;
      IOWR(SD_DAT_BASE, 0, 1);

      if((IORD(SD_DAT_BASE, 0)))
      break;
    }
    for(i=0;i<512;i++)
    {
      BYTE j;
      for(j=0;j<8;j++)
      {
        SD_CLK_LOW;
        SD_CLK_HIGH;
        c <<= 1; 
        if(SD_TEST_DAT)
        c |= 0x01;
      } 
      *buff=c;
      buff++;
    } 
    for(i=0; i<16; i++)
    {
        SD_CLK_LOW;
        SD_CLK_HIGH;
    }
  }
  read_status = 1;  //SD data next in
  return 0;

}
//-------------------------------------------------------------------------
BYTE response_R(BYTE s)
{
  BYTE a=0,b=0,c=0,r=0,crc=0;
  BYTE i,j=6,k;
  while(1)
  {
    SD_CLK_LOW;
    SD_CLK_HIGH;
    if(!(SD_TEST_CMD))
    break;
    if(crc++ >100)
    return 2;
  } 
  crc =0;
  if(s == 2)
  j = 17;

  for(k=0; k<j; k++)
  {
    c = 0;
    if(k > 0)                      //for crc culcar
    b = response_buffer[k-1];    
    for(i=0; i<8; i++)
    {
      SD_CLK_LOW;
      if(a > 0)
      c <<= 1; 
      else
      i++; 
      a++; 
      SD_CLK_HIGH;
      if(SD_TEST_CMD)
      c |= 0x01;
      if(k > 0)
      {
        crc <<= 1;
        if((crc ^ b) & 0x80)
        crc ^= 0x09;
        b <<= 1;
        crc &= 0x7f;
      }
    }
    if(s==3)
    { 
      if( k==1 &&(!(c&0x80)))
      r=1;
    }
    response_buffer[k] = c;
  }
  if(s==1 || s==6)
  {
    if(c != ((crc<<1)+1))
    r=2;
  } 
  return r; 
}
//-------------------------------------------------------------------------
BYTE send_cmd(BYTE *in)
{
  int i,j;
  BYTE b,crc=0;
  SD_CMD_OUT;
  for(i=0; i < 5; i++)
  {
    b = in[i];
    for(j=0; j<8; j++)
    {
      SD_CLK_LOW;
      if(b&0x80)
      SD_CMD_HIGH;
      else
      SD_CMD_LOW; 
      crc <<= 1;
      SD_CLK_HIGH;
      if((crc ^ b) & 0x80)
      crc ^= 0x09;
      b<<=1;
    } 
    crc &= 0x7f; 
  }  
  crc =((crc<<1)|0x01);
  b = crc; 
  for(j=0; j<8; j++)
  {
    SD_CLK_LOW;
    if(crc&0x80)
    SD_CMD_HIGH;
    else
    SD_CMD_LOW; 
    SD_CLK_HIGH;
    crc<<=1;
  }    
  return b;   
}

//-------------------------------------------------------------------------
BYTE SD_read_block(UINT32 block_number, BYTE *buff)
{
    SD_CMD_OUT;
    SD_DAT_IN;/////IN   card to  host
    BYTE c=0;
    UINT32  i,lba;
    lba = block_number *512;
    {
      Ncc();
      cmd_buffer[0] = cmd17[0];
      cmd_buffer[1] = (lba>>24)&0xff;
      cmd_buffer[2] = (lba>>16)&0xff;
      cmd_buffer[3] = (lba>>8)&0xff;
      cmd_buffer[4] = lba&0xff;
      send_cmd(cmd_buffer); 
      Ncr();
    } 
    while(1)
    {
      SD_CLK_LOW;
      SD_CLK_HIGH;
      if((SD_TEST_DAT & 0x01) == 0)
      break;
    }
    for(i=0;i<513;i++)
    {
      BYTE j;
      for(j=0;j<8;j++)
      {
        SD_CLK_LOW;
        SD_CLK_HIGH;
        c <<= 1; 
        if((SD_TEST_DAT & 0x01))
        c |= 0x01;
      } 
      *buff=c;
      buff++;
    } 
    for(i=0; i<16; i++)
    {
        SD_CLK_LOW;
        SD_CLK_HIGH;
    }
    read_status = 1;  //SD data next in
    return 0;
}
//---------------------------------------------------------------------

BYTE SD_write_block(UINT32 block_number, BYTE *buff)
{
    UINT16 b = 0;
    UINT16 crc=0;
    BYTE temp = 0;
    BYTE crc_status = 0;
    
    
    SD_DAT_OUT;
    
    UINT32 i,k,lba;
    lba = block_number * 512;

    Ncc();
    cmd_buffer[0] = cmd24[0];
    cmd_buffer[1] = (lba>>24)&0xff;
    cmd_buffer[2] = (lba>>16)&0xff;
    cmd_buffer[3] = (lba>>8)&0xff;
    cmd_buffer[4] = lba&0xff;
    send_cmd(cmd_buffer); 
    Ncr();

    if(response_R(1)>1)
    return 1;
    Ncc();
    
    
    SD_CLK_LOW;
    SD_DAT_LOW;
    SD_CLK_HIGH;

    
    for(i=0; i < 512; i++) 
    {
        temp = buff [ i ];
        
        for(k=0x80; k!=0; k=k>>1) 
        {
          SD_CLK_LOW;   

          if(temp&k)     
          SD_DAT_HIGH; 
          else 
          SD_DAT_LOW; 

          SD_CLK_HIGH;   
          
          b = b*2;
          if((b&0x10000))
          b = b^0x11021;
          if((temp&k))
          b = b^(0x10000^0x11021);
        } 
    } 
    crc = b;  
//    crc =((crc<<1)|0x01);        //////////////end bit  
    
    
    for(i=0; i<16; i++) 
    {
      SD_CLK_LOW;
      
      if(crc&0x8000)
      SD_DAT_HIGH;
      else
      SD_DAT_LOW; 
      SD_CLK_HIGH;
      crc<<=1;
    }


    SD_CLK_LOW;
    SD_DAT_HIGH;
    SD_CLK_HIGH;
     

    Ncc();

    SD_DAT_IN;       
    while(1)            
    {
      SD_CLK_LOW;
      SD_CLK_HIGH;
      if((SD_TEST_DAT & 0x01) == 0)
      break;
    }
    
    for(i=0;i<8;i++)
    {
        SD_CLK_LOW;
        SD_CLK_HIGH;
        
        crc_status <<= 1;

        if((SD_TEST_DAT & 0x01))
        crc_status |= 0x01;        
    }

    
    for(i=0;i<16;i++)
    {
        SD_CLK_LOW;
        SD_CLK_HIGH;
    }
    write_status = 1;     
    return 0;     
}
//---------------

