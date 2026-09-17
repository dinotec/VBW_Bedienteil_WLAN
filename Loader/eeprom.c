#include "ConstDef.h"                 // SFR declarations
#include "Read_AT24C1024.h"                 // SFR declarations

xdata unsigned long adr_128k;
xdata unsigned char *ptr_128k;
xdata unsigned int len_128k;


// Verzögerung für IIC Routinen
// Ausgelegt für 400kHz Bausteine !
// void iic_delay(void)
// {	
	// unsigned char n;
	// for (n = (SYSCLK / 1000000); n > 0; n--) ;
// }

// void iic_delay(void)
// {	
	// // xdata unsigned char n;
	// // for (n = 1; n > 0; n--) 
	// _nop_();
// }

// void SCL(bit b)
// {
	// if(b) P4 |= 0x01;
	// else  P4 &= 0xfe;
// }


void iic_start(void)
{	
	SCL = 1;
	_nop_();
	SDA = 0;
	_nop_();
	SCL = 0;
	_nop_();
}		 

// Ein Byte über IIC_Bus einlesen.
char iic_receive(void)
{	
	unsigned char byte = 0;		
	char n;
	for(n = 8; n > 0; n--)
	{ 	
		byte <<= 1;
		SCL = 1;
		_nop_();	
		if (SDA == 1)	byte |= 1 ;//bitmaski(n - 1);
		_nop_();
		SCL = 0;
		_nop_();
	}
	_nop_();
	return (byte);
}

// Acknowledge Bit senden
void iic_snd_ack(void)
{	
	SDA = 0;
	SCL = 1;
	_nop_();
	SCL = 0;
	SDA = 1;
	_nop_();
}

void iic_snd_no_ack(void)
{	
	SDA = 1;
	_nop_();
	SCL = 1;
	_nop_();
	SCL = 0;
	_nop_();
	SDA = 1;
	_nop_();
}

bit iic_rec_ack(void)
{	
	bit b;
	SCL = 0;
	_nop_();
	SDA = 1;
	_nop_();
	SCL = 1;
	_nop_();
	b = SDA;
	_nop_();
	SCL = 0;
	_nop_();
	return b;
}


// Stopsequenz übertragen
// Das Ende einer Übertragung wird angezeigt und zugleich der Ruhezustand
// des Busses wiederhergestellt.
void iic_stop(void)
{	
	SDA = 0;
	_nop_();
	SCL = 1;
	_nop_(); 
	SDA = 1;	
	_nop_();
}

void iic_send(char byte)
{	
	char n;
	for (n = 8; n > 0; n--)
	{	
		if ((byte & 0x80) == 0) SDA = 0;
		else SDA = 1;
		SCL = 1;
		_nop_();
		SCL = 0;
		byte = byte <<1;
		_nop_();
	}		
	_nop_();
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
