#include <string.h>
#include "ConstDef.h"
#include "eeprom_drv.h"
#include "Read_AT24C1024.h"

void crc_sum(unsigned char anf)
{
	unsigned int i;
	for(i = anf; i < len_128k; ++i)
		Calc_CRC(buf.flash_buf[i]);
}

// void eeprom_test(bit adr, bit mem) // 0 - interner EEPROM, 1 - udate; mem = 0: menu eeprom, 1: flash eeprom
void eeprom_test(unsigned char eepr) // 0 - udate_menu; 1 - interner EEPROM; 2 - flash eeprom
{
	unsigned long cnt;
	ptr_128k = buf.flash_buf;
	len_128k = 128;	
	crc = 0;
	if(eepr < 2)
	{	
		adr_128k = 1024 * eepr;
		if(!ee_rdseq_128k(eepr))
		{
			crc = 1;
			return;
		}
		len.b[0] = 0;
		len.b[1] = buf.flash_buf[0];
		len.b[2] = buf.flash_buf[1];
		len.b[3] = buf.flash_buf[2];
		len.ul += 2;
		if(len.ul > MAX_LEN_MENU)
		{
			crc = 1;
			return;
		}
		crc_sum(0);
	}
	else
	{
		adr_128k = 0;
		if(!ee_rdseq_128k(eepr))
		{
			crc = 1;
			return;
		}
		len.b[0] = len.b[1] = 0;
		len.b[2] = buf.flash_buf[OFF_ID + 25]; 
		len.b[3] = buf.flash_buf[OFF_ID + 26];
		len.ul += 2;	//len.i + 2 -->> wegen CRC
		if(len.ul > MAX_LEN_FLASH)
		{
			crc = 1;
			return;
		}
		crc_sum(3);
	}
	cnt=0;
	if(eepr==1)
		len.ul += 1024;
	while(adr_128k < len.ul)
	{	
		++cnt;
		memset(&buf,0,sizeof(buf));
		ee_rdseq_128k(eepr);
		crc_sum(0);
		if(adr_128k + 128 > len.ul)
			len_128k = len.ul - adr_128k;
	}
	len_128k=cnt;
}
