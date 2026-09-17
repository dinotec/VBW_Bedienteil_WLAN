#include <intrins.h>
#include <string.h>
#include <stdlib.h>
#include "ConstDef.h"
#include "eeprom_128drv.h"
#include "eeprom128.h"
#include "main.h"

#define SDA_RTC_0 P4 &= ~0x02
#define SDA_RTC_1 P4 |=  0x02
#define SCL_RTC_0 P4 &= ~0x01
#define SCL_RTC_1 P4 |=  0x01
// P4.0 scl P4.1 - SDA


void iic_snd_ack_rtc(void) {	
	SDA_RTC_0;
	SCL_RTC_1;
	iic_delay();
	SCL_RTC_0;
	SDA_RTC_1;
	iic_delay();
}

char iic_receive_rtc(void) {	
	xdata unsigned char byte,i;		
	//~ char n;
	//~ for(n = 8, byte = 0; n > 0; --n)
	for(i = 8, byte = 0; i > 0; --i) { 	
		byte = byte << 1;
		//SCL_RTC = 1;
		SCL_RTC_1;
		iic_delay();	
		//if (SDA_RTC == 1)	byte |= 1;
		byte |= ((P4 & 0x02) > 0);
		iic_delay();
		// SCL_RTC = 0;
		SCL_RTC_0;
		iic_delay();
	}
	iic_delay();
	return (byte);
}


void iic_start_rtc(void) {	
	//SCL_RTC = 1;
	SCL_RTC_1;
	iic_delay();
	//SDA_RTC = 0;
	SDA_RTC_0;
	iic_delay();
	//SCL_RTC = 0;
	SCL_RTC_0;
	iic_delay();
}		 

void iic_send_rtc(char byte) {	
	xdata char i;
	for (i = 8; i > 0; --i) {	
		if ((byte & 0x80) == 0) SDA_RTC_0;
		else SDA_RTC_1;
		SCL_RTC_1;
		iic_delay();
		SCL_RTC_0;
		byte = byte << 1;
		iic_delay();
	}		
	iic_delay();
}

bit iic_rec_ack_rtc(void) {	
	bit b;
	//SCL_RTC = 0;
	SCL_RTC_0;
	iic_delay();
	//SDA_RTC = 1;
	SDA_RTC_1;
	iic_delay();
	//SCL_RTC = 1;
	SCL_RTC_1;
	iic_delay();
	//b = SDA_RTC;
	b = ((P4 & 0x02) > 0);
	iic_delay();
	//SCL_RTC = 0;
	SCL_RTC_0;
	iic_delay();
	return b;
}

// Acknowledge Bit senden
void iic_snd_no_ack_rtc(void) {	
	//SDA_RTC = 1;
	SDA_RTC_1;
	//SCL_RTC = 1;
	SCL_RTC_1;
	iic_delay();
	//SCL_RTC = 0;
	SCL_RTC_0;
	//SDA_RTC = 1;
	SDA_RTC_1;
	iic_delay();
}

// Stopsequenz übertragen
// Das Ende einer Übertragung wird angezeigt und zugleich der Ruhezustand
// des Busses wiederhergestellt.
void iic_stop_rtc(void) {	
	//SDA_RTC = 0;
	SDA_RTC_0;
	iic_delay();
	//SCL_RTC = 1;
	SCL_RTC_1;
	iic_delay(); 
	//SDA_RTC = 1;	
	SDA_RTC_1;	
	iic_delay();
}

unsigned char bcd2(unsigned char z) {
	return ((z >> 4)*10 + (z & 0x0f));
}

