//#include <ABSACC.H>

#include "c8051f340.h"                 // SFR declarations
//#include <stdio.h>
//#include <string.h>
//#include <intrins.h>
//-----------------------------------------------------------------------------
// Global VARIABLES
extern unsigned char code * xdata id;
void Wait_ms(unsigned int count);
unsigned char MMC_FLASH_Block_Read(unsigned long address, unsigned char *pchar) ;
void MMC_FLASH_Init(unsigned char *ptr);
void send(unsigned int i);
void send_str(unsigned char *ptr);
void sl(unsigned char c);
//void Block_loeschen(unsigned int bl);
void FLASH_PageErase (unsigned int addr);
//void Flash_schreiben(unsigned char *ptr);
void FLASH_Write (unsigned int dest, char *src, unsigned int numbytes);
char strn_cmp(char * str1, char *str2, unsigned char n);
unsigned long Calc_CRC(unsigned char a, unsigned long crc);


unsigned char Write_Read_Spi_Byte(unsigned char byte);

sbit C_PWR = P1^7;

typedef union UINT
{
	unsigned int i;
	unsigned char b[2];
} UINT;


void update(void)
{
	xdata UINT len;
	xdata unsigned int cnt,
	                   y,
					   y0;
	                   
	xdata unsigned long crc;
	xdata unsigned char page_cnt,
                        Scratch[512],
						id[30],
						q;
    unsigned char code * xdata pread;           // FLASH read pointer

	MMC_FLASH_Init(Scratch);
	
	for(q = 0, pread = 0x1000; q < 30; q++) id[q] = *pread++; 
	for(y = 610; y < 620; y++)
	{
		if(MMC_FLASH_Block_Read(y,Scratch) == 0) 
		{
			send_str("Kann die Karte nicht lesen...");
			break;	
		}
		send(y);
		sl(0x0d),sl(0x0a);
		//for(i=0;i<512;i++)
		// for(i = 0; i < 20; i++)
		// {
			// if(Scratch[i]) sl(Scratch[i]);
			// if((i%70) ==0) sl(0x0d),sl(0x0a);
		// }
		// sl(0x0d),sl(0x0a);
		// for(i = 0; i < 30; i++) sl(Scratch[i+3]);
		// sl(0x0d),sl(0x0a);
		// send_str(id);
		if(!strn_cmp(Scratch+2,id,26))// erste 2 Byte -> Länge
		{
			send_str("Update gefunden...");
			//break;
						
			if(!strn_cmp(Scratch+2,id,30))
			{
				send_str("Gleiche Version.");
			}
			else
			{
				len.b[0] = Scratch[0]; 
				len.b[1] = Scratch[1]; 
				page_cnt = len.i / 512;
				if(len.i % 512) page_cnt++;
				//for(n = 0; n < page_cnt; n++)
				
				
				
				//----------------------------------------------------------------------------
				for(cnt = 0, crc = 0, y0 = y; cnt < len.i; cnt++)
				{
					if(cnt % 512==0 && cnt)
					{
						if(MMC_FLASH_Block_Read(y,Scratch) == 0) 
						{
							send_str("Kann die Karte nicht lesen...");
							break;	
						}
					}
					crc = Calc_CRC(Scratch[cnt % 512], crc);
					
				}
				send_str("Flash schreiben...");
				if(0)//strn_cmp(Scratch,id,30) )
				{
					FLASH_PageErase(0x1000);
					Wait_ms(50);
					FLASH_Write(0x1000, Scratch+2, 510);
					send_str("Flash ist beschrieben.");
				}
				//else send_str("Flash ist nicht beschrieben.");
				
			}
			break;
		}
	}
}

