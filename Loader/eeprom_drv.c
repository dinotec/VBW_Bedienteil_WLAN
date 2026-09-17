#include <math.h>
#include <intrins.h>
#include <string.h>
#include "ConstDef.h"
#include "Read_AT24C1024.h"

#define ADR_L 0xa0 // menu EEPROM 
#define ADR_H 0xa2

#define ADR_L_UPD_MENU 0xa8 //	0xac 0xa8 
#define ADR_H_UPD_MENU 0xaa // 	0xae 0xaa	

#define ADR_UPD_FLASH 0xa6 //	0xac 0xa8 

// #define BUF_LEN 256


extern void WDT_RT(void);

xdata unsigned long adr_128k,
					adr_wr;
xdata unsigned char *ptr_128k;
xdata unsigned int len_128k;
// xdata UINT vek[status + 1];
// xdata unsigned char buf[BUF_LEN];

// Verzögerung für IIC Routinen
// Ausgelegt für 400kHz Bausteine !
void iic_delay(void)
{	
	xdata unsigned char n;
	for (n = 1; n > 0; n--) _nop_();
}

/* void WP_128k(bit b)
// {
	// (b) ? (P4 |= 0x80) : (P4 &= 0x7f);
// }
// void SCL_128k(bit b)
// {
	// (b) ? (P4 |= 0x20) : (P4 &= 0xdf);
// }
// void SDA_128k(bit b) // b = 0 Schreiben ist möglich
// {
	// (b) ? (P4 |= 0x40) : (P4 &= 0xbf);
// }
*/

void iic_start_128k(void)
{	
	SDA = 1;
	iic_delay();
	SCL = 1;
	iic_delay();
	SDA = 0;
	iic_delay();
	SCL = 0;
	iic_delay();
}		 

// Stopsequenz übertragen
// Das Ende einer Übertragung wird angezeigt und zugleich der Ruhezustand
// des Busses wiederhergestellt.
void iic_stop_128k(void)
{	
	SDA = 0;
	iic_delay();
	SCL = 1;
	iic_delay(); 
	SDA = 1;	
	iic_delay();
}

void iic_send_128k(char byte)
{	
	xdata char n;
	for (n = 8; n > 0; n--)
	{	
		// if ((byte & 0x80) == 0) SDA_128k(0);
		// else SDA_128k(1);
		SDA = (byte & 0x80);
		SCL = 1;
		iic_delay();
		SCL = 0;
		byte = byte << 1;
		iic_delay();
	}		
	iic_delay();
}

bit iic_rec_ack_128k(void)
{	
	bit b;
	SCL = 0;
	iic_delay();
	SDA = 1;
	iic_delay();
	SCL = 1;
	iic_delay();
	b = SDA;
	// (P4 & 0x40) ? (b = 1) : (b = 0);
	iic_delay();
	SCL = 0;
	iic_delay();
	return b;
}


// Ein Byte über IIC_Bus einlesen.
char iic_receive_128k(void)
{	
	xdata unsigned char byte = 0;		
	xdata char n;
	for(n = 8; n > 0; n--)
	{ 	
		byte <<= 1;
		SCL = 1;
		iic_delay();	
		if (SDA)	byte |= 1 ;
		iic_delay();
		SCL = 0;
		iic_delay();
	}
	iic_delay();
	return byte;
}

// Acknowledge Bit senden
void iic_snd_ack_128k(void)
{	
	SDA = 0;
	SCL = 1;
	iic_delay();
	SCL = 0;
	SDA = 1;
	iic_delay();
}

void iic_snd_no_ack_128k(void)
{	
	SDA = 1;
	SCL = 1;
	iic_delay();
	SCL = 0;
	SDA = 1;
	iic_delay();
}

