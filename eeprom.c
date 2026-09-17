#include "c8051f340.h"                 // SFR declarations

#define SYSCLK       11059200       // Oszillatorfrequenz in Hz

sbit SDA = P3^7;		// IIC-Bus	

// Startsequenz übertragen
// Die IIC Leitungen sind im Ruhezustand beide auf high-Pegel. Um den Beginn
// einer Übertragung anzuzeigen zieht der Master zuerst die Datenleitung
// dann die Taktleitung nach GND.

// Verzögerung für IIC Routinen
// Ausgelegt für 400kHz Bausteine !
void iic_delay(void)
{	
	unsigned char n;
	for (n = (SYSCLK / 1000000); n > 0; n--) ;
}

void SCL(bit b)
{
	if(b) P4 |= 0x01;
	else  P4 &= 0xfe;
}


void iic_start(void)
{	
	SCL(1);
	iic_delay();
	SDA = 0;
	iic_delay();
	//SCL = 0;
	SCL(0);
	iic_delay();
}		 

// Stopsequenz übertragen
// Das Ende einer Übertragung wird angezeigt und zugleich der Ruhezustand
// des Busses wiederhergestellt.
void iic_stop(void)
{	
	SDA = 0;
	iic_delay();
	//SCL = 1;
	SCL(1);
	iic_delay(); 
	SDA = 1;	
	iic_delay();
}

void iic_send(char byte)
{	
	char n;
	for (n = 8; n > 0; n--)
	{	
		if ((byte & 0x80) == 0) SDA = 0;
		else SDA = 1;
		//SCL = 1;
		SCL(1);
		iic_delay();
		//SCL = 0;
		SCL(0);
		byte = byte <<1;
		iic_delay();
	}		
	iic_delay();
}

bit iic_rec_ack(void)
{	
	bit b;
	//SCL = 0;
	SCL(0);
	iic_delay();
	SDA = 1;
	iic_delay();
	//SCL = 1;
	SCL(1);
	iic_delay();
	b = SDA;
	iic_delay();
	//SCL = 0;
	SCL(0);
	iic_delay();
	return b;
}


// Ein Byte über IIC_Bus einlesen.
char iic_receive(void)
{	
	unsigned char byte = 0;		
	char n;
	for(n = 8; n > 0; n--)
	{ 	
		byte <<= 1;
		//SCL = 1;
		SCL(1);
		iic_delay();	
		if (SDA == 1)	byte |= 1 ;//bitmaski(n - 1);
		iic_delay();
		//SCL = 0;
		SCL(0);
		iic_delay();
	}
	iic_delay();
	return (byte);
}

// Acknowledge Bit senden
void iic_snd_ack(void)
{	
	SDA = 0;
	//SCL = 1;
	SCL(1);
	iic_delay();
	//SCL = 0;
	SCL(0);
	SDA = 1;
	iic_delay();
}

void iic_snd_no_ack(void)
{	
	SDA = 1;
	//SCL = 1;
	SCL(1);
	iic_delay();
	//SCL = 0;
	SCL(0);
	SDA = 1;
	iic_delay();
}

unsigned char ee_rdb_8k(unsigned int adr)
{	
	unsigned char byte;
	iic_start();
	iic_send(0xA0);		// Adresse EEPROM im Schreibmodus
	while(iic_rec_ack());
	iic_send(adr >> 8);
	iic_rec_ack();
	iic_send(adr);
	iic_rec_ack();
	iic_stop();
	iic_start();
	iic_send(0xA1);		// Adresse EEPROM im Lesemodus
	iic_rec_ack();
	byte = iic_receive();
	iic_snd_no_ack();
	//iic_rec_ack();
	iic_stop();
	return (byte);
}

void ee_wrb_8k(unsigned int adr,unsigned char val)
{	
	iic_start();
	iic_send(0xA0);
	while(iic_rec_ack());
	iic_send(adr >> 8);
	iic_rec_ack();
	iic_send(adr);
	iic_rec_ack();
	iic_send(val);
	iic_rec_ack();
	iic_stop();
}		

