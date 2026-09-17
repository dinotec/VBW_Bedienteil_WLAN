#include "ConstDef.h"                 // SFR declarations
extern xdata unsigned char vec_sz[512];
extern xdata unsigned int cnt_sz;

void FLASH_PageErase (unsigned int addr)
{
	bit EA_SAVE = EA;                   // Preserve EA
	char xdata * xdata pwrite;           // FLASH write pointer

	EA = 0;                             // Disable interrupts

	VDM0CN = 0x80;                      // Enable VDD monitor
	RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
	
	//addr = 0xc800;
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
	xdata unsigned int 	i,
						cntr;
	xdata unsigned char c;
	cntr = dest + numbytes;
	for (i = dest; i < cntr; i++) 
	{
		c = *src;
		FLASH_ByteWrite (i, *src++);
	}
}


void Wait_ms(unsigned int count)
{
	while(count--)
	{
		TR0 = 1;
		while(!TF0);
		TF0 = 0;
		WD_TOOGLE = 1;
		WD_TOOGLE = 0;
		PCA0CPH4 = 255; 

	}
}

void flash_update(void)
{
	FLASH_PageErase(0xf300);
	Wait_ms(50);
	FLASH_Write(0xf300, vec_sz + 2, cnt_sz - 2); 
}