// unsigned char ee_rdb_128k(bit page, unsigned int adr)
/*
unsigned char ee_rdb_128k(void)
{	
	// A1 = 0, wenn nicht angeschlossen
	// xdata unsigned char byte;
	xdata unsigned int tmp;
	
	// (page) ? (tmp = 0xa2) : (tmp = 0xa0);
	// (page) ? (tmp = ADR_H) : (tmp = ADR_L);
	(adr_128k >> 16) ? (tmp = ADR_H) : (tmp = ADR_L);
	do
	{	
		iic_start_128k();
		iic_send_128k(tmp);		// Adresse EEPROM im Schreibmodus
	} while(iic_rec_ack_128k());
	iic_send_128k(adr_128k >> 8);
	iic_rec_ack_128k();
	iic_send_128k(adr_128k);
	iic_rec_ack_128k();
	iic_start_128k();
	iic_send_128k(tmp | 1);		// Adresse EEPROM im Lesemodus
	iic_rec_ack_128k();
	// byte = iic_receive_128k();
	tmp = iic_receive_128k();
	iic_snd_no_ack_128k();
	iic_stop_128k();
	++adr_128k;
	return tmp;
}

// void ee_wrb_128k(bit page, unsigned int adr,unsigned char val)
void ee_wrb_128k(unsigned char val)
{	
	xdata unsigned char tmp;
	// WP_128k(0);
	WR_EN = 0;
	// (page) ? (tmp = 0xa2) : (tmp = 0xa0);
	// (page) ? (tmp = 0xa6) : (tmp = 0xa4);
	(adr_128k >> 16) ? (tmp = ADR_H) : (tmp = ADR_L);
	do
	{	
		iic_start_128k();
		iic_send_128k(tmp);
	} while(iic_rec_ack_128k());
	iic_send_128k(adr_128k >> 8);
	iic_rec_ack_128k();
	iic_send_128k(adr_128k);
	iic_rec_ack_128k();
	iic_send_128k(val);
	iic_rec_ack_128k();
	iic_stop_128k();
	// WP_128k(1);
	WR_EN = 1;
}		
*/
// void ee_wrseq_128k(unsigned long adr, unsigned int len, unsigned char *ptr)
void ee_wrseq_128k(void)
{	
	xdata unsigned char tmp;
	xdata unsigned int i;
	
	i   = 0;
	while(i < len_128k)
	{	
		(adr_wr >> 16) ? (tmp = ADR_H) : (tmp = ADR_L);
		WR_EN = 0;
		do
		{	
			iic_start_128k();
			iic_send_128k(tmp);
		} while(iic_rec_ack_128k());
		iic_send_128k(adr_wr >> 8);
		iic_rec_ack_128k();
		iic_send_128k(adr_wr);
		iic_rec_ack_128k();
		do
		{	
			tmp = ptr_128k[i];
			iic_send_128k(ptr_128k[i]);
			// iic_send_128k(buf.flash_buf[i]);
			iic_rec_ack_128k();
			++adr_wr;
			if(++i >= len_128k)
			{
				iic_stop_128k();
				break;
			}
			WDT_RT();
		} while(adr_wr % 0x100);
		// } while(adr_wr % 0x80);
		WR_EN = 1;
	}
	// WP_128k(1);
}		

// void ee_rdseq_128k(unsigned long adr, unsigned int len, unsigned char *ptr)
// bit ee_rdseq_128k(bit type)
bit ee_rdseq_128k(unsigned char type)  // 0 - udate_menu; 1 - interner EEPROM; 2 - flash eeprom
{	
	xdata unsigned char tmp;
	xdata unsigned int  i;

	i  = 0;
	{	
		// if(type) // update-EEPROM
		// {	
			// // (adr_128k >> 16 || eepr_64k) ? (tmp = ADR_H_UPD) : (tmp = ADR_L_UPD);
			// if(eepr_64k)
				// tmp = ADR_UPD_FLASH;
			// else
				// (adr_128k >> 16) ? (tmp = ADR_H_UPD_MENU) : (tmp = ADR_L_UPD_MENU);
		// }
		// else 	// interne EEPROM 
			// (adr_128k >> 16) ? (tmp = ADR_H) : (tmp = ADR_L);
		
		if(type == 0) // udate_menu;
		{
			(adr_128k >> 16) ? (tmp = ADR_H_UPD_MENU) : (tmp = ADR_L_UPD_MENU);
		}
		else if(type == 1) // interner EEPROM
		{
			(adr_128k >> 16) ? (tmp = ADR_H) : (tmp = ADR_L);
		}
		else // flash eeprom
		{
			tmp = ADR_UPD_FLASH;
		}
		do
		{	
			iic_start_128k();
			iic_send_128k(tmp);
		} while(iic_rec_ack_128k() && ++i < 0xff);
		if(i == 0xff)
			return 0;
		i = 0;
		iic_send_128k(adr_128k >> 8);
		iic_rec_ack_128k();
		iic_send_128k(adr_128k);
		iic_rec_ack_128k();
		iic_start_128k();
		iic_send_128k(tmp | 1);		// Adresse EEPROM im Lesemodus
		iic_rec_ack_128k();
		while(2)
		{	
			ptr_128k[i] = iic_receive_128k();
			++adr_128k;
			// if(++i >= len_128k || (adr_128k % 0x100 == 0))
			if(++i >= len_128k)// || (adr_128k % 0x100 == 0))
			{
				iic_snd_no_ack_128k();
				iic_stop_128k();
				break;
			}
			else
				iic_snd_ack_128k();
			WDT_RT();
		}//while(adr % 0x100);
	}
	return 1;
}