unsigned char ee_rdb_128k(bit page, unsigned int adr)
{	
	// A1 = 0, wenn nicht angeschlossen
	unsigned char byte;
	unsigned int tmp;
	
	(page) ? (tmp = 0xa2) : (tmp = 0xa0);
	iic_start();
	iic_send(tmp);		// Adresse EEPROM im Schreibmodus
	while(iic_rec_ack());
	iic_send(adr >> 8);
	iic_rec_ack();
	iic_send(adr);
	iic_rec_ack();
	iic_start();
	iic_send(tmp | 1);		// Adresse EEPROM im Lesemodus
	iic_rec_ack();
	byte = iic_receive();
	iic_snd_no_ack();
	//iic_rec_ack();
	iic_stop();
	return (byte);
}

void ee_wrb_128k(bit page, unsigned int adr,unsigned char val)
{	
	unsigned int tmp;
	
	(page) ? (tmp = 0xa2) : (tmp = 0xa0);
	iic_start();
	iic_send(tmp);
	while(iic_rec_ack());
	iic_send(adr >> 8);
	iic_rec_ack();
	iic_send(adr);
	iic_rec_ack();
	iic_send(val);
	iic_rec_ack();
	iic_stop();
}		

void ee_wrseq_128k(unsigned long adr, unsigned char len, unsigned char *ptr)
{	
	unsigned int tmp,
				 i;
	bit page;
	
	i   = 0;
	while(i < len)
	{	
		page = (bit)(adr >> 16);
		(page) ? (tmp = 0xa2) : (tmp = 0xa0);
		iic_start();
		iic_send(tmp);
		while(iic_rec_ack());
		iic_send(adr >> 8);
		iic_rec_ack();
		iic_send(adr);
		iic_rec_ack();
		while(adr % 0xff)
		{	
			iic_send(ptr[i]);
			iic_rec_ack();
			++adr;
			if(++i >= len)
			{
				iic_stop();
				break;
			}
		}
	}
}		

unsigned char ee_rdseq_128k(unsigned long adr, unsigned char len, unsigned char *ptr)
{	
	// A1 = 0, wenn nicht angeschlossen
	unsigned int tmp,
				 i;
	bit page;
	
	i   = 0;
	while(i < len)
	{	
		page = (bit)(adr >> 16);
		(page) ? (tmp = 0xa2) : (tmp = 0xa0);
		iic_start();
		iic_send(tmp);
		while(iic_rec_ack());
		iic_send(adr >> 8);
		iic_rec_ack();
		iic_send(adr);
		iic_rec_ack();
		iic_start();
		iic_send(tmp | 1);		// Adresse EEPROM im Lesemodus
		iic_rec_ack();
		while(adr % 0xff)
		{	
			ptr[i] = iic_receive();
			++adr;
			if(++i >= len)
			{
				iic_snd_no_ack();
				iic_stop();
				break;
			}
			else
				iic_snd_ack();
		}
	}
}

char AT24C512_read(unsigned int adr, unsigned char *ptr)
{
	unsigned char i;
	
	iic_start();
	iic_send(0xA2);		// Adresse AT24C512 im Schreibmodus
	if(iic_rec_ack()) 
		return 0;
	adr = adr * 128;
	iic_send(adr >> 8);
	iic_rec_ack();
	iic_send(adr);
	iic_rec_ack();
	
	iic_start();
	iic_send(0xA3);		// Adresse AT24C512 im Lesemodus
	iic_rec_ack();
	for(i = 0; i < 127; i++)
	{
		ptr[i] = iic_receive();
		iic_snd_ack();
	}
	ptr[i] = iic_receive();
	iic_snd_no_ack();
	iic_stop();
	return 1;
}




// 128 Byte aus AT24C512 lesen. 
char AT24C512_read(unsigned int adr, unsigned char *ptr)
{
	unsigned char i;
	
	iic_start();
	iic_send(0xA2);		// Adresse AT24C512 im Schreibmodus
	if(iic_rec_ack()) 
		return 0;
	adr = adr * 128;
	iic_send(adr >> 8);
	iic_rec_ack();
	iic_send(adr);
	iic_rec_ack();
	
	iic_start();
	iic_send(0xA3);		// Adresse AT24C512 im Lesemodus
	iic_rec_ack();
	for(i = 0; i < 127; i++)
	{
		ptr[i] = iic_receive();
		iic_snd_ack();
	}
	ptr[i] = iic_receive();
	iic_snd_no_ack();
	iic_stop();
	return 1;
}

