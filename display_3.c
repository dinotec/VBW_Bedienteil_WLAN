#include <stdlib.h>
#include <intrins.h>
#include <string.h>
#include "ConstDef.h"                 // SFR declarations
// #include "extern.h"             
#include "eeprom128.h"      
#include "font.h"                 
#include "dino2.h"                

#define START_LINE 0x40

// #define A0(B)    (B) ? (P4 |= 0x08) : (P4 &= 0xf7);
#define RESET(B) (B) ? (P4 |= 0x08) : (P4 &= ~0x08);
// #define CSA(B)   (B) ? (P4 |= 0x02) : (P4 &= 0xfd);
// #define CSB(B)   (B) ? (P4 |= 0x04) : (P4 &= 0xfb);

sbit A0    = P3^7;
// sbit RESET = P4^3;
sbit CSA   = P3^5;
sbit CSB   = P3^6;



extern void WDT_RT(void);
extern bit CLCKTCK;
extern xdata unsigned char s_zeichen[];
extern xdata unsigned char MENU_MODE;
extern xdata RRAM rram;
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


void bmp_text1(unsigned char, unsigned char, unsigned char *);
void bmp_text2(unsigned char, unsigned char, unsigned char *);
// void bmp_text3(unsigned char, unsigned char, unsigned char *);
void line_h(unsigned char, unsigned char, unsigned char *);
void line_v(unsigned char, unsigned char, unsigned char *);

code unsigned char bargr[][5] = {
	{0x3c, 0x00, 0x00, 0x00, 0x00},
	{0x3c, 0x3c, 0x00, 0x00, 0x00},
	{0x3c, 0x3c, 0x3c, 0x00, 0x00},
	{0x3c, 0x3c, 0x3c, 0x3c, 0x00},
	{0x3c, 0x3c, 0x3c, 0x3c, 0x3c}
};


// code void (*fptr[])(unsigned char, unsigned char, unsigned char*) = {bmp_text1, bmp_text2, bmp_text3, line_h, line_v, bmp_text1};
// xdata void (*fptr[])(unsigned char*) = {bmp_text1, bmp_text2, bmp_text3, line_h, line_v, bmp_text1};
// xdata void (*fptr[])(unsigned char, unsigned char, unsigned char*) = {bmp_text1, bmp_text2, bmp_text3, line_h, line_v, bmp_text1};

void send(bit command, unsigned char daten) {
	//unsigned char ix;
	A0 = command;
	// A0(command);
	SPIF = 0;
	while(!TXBMT); // TXBMT: Transmit Buffer Empty
	SPI0DAT = daten;
	while(!SPIF); 
}


