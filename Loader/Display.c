#include <stdlib.h>
#include <intrins.h>
#include <string.h>
#include "ConstDef.h"                 // SFR declarations


// #define A0(B)    (B) ? (P4 |= 0x08) : (P4 &= 0xf7);
// #define RESET(B) (B) ? (P4 |= 0x01) : (P4 &= 0xfe);
// #define CSA(B)   (B) ? (P4 |= 0x02) : (P4 &= 0xfd);
// #define CSB(B)   (B) ? (P4 |= 0x04) : (P4 &= 0xfb);
// #define A0(B)    (B) ? (P4 |= 0x08) : (P4 &= 0xf7);
#define RESET(B) (B) ? (P4 |= 0x08) : (P4 &= ~0x08);
sbit A0    = P3^7;
sbit CSA   = P3^5;
sbit CSB   = P3^6;



extern void WDT_RT(void);
// extern bit CLCKTCK;
xdata RRAM rram;
static xdata unsigned char zlr;
static xdata unsigned char anf;
static xdata unsigned char end;
static xdata unsigned char seg;

static unsigned char bdata swap;
sbit b0 = swap ^ 0;
sbit b1 = swap ^ 1;
sbit b2 = swap ^ 2;
sbit b3 = swap ^ 3;
sbit b4 = swap ^ 4;
sbit b5 = swap ^ 5;
sbit b6 = swap ^ 6;
sbit b7 = swap ^ 7;

static unsigned char bdata snd;
sbit s0 = snd ^ 0;
sbit s1 = snd ^ 1;
sbit s2 = snd ^ 2;
sbit s3 = snd ^ 3;
sbit s4 = snd ^ 4;
sbit s5 = snd ^ 5;
sbit s6 = snd ^ 6;
sbit s7 = snd ^ 7;


void send(bit command, unsigned char daten)
{
	//unsigned char ix;
	A0 = command;
	SPIF = 0;
	while(!TXBMT); // TXBMT: Transmit Buffer Empty
	SPI0DAT = daten;
	while(!SPIF); 
}

void pause(unsigned char c)
{
	unsigned char i;
	TR0 = 1;
	for(i = 0; i < c; ++i)
	{
		while(!TF0) ;
		TF0 = 0;
		WDT_RT();
	}
	TR0 = 0;
}
	


void display_reset(void)
{
	xdata unsigned char cnt,
						ix;
	RESET(0);
	pause(10);
	RESET(1);
	CSA = 0; CSB = 1;
	for(cnt = 0; cnt < 2; cnt++)
	{
		send(0,0xe2); //  reset
		send(0,0xa3); //  11 LCD Bias set
		send(0,0xa1); // 8 ADC select
		send(0,0xc0); // 15 common output selection mod
		send(0,0x23); // 17 internal resistor ratio set
		send(0,0x81); // 18 electronic volume
		send(0,0x28); // 18 electronic volume
		send(0,0xf8); // 20 
		send(0,0x03); // 20 
		//----------------------
		send(0,0x2f); // 16 power control set
		send(0,0xa4); // 10 display all points on/off
		send(0,0xac); // 19 static indicator off
		send(0,0xaf); // 1 display on
		CSA = 1; CSB = 0;
	}
	CSA = 0; CSB = 1;
	for(cnt = 0; cnt < 8; cnt++)
	{
		send(0,0xb0 + cnt);	//3 page adress set 
		send(0,0x00);	//4 column adress set low
		send(0,0x10);	//4 column adress set high
		// _nop_();
		// _nop_();
		// _nop_();
		// _nop_();
		// _nop_();
		for(ix = 0; ix < 120; ix++)
		{
			send(1,0x00);
		}
	}	
	CSA = 1; CSB = 0;
	for(cnt = 0; cnt < 8; cnt++)
	{
		send(0,0xb0 + cnt);	//3 page adress set 
		send(0,0x00);	//4 column adress set low
		send(0,0x10);	//4 column adress set high
		// _nop_();
		// _nop_();
		// _nop_();
		// _nop_();
		// _nop_();
		for(ix = 0; ix < 120; ix++)
		{
			send(1,0x00);
		}
	}	
	CSA = 1; CSB = 1;
}
 


// void SPI_irq(void) interrupt 6
// {
	// SPIF = 0;
	// ++zlr;
	// if(zlr < end)
		// SPI0DAT = rram.vec[seg][zlr];
	// else 
		// ESPI0 = 0;
// }


// void bild_txt(unsigned char nr, unsigned char pos, unsigned char txt)
// {
	// memcpy(rram.vec[nr] + pos, bmp_vec[txt], bmp_len[txt]);
