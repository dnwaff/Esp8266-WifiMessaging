/*
* Oryginal by:
* Author : Aaron Lee
* Version: 1.00
* Date   : 2014.3.24
* Email  : hello14blog@gmail.com
* Modification: none
* Mod by reaper7
* found at http://bbs.espressif.com/viewtopic.php?f=15&t=31
*/
#include "math.h"
#include "driver/i2c.h"
#include "driver/i2c_oled.h"
#include "driver/i2c_oled_fonts.h"
#include "osapi.h"

#define MAX_SMALL_FONTS 21 //SMALL FONTS
#define MAX_BIG_FONTS 16 //BIG FONTS

static bool oledstat = false;

bool ICACHE_FLASH_ATTR
OLED_writeReg(uint8_t reg_addr,uint8_t val)
{
  i2c_start();
  i2c_writeByte(OLED_ADDRESS);
  if(!i2c_check_ack()) {
    i2c_stop();
    return 0;
  }
  i2c_writeByte(reg_addr);
  if(!i2c_check_ack()) {
    i2c_stop();
    return 0;
  }
  i2c_writeByte(val&0xff);
  if(!i2c_check_ack()) {
    i2c_stop();
    return 0;
  }
  i2c_stop();
  
  if (reg_addr==0x00)
    oledstat=true;
    
  return 1;
}

void ICACHE_FLASH_ATTR
OLED_writeCmd(unsigned char I2C_Command)
{
  OLED_writeReg(0x00,I2C_Command);
}

void ICACHE_FLASH_ATTR
OLED_writeDat(unsigned char I2C_Data)
{
	OLED_writeReg(0x40,I2C_Data);
}

bool ICACHE_FLASH_ATTR
OLED_Init(void)
{
	os_delay_us(60000);
	os_delay_us(40000);
	
  OLED_writeCmd(0xae); // turn off oled panel
  OLED_writeCmd(0x00); // set low column address 
  OLED_writeCmd(0x10); // set high column address 
  OLED_writeCmd(0x40); // set start line address 
  OLED_writeCmd(0x81); // set contrast control register 
  
  OLED_writeCmd(0xa0);
  OLED_writeCmd(0xc0);
  
  OLED_writeCmd(0xa6); // set normal display 
  OLED_writeCmd(0xa8); // set multiplex ratio(1 to 64) 
  OLED_writeCmd(0x3f); // 1/64 duty 
  OLED_writeCmd(0xd3); // set display offset 
  OLED_writeCmd(0x00); // not offset 
  OLED_writeCmd(0xd5); // set display clock divide ratio/oscillator frequency 
  OLED_writeCmd(0x80); // set divide ratio 
  OLED_writeCmd(0xd9); // set pre-charge period 
  OLED_writeCmd(0xf1); 
  OLED_writeCmd(0xda); // set com pins hardware configuration 
  OLED_writeCmd(0x12); 
  OLED_writeCmd(0xdb); // set vcomh 
  OLED_writeCmd(0x40); 
  OLED_writeCmd(0x8d); // set Charge Pump enable/disable 
  OLED_writeCmd(0x14); // set(0x10) disable
  OLED_writeCmd(0xaf); // turn on oled panel	
    
  OLED_Fill(0x00);  //OLED CLS
  
  return oledstat;
}

void ICACHE_FLASH_ATTR
start_horizontal_scroll(unsigned char direction, unsigned char start, unsigned char end, unsigned char interval)
{
    OLED_writeCmd(direction ? 0x27 : 0x26);
    OLED_writeCmd(0x00);
    OLED_writeCmd(start & 0x07);
    switch (interval) {
        case   2: OLED_writeCmd(0x07); break; // 111b
        case   3: OLED_writeCmd(0x04); break; // 100b
        case   4: OLED_writeCmd(0x05); break; // 101b
        case   5: OLED_writeCmd(0x00); break; // 000b
        case  25: OLED_writeCmd(0x06); break; // 110b
        case  64: OLED_writeCmd(0x01); break; // 001b
        case 128: OLED_writeCmd(0x02); break; // 010b
        //case 256: OLED_writeCmd(0x03); break; // 011b
        default:
            // default to 2 frame interval
            OLED_writeCmd(0x07); break;
    }
    OLED_writeCmd(end & 0x07);
    OLED_writeCmd(0x00);
    OLED_writeCmd(0xFF);

    // activate scroll
    OLED_writeCmd(0x2F);
}

void ICACHE_FLASH_ATTR
OLED_SetPos(unsigned char x, unsigned char y)
{ 
	OLED_writeCmd(0xb0+y);
	OLED_writeCmd(((x&0xf0)>>4)|0x10);
	OLED_writeCmd((x&0x0f)|0x01);
}

void ICACHE_FLASH_ATTR
OLED_Fill(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_writeCmd(0xb0+m);		//page0-page1
		OLED_writeCmd(0x00);		//low column start address
		OLED_writeCmd(0x10);		//high column start address
		for(n=0;n<132;n++)
			{
				OLED_writeDat(fill_Data);
			}
	}
}

void ICACHE_FLASH_ATTR
OLED_CLS(void)
{
	OLED_Fill(0x00);
}

void ICACHE_FLASH_ATTR
OLED_ON(void)
{
	OLED_writeCmd(0X8D);
	OLED_writeCmd(0X14);
	OLED_writeCmd(0XAF);
}

void ICACHE_FLASH_ATTR
OLED_OFF(void)
{
	OLED_writeCmd(0X8D);
	OLED_writeCmd(0X10);
	OLED_writeCmd(0XAE);
}

void
OLED_WriteChar(uint8 x , uint8 y , uint8 c ){
	c =  c - 32  ;
	int i;
	x = x * 6;

	if (x  > 126)
		x = 0;

	OLED_SetPos(x,y);

	for(i=0;i<6;i++)
		OLED_writeDat(F6x8[c][i]);

}

void ICACHE_FLASH_ATTR
OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					OLED_writeDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					OLED_writeDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					OLED_writeDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

void ICACHE_FLASH_ATTR
OLED_Print(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
  uint8_t step;
  if (TextSize==1)
    step = x*6; 
  else if (TextSize==2)
    step = x*8; 
  else 
    step = x;
  OLED_ShowStr(step,y,ch,TextSize);
}

void ICACHE_FLASH_ATTR
OLED_Line(unsigned char y, unsigned char ch[], unsigned char TextSize)
{
  uint8 step;
  uint8 len = strlen(ch);
  uint8 maxf = 0;
  
  y--;
  
  if (TextSize==1) { 
    maxf = MAX_SMALL_FONTS;
  }
  else if (TextSize==2) {
    y *= 2;
    maxf = MAX_BIG_FONTS;
  }

  if ((TextSize>=1) && (TextSize<=2)) {
    os_memset(ch+len+1,' ',maxf-len);
    os_memset(ch+maxf,'\0',1);

    OLED_ShowStr(0,y,ch,TextSize);
  }
}

void ICACHE_FLASH_ATTR
OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
  {
		y1 = y1/8;
  	  	y0 = y0/8;
  }
  else
  {
		y1 = y1/8 + 1;
		y0 = y0/8 + 1;
  }

	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x< x1 ;x++)
		{
			OLED_writeDat(BMP[j++]);// if (j > len) break;
		}
	}
}