void display_reset(void) {
	xdata unsigned char cnt,
						ix;
	
	P4 |= 0x70;
	
	RESET(0);
	for(cnt = 0; cnt < 10; cnt++) {
		while(!CLCKTCK);
		CLCKTCK = 0;
	}
	RESET(1);
	CSA = 0; CSB = 1;
	for(cnt = 0; cnt < 2; cnt++) {
		send(0,0xe2); //  reset
		send(0,0xa3); //  11 LCD Bias set
		send(0,0xa1); // 8 ADC select
		send(0,0xc0); // 15 common output selection mod
		send(0,0x23); // 17 internal resistor ratio set
		send(0,0x81); // 18 electronic volume
		send(0,vek[contr].b[1]); // 18 electronic volume
		//----------------------
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
	for(cnt = 0; cnt < 8; cnt++) {
		send(0,0xb0 + cnt);	//3 page adress set 
		send(0,0x00);	//4 column adress set low
		send(0,0x10);	//4 column adress set high
		for(ix = 0; ix < 120; ix++) {
			send(1,0x00);
		}
	}	
	CSA = 1; CSB = 0;
	for(cnt = 0; cnt < 8; cnt++) {
		send(0,0xb0 + cnt);	//3 page adress set 
		send(0,0x00);	//4 column adress set low
		send(0,0x10);	//4 column adress set high
		for(ix = 0; ix < 120; ix++) {
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

	
void bmp_text1(unsigned char col, unsigned char row, unsigned char *ptr) {
	xdata unsigned char x,
				  y,
				  i,
				  m,
				  n,
				  c_n,
				  len,
				  //*c,
				  tmp;
	xdata unsigned char *c;
	// xdata unsigned char s_zeichen[5];
	
	len = strlen(ptr);				
	x   = row / 8;
	y   = row % 8;
	c_n = 5;
	for(i = col, m = 0; m < len && i < WIDTH-5 ; i += 6, m++) {
		if(ptr[m] <= 0x80) {	
			tmp = ptr[m] - 0x20;
			c = font1[tmp];
		} else if(ptr[m] >= 0xfb) {	
			tmp = ptr[m] - 0xfb;
			c = bargr[tmp];
		} else {	
			c = s_zeichen + (ptr[m] - 0x81)*5;
		}
		for(n = 0; n < c_n; n++) {
			if(i + n >= WIDTH) break;
			if(x > 7) continue;
			rram.vec[x][i + n] |= (*(c + n)) << y;
			if(y) {
				if(x > 6) continue;
				rram.vec[x + 1][i + n] |= (*(c + n)) >> (8 - y);
			}
		}
	}
}

unsigned char bmp_text1_n(unsigned char col, unsigned char row, unsigned char *ptr) {
	xdata unsigned char x,
				  y,
				  i,
				  m,
				  n,
				  c_n,
				  len,
				  //*c,
				  tmp;
	xdata unsigned char *c;
	// xdata unsigned char s_zeichen[5];
	
	len = strlen(ptr);				
	x   = row / 8;
	y   = row % 8;
	c_n = 5;
	for(i = col, m = 0; m < len && i < WIDTH-5 ; i += 6, m++) {
		if(ptr[m] <= 0x80) {	
			tmp = ptr[m] - 0x20;
			c = font1[tmp];
		} else if(ptr[m] >= 0xfb) {	
			tmp = ptr[m] - 0xfb;
			c = bargr[tmp];
		} else {	
			c = s_zeichen + (ptr[m] - 0x81)*5;
		}
		for(n = 0; n < c_n; n++) {
			if(i + n >= WIDTH) break;
			if(x > 7) continue;
			rram.vec[x][i + n] |= (*(c + n)) << y;
			if(y) {
				if(x > 6) continue;
				rram.vec[x + 1][i + n] |= (*(c + n)) >> (8 - y);
			}
		}
	}
}

void bmp_text2(unsigned char col, unsigned char row, unsigned char *ptr) {
	xdata unsigned char x,
				  y,
				  i,m,
				  n,
				  len;
	xdata unsigned int *pr;
	xdata UINT num;

	len = strlen(ptr);				
	x = row / 8;
	y = row % 8;
	// for(i = col, m = 0; m < len && i < 120 ; i += 12, m++)
	for(i = col, m = 0; m < len && i < WIDTH-12 ; i += 12, m++) {
		if(ptr[m] >= '+' && ptr[m] <= ';') pr = Zahlen[ptr[m] - 0x2b]; 
		else if(ptr[m] == '°')             pr = Zahlen[17]; 
		else if(ptr[m] == 'C')             pr = Zahlen[18]; 
		else                               pr = Zahlen[19]; 
		
		for(n = 0; n < 10; n++) {
			//if(i + n > 119) break;
			if(i + n >= WIDTH) break;
			num.i = *(pr + n);
			if(x > 7) continue;
			rram.vec[x][i + n] |= (num.b[1]) << y; 
			if(y) {
				if(x > 6) continue;
				rram.vec[x + 1][i + n] |= (num.b[1]) >> (8 - y);
			}
			rram.vec[x+1][i + n] |= (num.b[0]) << y;
			if(y) {
				if(x > 5) continue;
				rram.vec[x + 2][i + n] |= (num.b[0]) >> (8 - y);
			}
		}
	}
}


/*
void bmp_text3(unsigned char col, unsigned char row, unsigned char *ptr)
// void bmp_text3(unsigned char *ptr)
{
	xdata unsigned char x,
				  y,
				  i,m,
				  n,
				  len;
	xdata unsigned int *pr;
	xdata UINT num;
	
	len  = strlen(ptr);				
	x    = row / 8;
	y    = row % 8;
	// for(i = col, m = 0; m < len && i < 120 ; i += 12, m++)
	for(i = col, m = 0; m < len && i < WIDTH ; i += 12, m++)
	{
		
		switch(ptr[m])
		{
			case 'B' :  pr = f_8x12[1]; break; 
			case 'C' :  pr = f_8x12[2]; break; 
			case 'D' :  pr = f_8x12[3]; break; 
			case '1' :  pr = f_8x12[4]; break; 
			case '2' :  pr = f_8x12[5]; break; 
			case '3' :  pr = f_8x12[6]; break; 
			default  :  pr = f_8x12[0]; break; 
		}
		
		for(n = 0; n < 8; n++)
		{
			num.i = *(pr + n);
			// if(i + n > 119) break;
			if(i + n >= WIDTH) break;
			if(x > 7) continue;
			rram.vec[x][i + n] |= (num.b[1]) << y; //pr[n].b[1]
			if(y) 
			{
				if(x > 6) continue;
				rram.vec[x + 1][i + n] |= (num.b[1]) >> (8 - y);
			}
			if(x > 6) continue;
			rram.vec[x+1][i + n] |= (num.b[0]) << y;
			if(y) 
			{
				if(x > 5) continue;
				rram.vec[x + 2][i + n] |= (num.b[0]) >> (8 - y);
			}
		}
	}
}


void bilder(void)
{
	xdata unsigned char page;
	for(page = 0; page < 8; page++) 
	{
		memcpy(rram.vec[page],dino1[page],sizeof(dino1[page]));
		memcpy(rram.vec[page] + 120,dino1[page],sizeof(dino1[page]));
	}
	//memcpy(rram.vec[0],dino1[0],sizeof(rram.vec[0]) * 8);
}
*/
void bilder2(void) {
	xdata unsigned char page;
	memset(&rram, 0, sizeof(rram));
	#ifndef OEM
	for(page = 0; page < 8; page++) {
		memcpy(rram.vec[page],dino2[page],sizeof(dino2[page]));
	}
	#endif
	//memcpy(rram.vec[0],dino1[0],sizeof(rram.vec[0]) * 8);
}
void bilder3(void) {
	xdata unsigned char page;
	for(page = 0; page < 3; ++page) {
		memcpy(rram.vec[page] + 120,dino_logo[page],sizeof(dino_logo[page]));
	}
	//memcpy(rram.vec[0],dino1[0],sizeof(rram.vec[0]) * 8);
}
/*
void wasser_bild(void) {
	xdata unsigned char page;
	for(page = 0; page < 2; ++page) {
		memcpy(rram.vec[page + 4] + 122,wasser[page],sizeof(wasser[page]));
	}
}
void hand_bild(void) {
	xdata unsigned char page;
	for(page = 0; page < 2; ++page) {
		memcpy(rram.vec[page + 4] + 130,hand[page],sizeof(hand[page]));
	}
}
*/
// void bild_8x12(void)
// {
	// memcpy(rram.vec[0],Font_8_12[0],sizeof(rram.vec[0]) * 8);
// }

void line_h(unsigned char col, unsigned char row, unsigned char *plen) {
	xdata unsigned char page,
				  page_nr,
				  len,
				  i;
	
	len = atoi(plen);
	row %= HEIGHT;
	col %= WIDTH;
	page    = row / 8;
	page_nr = 1 << (row % 8);
	if(page > 7 ) return;
	len += col;
	if(len > WIDTH) len = WIDTH;
	for(i = col; i < len ; i++) {
		rram.vec[page][i] |= page_nr;	
	}	
}

void line_v(unsigned char col, unsigned char row, unsigned char *plen) {
	xdata unsigned char page,
				  page_nr,
				  len,
				  i;
	
	row %= HEIGHT;
	col %= WIDTH;
	len = atoi(plen);
	len += row;
	if(len > HEIGHT) len = HEIGHT;
	for(i = row; i < len ; i++) {
		page         = i / 8;
		page_nr      = 1 << (i % 8);
		rram.vec[page][col] |= page_nr;	
	}	
}



void display_wr(unsigned char *ptr) {
	xdata unsigned char col,
						row,
						font;
	font = atoi(ptr);
	font %= 6;
	col	 = atoi(ptr + 3);	  
	col %= WIDTH;
	row	 = atoi(ptr + 7);
	row %= HEIGHT;
	switch(font) {
		default: bmp_text1(col,row,ptr + 11); break;
		case 1:  bmp_text2(col,row,ptr + 11); break;
		// case 2:  bmp_text3(col,row,ptr + 11); break;
		case 3:  line_h(col,row,ptr + 11);    break;
		case 4:  line_v(col,row,ptr + 11);    break;
	}
}

void display_flush(bit num, bit pos, bit invt) { // 1 oder 2 Display normal oder umdrehen Bild invertieren
	xdata unsigned char n; // ix 
	if(!num) { // 1. Display
		CSA = 0; CSB = 1;
		// CSA(0); CSB(1);
		anf = 0;
		end = WIDTH / 2;
	} else { // 2. Display
		CSA = 1; CSB = 0;
		// CSA(1); CSB(0);
		anf = WIDTH / 2;
		end = WIDTH;
	}
	if(!pos) {// normal oder umdrehen
		// WDT_RT();
		for(seg = 0; seg < 8; seg++) {
			send(0, 0xb0 + seg);
			send(0, 0x10);
			send(0, 0x00);
			zlr     = anf;
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			for(zlr = anf; zlr < end; ++zlr) {
				send(1, rram.vec[seg][zlr]);
				WDT_RT();
			}
		}	
	} else {
		xdata unsigned char nn,ixx;//inv, 
		//bit b;
		if(num) {
			anf = 0;
			end = WIDTH / 2;
		} else {	
			anf = WIDTH / 2;
			end = WIDTH;
		}
		
		for(nn = 7, n = 0; nn < 8; nn--, n++) {
			send(0, 0xb0 + n);
			send(0, 0x10);
			send(0, 0x00);
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			A0 = 1;
			for(ixx = end - 1; ixx < end && ixx >= anf; ixx--) {
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
	send(0, START_LINE);
	//~ send(0,0xa1); // 8 ADC select
		send(0,0xa3); //  11 LCD Bias set
		send(0,0xa1); // 8 ADC select
		send(0,0xc0); // 15 common output selection mod
		send(0,0x23); // 17 internal resistor ratio set
		send(0,0x81); // 18 electronic volume
		send(0,vek[contr].b[1]); // 18 electronic volume
		//----------------------
		send(0,0xf8); // 20 
		send(0,0x03); // 20 
		//----------------------
		send(0,0x2f); // 16 power control set
		send(0,0xa4); // 10 display all points on/off
		send(0,0xac); // 19 static indicator off
		send(0,0xaf); // 1 display on

	if(!invt) 
		send(0,0xa6);
	else
		send(0,0xa7); // Bild invertieren
		
	CSA = 1; CSB = 1;
	// CSA(1); CSB(1);
}





















