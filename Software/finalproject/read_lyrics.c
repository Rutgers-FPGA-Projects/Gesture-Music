#include "head.h"

int read_lyrics(char *m_name,struct lyric0 *lyric,struct lyrics0 *lyrics,int num , int *clupsec_num, int *data_sector)

{
  UINT16 i,numb1=0,timetemp[6],base,a,b,c,d,j,e,temp_time[100],temp0;
  UINT32 numb0=0;
  BYTE buffer[1024]={0};
  char temp[100][97],tempp[97];
  
  strlwr(m_name); // lower case the song name
  for(i=0;i<num;i++) //loop to search
  {
     strlwr(lyric[i].l_name);  //low case the lyric file name
     if(!strcmp(m_name,lyric[i].l_name))  //break if matching lyric is found
     {
        break;
     }
  } 
  if(i==num)//cannot find lyric file for this song, return
     return numb0; 
  else
  {
    base=(lyric[i].cluster-2)**clupsec_num+*data_sector;  //base address
   
    j=base;   
    SD_read_lba(buffer,j,1);//read
    SD_read_lba(buffer+512,j+1,1); //read
    a=0;
    
    while(j<base+*clupsec_num)    //read lyrics
    {
      if(buffer[a]==0x00)       //'\0' is the ending char
        break;      
      else                      //if no repeat lines in lyric
      {
        if(buffer[a]=='['&&buffer[a+1]<='9'&&buffer[a+1]>='0'&&buffer[a+10]!='[')
        {
          a++;
          if(a>=512)  //read next cluster when finish the current one
          {
                j++;
                SD_read_lba(buffer,j,1); 
                SD_read_lba(buffer+512,j+1,1); 
                a=0;
          }
          c=0;
          while(buffer[a]!=']')     
          {
            if(buffer[a]<='9'&&buffer[a]>='0')
            {
              timetemp[c]=buffer[a];
              c++;
            }
            a++;             
            if(a>=512)              //read next cluster when finish the current one
            {
                j++;
                SD_read_lba(buffer,j,1); 
                SD_read_lba(buffer+512,j+1,1); 
                a=0;
            }                         
          }
          a++;
          if(a>=512)                //read next cluster when finish the current one
          {
                j++;
                SD_read_lba(buffer,j,1);
                SD_read_lba(buffer+512,j+1,1); 
                a=0;
          }
          temp_time[numb0]=(((timetemp[0]-0x30)*10+
                              (timetemp[1]-0x30))*6000+
                              (timetemp[2]-0x30)*1000+
                              (timetemp[3]-0x30)*100+
                              (timetemp[5]-0x30)*10+
                              (timetemp[5]-0x30))*375/100;
                
          for(c=0;c<96;)            //read long lyric
          {
            if(buffer[a]!=0x0d&&buffer[a+1]!=0x0a)
            {
              if((buffer[a]>=0x21&&buffer[a]<=0x7e)||buffer[a]==' ')
                temp[numb0][c++]=buffer[a];
              a++;            
              if(a>=512)            //read next cluster when finish the current one
              {
               j++;
               SD_read_lba(buffer,j,1);
               SD_read_lba(buffer+512,j+1,1); 
               a=0;
              } 
            } 
            else
              break;                       
          }
          temp[numb0][96]=c/32+1;  //how many 32 bits
          for(;c<96;c++)
              temp[numb0][c]=0x20;  //fill in if not a integer multiper of 32
          numb0++;
        }                           //finish reading this line
        else if(buffer[a]=='['&&buffer[a+1]<='9'&&buffer[a+1]>='0'&&buffer[a+10]=='[')  //if there's repeat lines
        {
          for(b=1;buffer[a+b*10]=='[';b++);  //how many timinginformation for this repeading line
          for(c=0;c<b;c++)                  //read all information regarding to this line
          {
             e=0;
             for(d=0;d<10;d++)
             {
               if(buffer[a]<='9'&&buffer[a]>='0')  
               {
                 timetemp[e]=buffer[a];
                 e++;
               }
               a++;             
               if(a>=512)                   //read next cluster when finish the current one
               {
                 j++;
                 SD_read_lba(buffer,j,1); 
                 SD_read_lba(buffer+512,j+1,1); 
                 a=0;
               }   
             }
             temp_time[numb0]=(((timetemp[0]-0x30)*10+
                              (timetemp[1]-0x30))*6000+
                              (timetemp[2]-0x30)*1000+
                              (timetemp[3]-0x30)*100+
                              (timetemp[5]-0x30)*10+
                              (timetemp[5]-0x30))*375/100;  //convert timing to cluster number
             numb0++;
              
          }
          for(c=0;c<96;) //read long lyric
          {
            if(buffer[a]!=0x0d&&buffer[a+1]!=0x0a)
            {
              if((buffer[a]>=0x21&&buffer[a]<=0x7e)||buffer[a]==' ')
              {
                for(d=0;d<b;d++)
                {
                  temp[numb0-b+d][c]=buffer[a];
                }
                c++;
              }
              a++;            
              if(a>=512)
              {
               j++;
               SD_read_lba(buffer,j,1);
               SD_read_lba(buffer+512,j+1,1);
               a=0;
              } 
            } 
            else
              break;                       
          }          
          for(d=0;d<b;d++)
          {
            temp[numb0-b+d][96]=c/32+1;
          }
          for(;c<96;)
          {
              for(d=0;d<b;d++)
              {
                temp[numb0-b+d][c]=0x20;
              } 
              c++;     
          }         
        }
        else
        {
          a++;
          if(a>=512)
          {
               j++;
               SD_read_lba(buffer,j,1); 
               SD_read_lba(buffer+512,j+1,1); 
               a=0;
          } 
        }
        
       } 
    }
    for(i=0;i<numb0-1;i++)   //sorting lyric according to timing info
    
    {
      for(j=i+1;j<numb0;j++)
      {
        if(temp_time[i]>temp_time[j])
        {
          temp0=temp_time[i];
          temp_time[i]=temp_time[j];
          temp_time[j]=temp0;
          for(c=0;c<97;c++)
          {
            tempp[c]=temp[i][c];
            temp[i][c]=temp[j][c];
            temp[j][c]=tempp[c];
          }
        }
      }
    }
    
    for(j=0;j<numb0;j++)
    { 
      switch(temp[j][96])    //convert long lyric to short ones for display
      {
            case 1:lyrics->time[numb1]=temp_time[j];
                   for(i=0;i<32;i++)
                    lyrics->text[numb1][i]=temp[j][i];
                   numb1++;
                   break;
            case 2:lyrics->time[numb1]=temp_time[j];
                   lyrics->time[numb1+1]=(temp_time[j]+temp_time[j+1])/2;
                   for(i=0;i<64;i++)
                     lyrics->text[numb1+i/32][i%32]=temp[j][i];
                   numb1+=2;
                   break;
            case 3:lyrics->time[numb1]=temp_time[j];
                   lyrics->time[numb1+1]=(lyrics->time[j+1]+lyrics->time[j]*2)/3; 
                   lyrics->time[numb1+2]=(lyrics->time[j+1]*2+lyrics->time[j])/3;
                   for(i=0;i<96;i++)
                     lyrics->text[numb1+i/32][i%32]=temp[j][i];
                   numb1+=3;
                   break;
           default:break;
      }  
    }  
    return numb1;
  }
}//int
//-------------------------------------------------------------------------
