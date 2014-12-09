#ifndef _HEAD_H_
#define _HEAD_H_

struct lyrics0
{
    int time[300];
    char text[300][32];
};
struct music0
{
  char m_name[50];
  unsigned int cluster;
};

struct lyric0
{
  char l_name[50];
  unsigned int cluster;
};

#include "basic_io.h"
#include "string.h"
#include <string.h>
#include <io.h>
#define DW 0X20 
#define BYTE    unsigned char
#define UINT16  unsigned int
#define UINT32  unsigned long

#define IOWR_LED_DATA(base, offset, data) \
  IOWR_16DIRECT(base, (offset) * 2, data) 
#define IORD_LED_DATA(base, offset) \
  IORD_16DIRECT(base, (offset) * 2)
#define IOWR_LED_SPEED(base, data) \
  IOWR_16DIRECT(base + 32, 0, data)

void Ncr(void);
void Ncc(void);

//-------------------------------------------------------------------------
void  LCD_Init();
void  LCD_Show_Text(char* Text);
void  LCD_Line2();
void  LCD_begin();
//-------------------------------------------------------------------------


BYTE SD_read_block(UINT32 block_number, BYTE *);
BYTE SD_write_block(UINT32 block_number, BYTE *);
void show_name(char *name);
void Ncr(void);
void Ncc(void);
BYTE response_R(BYTE);
BYTE send_cmd(BYTE *);
BYTE SD_read_lba(BYTE *,UINT32,UINT32);
BYTE SD_card_init(void);
void find_cluster(int m_num,int fat1_addr,unsigned int *cluster);
int read_lyrics(char *m_name,struct lyric0 *lyric,struct lyrics0 *lyrics,int num , int *clupsec_num, int *data_sect);
void time(int j,int n,int num);
void file_list(struct music0 *music,struct lyric0 *lyric , int *m_num , int *l_num , int *clupsec_num, int *data_sect,int *fat_addr);
#endif //_HEAD_H_
