#include <string.h>
#include <intrins.h>
#include <ctype.h>
#include "ConstDef.h"                 // SFR declarations
#include "eeprom_drv.h"                 // SFR declarations
#include "Display.h"
#include "menu.h"

void disp_rt(unsigned char *ptr);
void FLASH_PageErase (unsigned int addr);
void FLASH_Write (unsigned int dest);
void Calc_CRC(unsigned char a);
void reset(void);

xdata RW_BUF buf;
xdata unsigned char txt_cnt;
xdata unsigned int crc;
xdata ULONG	len;
bit eepr_64k;


void WDT_RT(void)
{
	WD_TOOGLE = 1;
	WD_TOOGLE = 0;
}

unsigned int datum(unsigned char *ptr)
{
	unsigned int cnt;
	// Jahr und Monat vertauschen
	cnt = *(ptr + 1) - 0x30 + (*ptr - 0x30)*10;
	cnt += (*(ptr + 2) - 0x30) * 1000;
	cnt += (*(ptr + 3) - 0x30) * 100;
	return cnt;
}

void test_ergebnis(bit b)
{
	if(!b)
	{
		memcpy(rram.vec[txt_cnt] + 100, bmp_vec[ok_txt], bmp_len[ok_txt]);
	}
	else
	{
		memcpy(rram.vec[txt_cnt] + 100, bmp_vec[false_txt], bmp_len[false_txt]);
	}
	display_flush(0,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
	display_flush(1,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
}

void message(unsigned char txt, unsigned char upd)
{
	memcpy(rram.vec[++txt_cnt] + 10, bmp_vec[txt], bmp_len[txt]);
	
	if(upd == 1)
		memcpy(rram.vec[txt_cnt] + 15 + bmp_len[txt], bmp_vec[flash_txt], bmp_len[flash_txt]);
	else if(upd == 2)
		memcpy(rram.vec[txt_cnt] + 15 + bmp_len[txt], bmp_vec[menu_txt], bmp_len[menu_txt]);
	
	display_flush(0,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
	display_flush(1,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
}

void update(void)
{

	bit flash,
		eprm;
				
	xdata unsigned int cnt,
	                   y;
					   
	                   
	xdata unsigned char id[30],
						c_c51startup[3],
						wr,
						page_cnt;
						
    unsigned char code * xdata pread;           // FLASH read pointer
	
	WDT_RT();
	message(flash_txt,0);
	memset(id,0,sizeof(id));
	for(wr = 0, pread = 0; wr < 3; ++wr) c_c51startup[wr] = *pread++; 
	WDT_RT();
	for(y = 0, pread = OFF_ID; y < 30; y++) id[y] = *pread++; 
	WDT_RT();
	len.b[0] = 0;	
	len.b[1] = 0;
	len.b[2] = id[25];	
	len.b[3] = id[26];
	WDT_RT();
	len.ul += 2;
	if( len.ul >= MAX_LEN_FLASH)
		crc = 1;
	else
		for(cnt = 3, crc = 0, pread = 3; cnt < len.ul; ++cnt) 
		{
			Calc_CRC(*pread++);
			WDT_RT();
		}
	test_ergebnis((bit)crc);
	flash = (bit)crc;
	// ----------  Menutest -----------------	
	message(menu_txt,0);
	eeprom_test(0,0);
	test_ergebnis((bit)crc);
	// ----------  Menutestende -----------------	
	message(update_txt,1);
	adr_128k = 0;
	ptr_128k = buf.flash_buf;
	len_128k = 128;	
	wr = ee_rdseq_128k(1);
	eepr_64k = 0;
	if(wr == 0) // kein EEPROM
	{
		adr_128k = 0x10000; 
		wr = ee_rdseq_128k(1);
		if(wr==0) // kein EEPROM
		{	
			test_ergebnis(1);
			message(update_txt,2);
			test_ergebnis(1);
			if(flash) 
				reset();
			return;	
		}
		else // 64k EEPROM
		{
			eepr_64k = 1;
		}
	}
	// Update flash
	eprm = 1;
	if(!flash)
	{	
		if(!strncmp(buf.flash_buf + OFF_ID,id,21))
		{
			cnt = datum(id + 21);
			if(isdigit(buf.flash_buf[OFF_ID + 21]) && isdigit(buf.flash_buf[OFF_ID + 22])  \
				&& isdigit(buf.flash_buf[OFF_ID + 23]) && isdigit(buf.flash_buf[OFF_ID + 24]))
			{
				y = datum(buf.flash_buf + OFF_ID + 21);
			}	
			else // Version nicht erkennbar  
			{
				eprm = 0;
				test_ergebnis(1);
			}
			if(cnt >= y && !crc) // Im Flash gleiche oder höhere Version 
			{
				eprm = 0;
				test_ergebnis(1);
			}
		}
		else// Gerätebezeichnung stimmt nicht
		{
			eprm = 0;
			test_ergebnis(1);
		}
	}
	if(eprm)
	{
		eeprom_test(1,1);
		if(crc)
		{
			test_ergebnis(1);
		}
		else // gültig
		{
			test_ergebnis(0);
			message(program_txt,0);
			page_cnt = 0;
			len_128k = 128;
			for(adr_128k = 0,cnt = 0; cnt < len.ul; ) 
			{
				memset(&buf,0xff,sizeof(buf));
				for(page_cnt = 0; page_cnt < 4; ++page_cnt)
				{	
					ptr_128k = buf.eepr_r[page_cnt]; 
					ee_rdseq_128k(1);
					cnt += 128;
				}
				FLASH_PageErase(cnt-512);
				pause(3);
				if(cnt <= 512)
				{
					buf.flash_buf[0] = c_c51startup[0];
					buf.flash_buf[1] = c_c51startup[1];
					buf.flash_buf[2] = c_c51startup[2];
				}
				FLASH_Write(cnt - 512); 
				WDT_RT();
				ON_OFF_LED = ~ON_OFF_LED;
			}
			test_ergebnis(0);
			ON_OFF_LED = 1;
			return;
		}
	}
	// Menu update
	if(!eepr_64k) // 128k EEPROM erkannt
	{	
		message(update_txt,2);
		eeprom_test(1,0);
		if(crc)
		{
			test_ergebnis(1);
		}
		else // gültig
		{
			message(program_txt,0);
			page_cnt = 0;
			for(adr_128k = 0, cnt = 3; cnt < len.ul; ) 
			{
				len_128k = 128;
				memset(&buf,0,sizeof(buf));
				for(page_cnt = 0; page_cnt < 4; ++page_cnt)
				{	
					ptr_128k = buf.eepr_r[page_cnt]; 
					ee_rdseq_128k(1);
					cnt += 128;
				}
				len_128k = 256;
				adr_128k -= 512;
				ee_wrseq_128k();
				ee_wrseq_128k();
				WDT_RT();
			}
			test_ergebnis(0);
		}
	}
	if(flash) 
		reset();
}

void reset(void)
{
	message(reset_txt,0);
	pause(250);
	RSTSRC |= 0x10; // Softwarereset
}

void Calc_CRC(unsigned char a)
{
	xdata unsigned int l=0;
	xdata unsigned char n;
	bit b;
	
	l = a;
	l <<= 8;
	crc = crc ^ l;
	for(n = 0; n < 8; n++)
	{
		b = (bit)(crc & 0x8000);
		crc = crc << 1;
		if(b)
		{
			crc = crc ^ 0x1021;
		}
	}
}


void FLASH_PageErase (unsigned int addr)
{
	bit EA_SAVE = EA;                   // Preserve EA
	char xdata * xdata pwrite;           // FLASH write pointer

	EA = 0;                             // Disable interrupts

	VDM0CN = 0x80;                      // Enable VDD monitor
	RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

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

void FLASH_Write (unsigned int dest)//, char *src)
{
	xdata unsigned int 	i;
	for (i = 0; i < 512; ++i) 
	{
		FLASH_ByteWrite (dest++, buf.flash_buf[i]);
	}
}

