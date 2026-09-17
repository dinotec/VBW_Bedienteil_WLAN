// #include <intrins.h>
#include <string.h>
#include "ConstDef.h"
// #include "Extern.h"
#include "extern_eeprom128.h"
xdata unsigned char zeit[8];

void iic_snd_ack_rtc(void)
{	
	SDA_RTC = 0;
	SCL_RTC = 1;
	iic_delay();
	SCL_RTC = 0;
	SDA_RTC = 1;
	iic_delay();
}

char iic_receive_rtc(void)
{	
	xdata unsigned char byte;		
	char n;
	for(n = 8, byte = 0; n > 0; n--)
	{ 	
		byte = byte << 1;
		SCL_RTC = 1;
		iic_delay();	
		if (SDA_RTC == 1)	byte |= 1;
		iic_delay();
		SCL_RTC = 0;
		iic_delay();
	}
	iic_delay();
	return (byte);
}


void iic_start_rtc(void)
{	
	SCL_RTC = 1;
	iic_delay();
	SDA_RTC = 0;
	iic_delay();
	SCL_RTC = 0;
	iic_delay();
}		 

void iic_send_rtc(char byte)
{	
	xdata char n;
	for (n = 8; n > 0; n--)
	{	
		if ((byte & 0x80) == 0) SDA_RTC = 0;
		else SDA_RTC = 1;
		SCL_RTC = 1;
		iic_delay();
		SCL_RTC = 0;
		byte = byte <<1;
		iic_delay();
	}		
	iic_delay();
}

bit iic_rec_ack_rtc(void)
{	
	bit b;
	SCL_RTC = 0;
	iic_delay();
	SDA_RTC = 1;
	iic_delay();
	SCL_RTC = 1;
	iic_delay();
	b = SDA_RTC;
	iic_delay();
	SCL_RTC = 0;
	iic_delay();
	return b;
}

// Acknowledge Bit senden
void iic_snd_no_ack_rtc(void)
{	
	SDA_RTC = 1;
	SCL_RTC = 1;
	iic_delay();
	SCL_RTC = 0;
	SDA_RTC = 1;
	iic_delay();
}

// Stopsequenz übertragen
// Das Ende einer Übertragung wird angezeigt und zugleich der Ruhezustand
// des Busses wiederhergestellt.
void iic_stop_rtc(void)
{	
	SDA_RTC = 0;
	iic_delay();
	SCL_RTC = 1;
	iic_delay(); 
	SDA_RTC = 1;	
	iic_delay();
}

unsigned char bcd2(unsigned char z)
{
	return ((z>>4)*10 + (z & 0x0f));
}

// 16 Byte aus PCF8563 lesen. 
void PCF8563_read(void)
{
	xdata unsigned char i,
						PCF8563[16];

	memset(PCF8563,0,sizeof(PCF8563[16]));
	
	while(1)
	{
		iic_start_rtc();
		iic_send_rtc(0xA2);		// Adresse PCF8563 im Schreibmodus
		//iic_send_rtc(0x52);		// Adresse PCF8563 im Schreibmodus
		if(iic_rec_ack_rtc()) 
			continue;
		iic_send_rtc(0); // ab Adresse 0 lesen
		if(!iic_rec_ack_rtc()) 
			break;
	}
	
	iic_start_rtc();
	iic_send_rtc(0xA3);		// Adresse PCF8563 im Lesemodus
	iic_rec_ack_rtc();
	for(i = 0; i < 15; i++)
	{
		PCF8563[i] = iic_receive_rtc();
		iic_snd_ack_rtc();
	}
	PCF8563[i] = iic_receive_rtc();
	iic_snd_no_ack_rtc();
	iic_stop_rtc();
	PCF8563[2] &= 0x7f; // sek
	PCF8563[3] &= 0x7f; // min
	PCF8563[4] &= 0x3f;// std
	PCF8563[5] &= 0x3f;// Tag
	PCF8563[6] &= 0x07;// Wtag
	PCF8563[7] &= 0x1f; // Monat
	//PCF8563[8]  ;                  // Jahr
	ES0 = 0;
	vek[sek].i   = bcd2(PCF8563[2]) % 60;	
	vek[min].i   = bcd2(PCF8563[3]) % 60;	
	vek[std].i   = bcd2(PCF8563[4]) % 24;	
	vek[tag].i   = bcd2(PCF8563[5]);	
	vek[monat].i = bcd2(PCF8563[7]);	
	vek[jahr].i  = bcd2(PCF8563[8]);	
	ES0 = 1;
}

// in PCF8563 schreiben. 
void PCF8563_write(unsigned char adr, unsigned char *ptr, unsigned char anz)
{
	xdata unsigned char i;
	
	while(1)
	{
		iic_start_rtc();
		iic_send_rtc(0xA2);		// Adresse PCF8563 im Schreibmodus
		if(!iic_rec_ack_rtc()) 
			break;
	}
	iic_send_rtc(adr); // ab Adr schreiben
	iic_rec_ack_rtc(); 
	
	for(i = 0; i < anz; i++)
	{
		iic_send_rtc(ptr[i]); // Daten
		iic_rec_ack_rtc(); 
	}
	iic_stop_rtc();
}

void PCF8563_init(void)
{
	// memset(PCF8563,0, sizeof(PCF8563));
	memset(zeit,0, sizeof(zeit));
	
	zeit[0] = 0x80;
	zeit[1] = 0x80;
	zeit[2] = 0x80;
	zeit[3] = 0x80;
	PCF8563_write(9,zeit,4);
	memset(zeit,0, sizeof(zeit));
}

unsigned char bcd(unsigned char c)
{
	xdata unsigned char a;
	c = c % 100;
	a = c / 10;
	a = a << 4;
	a = a | (c % 10);
	return (a);
}
