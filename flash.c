#include <string.h>
#include <intrins.h>
#include <ctype.h>
#include "c8051f340.h"                 // SFR declarations

void Wait_ms(unsigned int count);
void FLASH_PageErase (unsigned int addr);
void FLASH_Write (unsigned int dest, char *src, unsigned int numbytes);
unsigned long Calc_CRC(unsigned char a, unsigned long crc);

void update(void)
{
	typedef union UINT
	{
		unsigned int i;
		unsigned char b[2];
	} UINT;

	bit flash;
	xdata UINT 	len;// chs;
				
	xdata unsigned int cnt,
	                   y,
					   y0,
					   tmp;
					   
	                   
	xdata unsigned long crc;
	xdata unsigned char Scratch[512],
						buf[128],
						id[30],
						c_c51startup[3],
						wr;
						
    unsigned char code * xdata pread;           // FLASH read pointer
	
	WDT_RT();
	disp_rt("FLASH.");
	memset(id,0,sizeof(id));
	for(wr = 0, pread = 0; wr < 3; wr++) c_c51startup[wr] = *pread++; 
	WDT_RT();
	for(y = 0, pread = OFF_ID; y < 30; y++) id[y] = *pread++; 
	WDT_RT();
	len.b[0] = id[25];	
	len.b[1] = id[26];
	WDT_RT();
	len.i += 2;	
	for(cnt = 3, crc = 0, pread = 3; cnt < len.i; cnt++) 
	{
		crc = Calc_CRC(*pread++, crc);
		WDT_RT();
	}
	if(!crc) flash = 1, disp_rt("FLASH1");
	else	 flash = 0, disp_rt("FLASH0");
	
	disp_rt("EEPR..");
	wr = AT24C512_read(0, buf);
	if(wr == 0) // kein EEPROM
	{
		disp_rt("EEPR.0");
		if(crc) 
			reset();//RSTSRC |= 0x10;//  while(1);
		return;	
	}
	
	if(!strncmp(buf + OFF_ID,id,21))
	{
		// Jahr und Monat vertauschen
		cnt = id[22] - 0x30 + (id[21] - 0x30)*10;
		cnt += (id[23] - 0x30) * 1000;
		cnt += (id[24] - 0x30) * 100;
		//buf[OFF_ID + 25] = 0;
		if(isdigit(buf[OFF_ID + 21]) && isdigit(buf[OFF_ID + 22])  \
			&& isdigit(buf[OFF_ID + 23]) && isdigit(buf[OFF_ID + 24]))
		{
			// Jahr und Monat vertauschen
			y0 = OFF_ID + 21;
			y = buf[y0+1] - 0x30 + (buf[y0] - 0x30)*10;
			y += (buf[y0 + 2] - 0x30) * 1000;
			y += (buf[y0 + 3] - 0x30) * 100;
		}	
		else 
		{
			if(!crc) return;
			else reset();//RSTSRC |= 0x10; // Softwarereset
		}
		if(cnt >= y && !crc) 
		{
			disp_rt("EEPR.2"); // Im Flash gleiche oder höhere Version
			return;
		}
	}
	else
	{
		disp_rt("EEPR.1"); // Gerätebezeichnung stimmt nicht
		if(!crc) return;
		else reset();
	}
	
	len.b[0] = buf[OFF_ID + 25]; 
	len.b[1] = buf[OFF_ID + 26];
	len.i += 2;	//len.i + 2 -->> wegen CRC
	if(len.i >= 60288 && !crc) return;
	
	wr = 0;
	while(1)
	{
		if(wr == 0x55) disp_rt("PROGR.");
		
		for(cnt = 0, crc = 0, y = 0, y0 = 0; cnt < len.i; cnt++) 
		{
			tmp = cnt % 128; 
			if(tmp == 0)
			{
				if(AT24C512_read(y, buf) == 0) 
				{
					//send_str("Kann die Karte nicht lesen...");
					return;	
				}
				if(!y0) memset(Scratch,0,sizeof(Scratch));
				memcpy(Scratch + (y0 * 128),buf,128);
				y++;
				y0++;
				if((wr == 0x55) && (y0 == 4))
				{
					//send_str("Flash schreiben...");
					FLASH_PageErase(cnt);
					Wait_ms(50);
					if(cnt == 384)
					{
						Scratch[0] = c_c51startup[0];
						Scratch[1] = c_c51startup[1];
						Scratch[2] = c_c51startup[2];
					}
					FLASH_Write(cnt - 384, Scratch, 512); 
					//send_str("Flash ist beschrieben.");
				}
				if(y0 >= 4) y0 = 0;

			    WDT_RT();
			}
			//Erste 3 Bytes - keine CRC-Bildung
			if(cnt > 2)
				crc = Calc_CRC(buf[tmp], crc);
		}
		if(wr && y0) 
		{
			FLASH_PageErase(cnt);
			Wait_ms(50);
			y = cnt / 512;
			y *= 512;
			FLASH_Write(y, Scratch, 512);
		}
		if(crc || wr) break;
		wr = 0x55;
		EA = 0;
	}
	if(crc > 0) 
	{
		disp_rt("EEPR.0");
		if(!flash) reset();
	}
}