// }

void display_flush(bit num, bit pos, bit invt) // 1 oder 2 Display normal oder umdrehen Bild invertieren
{
	xdata unsigned char n; // ix 
	
	if(!num) // 1. Display
	{
		CSA = 0; CSB = 1;
		anf = 0;
		end = WIDTH / 2;
	}
	else // 2. Display
	{
		CSA = 1; CSB = 0;
		anf = WIDTH / 2;
		end = WIDTH;
	}
	
	if(!pos) // normal oder umdrehen
	{	
		// WDT_RT();
		for(seg = 0; seg < 8; seg++)
		{
			send(0, 0xb0 + seg);
			send(0, 0x10);
			send(0, 0x00);
			zlr     = anf;
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			for(zlr = anf; zlr < end; ++zlr)
			{
				send(1, rram.vec[seg][zlr]);
				WDT_RT();
			}
		}	
	}
	else
	{
		xdata unsigned char nn,ixx;//inv, 
		if(num)
		{
			anf = 0;
			end = WIDTH / 2;
		}
		else
		{	
			anf = WIDTH / 2;
			end = WIDTH;
		}
		
		for(nn = 7, n = 0; nn < 8; nn--, n++)
		{
			send(0, 0xb0 + n);
			send(0, 0x10);
			send(0, 0x00);
			
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			A0 = 1;
			for(ixx = end - 1; ixx < end && ixx >= anf; ixx--)
			{
				swap = rram.vec[nn][ixx];
				s7 = b0;
				s6 = b1;
				s5 = b2;
				s4 = b3;
				s3 = b4;
				s2 = b5;
				s1 = b6;
				s0 = b7;
				send(1, snd);
				WDT_RT();
			}
		}	
	}
	if(!invt) 
		send(0,0xa6);
	else
		send(0,0xa7); // Bild invertieren
		
	CSA = 1; CSB = 1;
}


/*
void display_flush(bit num, bit pos, bit invt) // 1 oder 2 Display normal oder umdrehen Bild invertieren
{
	xdata unsigned char n; // ix 
	
	SPIF = 0;
	if(!num) // 1. Display
	{
		CSA = 0; CSB = 1;
		anf = 0;
		end = WIDTH / 2;
	}
	else // 2. Display
	{
		CSA = 1; CSB = 0;
		anf = WIDTH / 2;
		end = WIDTH;
	}
	
	if(!pos) // normal oder umdrehen
	{	
		// WDT_RT();
		for(seg = 0; seg < 8; seg++)
		{
			//A0(0);
			A0 = 0;
			SPIF = 0;
			SPI0DAT = 0xb0 + seg;
			while(!SPIF); // 
			SPI0DAT = 0x10;
			while(!SPIF); // 
			SPIF = 0;
			SPI0DAT = 0x00;
			while(!SPIF); // 
			zlr     = anf;
			SPIF    = 0;
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			//A0(1);
			A0 = 1;
			for(zlr = anf; zlr < end; ++zlr)
			{
				SPIF = 0;
				SPI0DAT = rram.vec[seg][zlr];
				while(!SPIF); 
				WDT_RT();
			}
		}	
	}
	else
	{
		xdata unsigned char nn,ixx;//inv, 
		//bit b;
		if(num)
		{
			anf = 0;
			end = WIDTH / 2;
		}
		else
		{	
			anf = WIDTH / 2;
			end = WIDTH;
		}
		
		for(nn = 7, n = 0; nn < 8; nn--, n++)
		{
			// A0(0);
			A0 = 0;
			SPIF = 0;
			SPI0DAT = 0xb0 + n;
			while(!SPIF); // 
			SPI0DAT = 0x10;
			SPIF = 0;
			while(!SPIF); 
			SPI0DAT = 0x00;
			SPIF = 0;
			while(!SPIF); 
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			A0 = 1;
			for(ixx = end - 1; ixx < end && ixx >= anf; ixx--)
			{
				swap = rram.vec[nn][ixx];
				s7 = b0;
				s6 = b1;
				s5 = b2;
				s4 = b3;
				s3 = b4;
				s2 = b5;
				s1 = b6;
				s0 = b7;
				SPIF = 0;
				SPI0DAT = snd;//inv;
				while(!SPIF);
				// PCA0CPH4 = 255;     	
				WDT_RT();
			}
		}	
	}
	if(!invt) 
		send(0,0xa6);
	else
		send(0,0xa7); // Bild invertieren
		
	CSA = 1; CSB = 1;
}
*/





