unsigned long Calc_CRC(unsigned char a, unsigned long crc)
{
	xdata unsigned long l;
	xdata unsigned char n;
	//l = a * 256;
	l = a << 8;
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

unsigned char MMC_Command_Exec (unsigned char cmd_loc, unsigned long argument,unsigned char *pchar)
{
	xdata unsigned char loopguard;
	//xdata unsigned char idata card_response;
	xdata unsigned char card_response;
	xdata unsigned int i;
	typedef union 
	{                        // byte-addressable unsigned long
	    unsigned long l;
	    unsigned char b[4];
	} ULONG;
	//ULONG idata long_arg;               // Union variable for easy byte 
	ULONG xdata long_arg;               // Union variable for easy byte 
                                     
	card_response = 0;
	Write_Read_Spi_Byte(0xFF);
	NSSMD0 = 0;
	Write_Read_Spi_Byte(0xFF);
	Write_Read_Spi_Byte(cmd_loc);
	long_arg.l = argument;              // Make argument byte addressable;
	Write_Read_Spi_Byte(long_arg.b[0]);
	Write_Read_Spi_Byte(long_arg.b[1]);
	Write_Read_Spi_Byte(long_arg.b[2]);
	Write_Read_Spi_Byte(long_arg.b[3]);

	if(cmd_loc==0x40) Write_Read_Spi_Byte(0x95);
	else              Write_Read_Spi_Byte(0xff);

	loopguard=0;
	do 
	{
		card_response = Write_Read_Spi_Byte(0xFF);
		if(!++loopguard) break;
		if(card_response & 0x80) 
		{ 
			Wait_ms(1);
		}
	} while(card_response & 0x80);
    if(cmd_loc == 0x51 && loopguard>0)
    {                                   
		// Read data from the MMC;
		loopguard = 0;
		card_response=0;
		while(card_response != 0xfe) 
		{
			card_response=Write_Read_Spi_Byte(0xFF);
			Wait_ms(1);
			if(!++loopguard) break;
		}	
		for(i=0;i<512;i++) pchar[i]=Write_Read_Spi_Byte(0xFF);  
		Write_Read_Spi_Byte(0xFF);//CRC
		Write_Read_Spi_Byte(0xFF);//CRC
    }
    Write_Read_Spi_Byte(0xFF);
    NSSMD0 = 1;
    Write_Read_Spi_Byte(0xFF);
    return card_response;
}
extern void send_str(unsigned char *ptr);
//-----------------------------------------------------------------------------
// MMC_FLASH_Init
//-----------------------------------------------------------------------------
void MMC_FLASH_Init(unsigned char *ptr)
{
	xdata unsigned char loopguard,
	                    card_status,
				        counter;

	send_str("Start INI");
	C_PWR = 1;
	Wait_ms(50);
	C_PWR = 0;
	Wait_ms(100);
	for(counter = 0; counter < 10; counter++) 
	{
		Write_Read_Spi_Byte(0xFF);
	}
  
	NSSMD0 = 0;                         // Select the MMC with the CS pin;
	card_status = MMC_Command_Exec(0x40,0,ptr);
    send_str("nach GO_IDLE_STATE");
	loopguard=0;
    // Send the SEND_OP_COND command
	do  // until the MMC indicates that it is
	{         
		Wait_ms(1);
		card_status = MMC_Command_Exec(0x41,0,ptr);
		if(!++loopguard) break;
	} while (card_status & 0x01);

	if(!loopguard) return ;

    Write_Read_Spi_Byte(0xFF);
    send_str("INIT Ende");
}

//-----------------------------------------------------------------------------
// MMC_FLASH_Block_Read
//-----------------------------------------------------------------------------

unsigned char MMC_FLASH_Block_Read(unsigned long address, unsigned char *pchar) 
{
	xdata unsigned char card_status,     // Stores MMC status after each MMC command;
						q;	
	q = 250;
	do
	{
		address <<= 9;////addr = addr * 512
		card_status = MMC_Command_Exec(0x51,address,pchar);
		if(!++q) break;
	} while(card_status != 0xfe);
	return q;
	// address <<= 9;////addr = addr * 512
	// card_status = MMC_Command_Exec(0x51,address,pchar);
	// return card_status;
}

/*
		while((len.i != 0xfe)) 
		{
			len.i = MMC_FLASH_Block_Read(y,Scratch);
			if(!++q) break;
		}

*/

unsigned char Write_Read_Spi_Byte(unsigned char byte)
{
	xdata unsigned char ret;
	SPI0DAT = byte;
	while(!SPIF);                      
	SPIF = 0;
	ret = SPI0DAT;
	return ret;
}

void Wait_ms(unsigned int count)
{
	while(count--)
	{
		TR2=1;
		TF2H=0;
		while(!TF2H);
		TF2H=0;
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

void FLASH_Write (unsigned int dest, char *src, unsigned int numbytes)
{
	xdata unsigned int i;

	for (i = dest; i < dest+numbytes; i++) 
	{
		FLASH_ByteWrite (i, *src++);
	}
}


void send(unsigned int i)
{
	sl('S');sl('E');sl('K');sl(':');
	sl(i%1000/100 + 0x30);
	sl(i%100/10 + 0x30);
	sl(i%10 + 0x30);
}

void send_str(unsigned char *ptr)
{
	while(*ptr) sl(*ptr++);
	sl(0x0d);
	sl(0x0a);
}

void sl(unsigned char c)
{
	SBUF0=c;
	while(!TI0);
	TI0=0;
}

char strn_cmp(char * str1, char *str2, unsigned char n)
{
	xdata unsigned char i;
	for(i=0; i<n; i++) if(str1[i] != str2[i]) return 0xaa;
	return 0;	
}