void reset(void)
{
	disp_rt("RESET.");
	RSTSRC |= 0x10; // Softwarereset
}

unsigned long Calc_CRC(unsigned char a, unsigned long crc)
{
	xdata unsigned long l=0;
	xdata unsigned char n;
	//l = a * 256;
	l = a;
	l <<= 8;
	crc = crc ^ l;
	for(n = 0; n < 8; n++)
	{
		//crc = crc * 2;
		crc = crc << 1;
		if(crc & 65536)
		{
			crc = crc & 65535;
			crc = crc ^ 0x1021;
		}
		//crc = crc & 65535;	
	}
	return crc;
}



void FLASH_PageErase (unsigned int addr)
{
	bit EA_SAVE = EA;                   // Preserve EA
	char xdata * xdata pwrite;           // FLASH write pointer

	EA = 0;                             // Disable interrupts

	VDM0CN = 0x80;                      // Enable VDD monitor
	RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
	
	addr = 0xc800;
	pwrite = (char xdata *) addr;

	FLKEY  = 0xA5;                      // Key Sequence 1
	FLKEY  = 0xF1;                      // Key Sequence 2
	PSCTL |= 0x03;                      // PSWE = 1
  
	VDM0CN = 0x80;                      // Enable VDD monitor and high threshold
	RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
	*pwrite = 0;                        // Initiate page erase

	PSCTL &= ~0x03;                     // PSWE = 0; PSEE = 0

	EA = EA_SAVE;                       // Restore interrupts
}


void FLASH_ByteWrite (unsigned int addr, unsigned char byte)
{
	bit EA_SAVE = EA;                   // Preserve EA
	char xdata * xdata pwrite;           // FLASH write pointer

	EA = 0;                             // Disable interrupts
	VDM0CN = 0x80;                      // Enable VDD monitor and high threshold
	RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
	pwrite = (char xdata *) addr;
	FLKEY  = 0xA5;                      // Key Sequence 1
	FLKEY  = 0xF1;                      // Key Sequence 2
	PSCTL |= 0x01;                      // PSWE = 1 which enables writes
	VDM0CN = 0x80;                      // Enable VDD monitor and high threshold
	RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
	*pwrite = byte;                     // Write the byte
	PSCTL &= ~0x01;                     // PSWE = 0 which disable writes
	EA = EA_SAVE;                       // Restore interrupts
}

void FLASH_Write (unsigned int dest, char *src, unsigned int numbytes)
{
	xdata unsigned int 	i,
						cntr;

	cntr = dest + numbytes;
	for (i = dest; i < cntr; i++) 
	{
		FLASH_ByteWrite (i, *src++);
	}
}


void Wait_ms(unsigned int count)
{
	while(count--)
	{
		TR2=1;
		TF2H=0;

		while(!TF2H);
		TF2H=0;
		WD_TOOGLE = 1;
		WD_TOOGLE = 0;
	}
}