// 16 Byte aus PCF8563 lesen. 
/* void PCF8563_read(void) {
	xdata unsigned char i; //PCF8563[16];
	unsigned char xdata * xdata PCF8563;
	PCF8563 = calloc(16,1);           

	//~ memset(PCF8563,0,sizeof(PCF8563));
	
	while(1) {
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
	for(i = 0; i < 15; i++) {
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
	//~ vek[sec].ui     = bcd2(PCF8563[2]) % 60;	
	vek[min_1].ui   = bcd2(PCF8563[3]) % 60;	
	vek[std_1].ui   = bcd2(PCF8563[4]) % 24;	
	vek[day].ui     = bcd2(PCF8563[5]);	
	vek[mon].ui     = bcd2(PCF8563[7]);	
	vek[year].ui    = bcd2(PCF8563[8]) + 2000;	
	ES0 = 1;
}*/
// 16 Byte aus PCF8563 lesen. 
void PCF8563_read(void) {
	xdata unsigned char i; //PCF8563[16];
	unsigned char xdata * xdata PCF8563;
	PCF8563 = calloc(16,1);           

	//~ memset(PCF8563,0,sizeof(PCF8563));
	
	while(1) {
		iic_start_rtc();
		iic_send_rtc(0xA2);		// Adresse PCF8563 im Schreibmodus
		if(iic_rec_ack_rtc()) 
			continue;
		iic_send_rtc(0); // ab Adresse 0 lesen
		if(!iic_rec_ack_rtc()) 
			break;
	}
	iic_start_rtc();
	iic_send_rtc(0xA3);		// Adresse PCF8563 im Lesemodus
	iic_rec_ack_rtc();
	for(i = 0; i < 15; i++) {
		*(PCF8563+i) = iic_receive_rtc();
		iic_snd_ack_rtc();
	}
	*(PCF8563+i) = iic_receive_rtc();
	iic_snd_no_ack_rtc();
	iic_stop_rtc();
	*(PCF8563+2) &= 0x7f; // sek
	*(PCF8563+3) &= 0x7f; // min
	*(PCF8563+4) &= 0x3f;// std
	*(PCF8563+5) &= 0x3f;// Tag
	*(PCF8563+6) &= 0x07;// Wtag
	*(PCF8563+7) &= 0x1f; // Monat
	//PCF8563[8]  ;                  // Jahr
	ES0 = 0;
	//~ vek[sec].ui     = bcd2(PCF8563[2]) % 60;	
	vek[min_int].ui   = bcd2(*(PCF8563+3)) % 60;
	vek[std_int].ui   = bcd2(*(PCF8563+4)) % 24;
	vek[day_int].ui     = bcd2(*(PCF8563+5));
	vek[mon_int].ui     = bcd2(*(PCF8563+7));
	vek[year_int].ui    = bcd2(*(PCF8563+8)) + 2000;
	free(PCF8563);
	ES0 = 1;
}

// in PCF8563 schreiben. 
void PCF8563_write(unsigned char adr, unsigned char *ptr, unsigned char anz) {
	xdata char i;
	while(1) {
		iic_start_rtc();
		iic_send_rtc(0xA2);		// Adresse PCF8563 im Schreibmodus
		if(!iic_rec_ack_rtc()) 
			break;
	}
	iic_send_rtc(adr); // ab Adr schreiben
	iic_rec_ack_rtc(); 
	
	for(i = 0; i < anz; i++) {
		iic_send_rtc(ptr[i]); // Daten
		iic_rec_ack_rtc(); 
	}
	iic_stop_rtc();
}

unsigned char bcd(unsigned char c) {
	xdata unsigned char a;
	c = c % 100;
	a = c / 10;
	a = a << 4;
	a = a | (c % 10);
	return (a);
}

void new_time(void) {
	//~ xdata unsigned char zeit[8];
	unsigned char xdata * xdata zeit;
	zeit = calloc(16,1);           
	
	*(zeit+0) = bcd(0); 		// sek
	*(zeit+1) = bcd(vek[min_int].ui); 	// min
	*(zeit+2) = bcd(vek[std_int].ui); 		//std
	*(zeit+3) = bcd(vek[day_int].ui);	//Tag
	*(zeit+4) = bcd(1); //WTag
	*(zeit+5) = bcd(vek[mon_int].ui);//Monat
	*(zeit+6) = bcd(vek[year_int].ui %100); //Jahr
	//zeit[7] = 0x55;
	PCF8563_write(2,zeit,7);
	free(zeit);
}
