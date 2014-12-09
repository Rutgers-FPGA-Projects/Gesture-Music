#include "head.h"
#include "system.h"
#define lcd_write_command(base, data)                     IOWR(base, 0, data)
#define lcd_read_command(base)                            IORD(base, 1)
#define lcd_write_data(base, data)                    IOWR(base, 2, data)
#define lcd_read_data(base)                           IORD(base, 3) 


int main(void)
{
  
  //LCD_begin();

  UINT16 i=0,temporary1,temporary2,song_num=0,song_position=0,k,key,key2,key3,key4,sw[17],sw0,sw1,clupsec_num,data_sector;
  
  
  UINT16 back[2]={0},num_lyric,num_music,num_lyrics,time_base,j_base,base,fat_addr,cluster_num;

  UINT32 j;
  unsigned int cl[3000];
  BYTE Buffer[512]={0};
  
  while(SD_card_init())  // wait for the card to be inserted
    usleep(500000);
  LCD_begin();
  
  struct music0 music[30];
  struct lyric0 lyric[30];
  struct lyrics0 lyrics;
  
  char title[2][18]={" Press KEY1 to   ",
                      " select a song   "}; 
  char text[2][16]={"songs",
                    "lyrics"};
  char wait[16]="loading";
  
  file_list(music,lyric,&num_music,&num_lyric,&clupsec_num, &data_sector,&fat_addr); // search and update the database from the SD card
  sw1=IORD(SW_BASE,0);
  back[0]=sw1&0x20000;
  
  while(1)
  { 
    
    IOWR(SEG7_BASE,0,0);
    sw0=IORD(SW_BASE,0);
    sw0=sw0&0x07; 
    sw[0]=sw0&0x01;
    sw[1]=sw0&0x02;   
    sw[2]=sw0&0x04; 
    

    if(!sw0)
    {
      LCD_Init();   
      LCD_Show_Text(title[0]);
      lcd_write_command(LCD_BASE,0xC0);
      LCD_Show_Text(title[1]);
      while(1)
      {
        key=IORD(KEY_BASE,0);      // wait till key2 is pressed
        key2=~key&0x02;
        if(key2)
          break;
      }
      LCD_Init();
      lcd_write_command(LCD_BASE,0x80);
      usleep(100);  
      lcd_write_data(LCD_BASE,(char)(num_music/10+0x30));   // display the number of wav files
      usleep(100);  
      lcd_write_data(LCD_BASE,(char)(num_music%10+0x30));
      usleep(100);
      for(i=0;i<5;i++)
      {
        lcd_write_data(LCD_BASE,text[0][i]);
        usleep(100);
      }
      lcd_write_command(LCD_BASE,0xc0);
      usleep(100);  
      lcd_write_data(LCD_BASE,(char)(num_lyric/10+0x30));  // display the number of lysic files
      usleep(100);  
      lcd_write_data(LCD_BASE,(char)(num_lyric%10+0x30));
      usleep(100);
      for(i=0;i<6;i++)
      {
        lcd_write_data(LCD_BASE,text[1][i]);
         usleep(100);
      }
     usleep(1000000);
     
     while(1)
     {
        show_name(music[song_num].m_name);
        key=IORD(KEY_BASE,0);
        key=~key&0x0e; 
        if(key)
        {   
           key2=key&0x02;
           key3=key&0x04;
           key4=key&0x08;
           if(key2)
             break;
           else if(key3)
           {  
             song_num=(song_num+1)%num_music;           // next song
           }
           else if(key4)
           {
             song_num=(song_num+num_music-1)%num_music;  // previous song
           }
           else
           ;
        }
         time(0,song_num+1,num_music);    
     }
    }
    
    else if(sw[0])       {
      song_num=(song_num+1)%num_music;      // continuous 
      time(0,song_num+1,num_music); 
    }
    else if(sw[1])          {
      song_num+=0;
      time(0,song_num+1,num_music);        // repeat the song
    }
    else if(sw[2])
    {
      song_num=(num_lyrics+temporary1+temporary2+music[song_num].m_name[0])%num_music;  // random play
      time(0,song_num+1,num_music);   
    }    
    LCD_Init();    
    LCD_Show_Text(wait);
    num_lyrics=read_lyrics(music[song_num].m_name,lyric,&lyrics,num_lyric,&clupsec_num, &data_sector); //lyrics
    cl[0]=music[song_num].cluster;
    find_cluster(song_num,fat_addr,cl);
    for(i=0;cl[i]>1;i++);
    cluster_num=i;  
    
    show_name(music[song_num].m_name);   // show the name of the song
    for(song_position=0;song_position<cluster_num;song_position++)
    {  
      base=(cl[song_position]-2)*clupsec_num+data_sector;
      j_base=song_position*clupsec_num-base;
      sw1=IORD(SW_BASE,0);
      sw[4]=sw1&0x10;
      sw[5]=sw1&0x20; 
      sw[6]=sw1&0x40;
      
      back[1]=sw1&0x20000;
      if(back[1]!=back[0])
      {
        back[0]=back[1];
        break;
      } 
      if(sw[4])
      {
         song_position=(song_position+4)%cluster_num;
         time(song_position*clupsec_num,song_num+1,num_music);    // fast forward
         usleep(20000);
         continue;        
      }
      else if(sw[6])
      {
         song_position=(song_position+cluster_num-4)%cluster_num;
         time(song_position*clupsec_num,song_num+1,num_music);   // pause
         usleep(20000);
         continue;
      }    
      else if(sw[5])
      {
         song_position=(song_position+cluster_num-1)%cluster_num;  // reverse
         usleep(20000);
         continue;
      }   
      
      for(j=base;j<base+clupsec_num;j++) //j is the section in the cluster
      {      
        SD_read_lba(Buffer,j,1);    // read function from SD card controller.c
        i=0;
        while(i<512)
        {
          if(!IORD(AUDIO_BASE,0))
          {
            temporary1=((Buffer[i+1]<<8)|Buffer[i]);   // read from SD card
           
            IOWR(AUDIO_BASE,0,temporary1);
            
            i+=2;            
          }
        }

        if(j%32==0)
        {
            unsigned int x=0;
            unsigned int y=0xffff;
            unsigned int z = temporary1;
            do
            {
                x++;
                z=z<<1;
            }while((z & 0x8000)==0);
            y=y>>x;
            IOWR(LEDR_BASE,0,y);
            IOWR(LEDG_BASE,0,z);
        }
/********************************************************************/

        time(j+j_base,song_num+1,num_music); //display time

        for(k=0;k<num_lyrics;k++)            //display lyric
        {       
          time_base=j_base-lyrics.time[k]-2;       
          if(j+j_base==lyrics.time[k])   
          {    
            lcd_write_command(LCD_BASE,0x80);
            break;
          }
          else if(j+j_base==lyrics.time[k]+36)
          {
            lcd_write_command(LCD_BASE,0xc0);
            break;          
          }
          
          else if(j%2==0&&(j+j_base>=lyrics.time[k]+2)&&(j+j_base<=lyrics.time[k]+34))
          {
              lcd_write_data(LCD_BASE,lyrics.text[k][(j+time_base)/2]);
              break;             
          }          
          else if(j%2==0&&(j+j_base>=lyrics.time[k]+38)&&(j+j_base<lyrics.time[k]+70))
          {
              lcd_write_data(LCD_BASE,lyrics.text[k][(j+time_base)/2-2]);
              break;            
          }          
        }             
    }   
   }
  }
  return 0;    
}
