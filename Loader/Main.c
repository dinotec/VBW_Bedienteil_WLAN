#include <string.h>
#include "ConstDef.h"
#include "Display.h"
#include "Read_AT24C1024.h"

// extern void Wait_ms(unsigned int count);
extern void update(void);
void Init_Device(void);

//xdata char reserved_memory_bank[700] _at_ 0x00;
//Linker: RS(256) PL(68) PW(78) IX 
// CODE(0xf800) 
void (*fptr)(void);						
extern xdata unsigned char txt_cnt;

// unsigned char vek[10],
				  // i;

void main(void) 
{
	fptr = (void code *)0x100 ;
	Init_Device();
	P2 &= 0x80;
	P1 &= ~0x02;
	display_reset();
	memset(rram.vec[0],0,sizeof(rram.vec[0]) * 8);
	txt_cnt = 255;
	message(boot_txt,0);
	pause(50);
	update();
	message(start_txt,0);
	pause(100);
	P2 |= 0x7f; 
	P1 |= 0x02;
	fptr();
}

/*
// Peripheral specific initialization functions,
// Called from the Init_Device() function
void Timer_Init()
{
    TCON      = 0x40;
    TMOD      = 0x20;
    CKCON     = 0x01;
    TH1       = 0x64;
    //TMR2CN    = 0x04;
    TMR2RLL   = 0x17;
    TMR2RLH   = 0xFC;
}

// void Timer_Init()
// {
    // TMOD      = 0x20;
    // CKCON     = 0x01;
    // TH1       = 0x64;
// }

 
void Port_IO_Init()
{
    // P0.0  -  CEX0  (PCA), Open-Drain, Digital
    // P0.1  -  CEX1  (PCA), Open-Drain, Digital
    // P0.2  -  Unassigned,  Open-Drain, Digital
    // P0.3  -  Unassigned,  Open-Drain, Digital
    // P0.4  -  TX0 (UART0), Open-Drain, Digital
    // P0.5  -  RX0 (UART0), Open-Drain, Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Open-Drain, Digital

    // P1.0  -  Unassigned,  Open-Drain, Digital
    // P1.1  -  Unassigned,  Open-Drain, Digital
    // P1.2  -  Unassigned,  Open-Drain, Digital
    // P1.3  -  Unassigned,  Open-Drain, Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  Unassigned,  Open-Drain, Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital

    // P2.0  -  Unassigned,  Open-Drain, Digital
    // P2.1  -  Unassigned,  Open-Drain, Digital
    // P2.2  -  Unassigned,  Open-Drain, Digital
    // P2.3  -  Unassigned,  Open-Drain, Analog
    // P2.4  -  Unassigned,  Open-Drain, Digital
    // P2.5  -  Unassigned,  Open-Drain, Analog
    // P2.6  -  Unassigned,  Open-Drain, Analog
    // P2.7  -  Unassigned,  Push-Pull,  Digital

    // P3.0  -  Unassigned,  Open-Drain, Digital
    // P3.1  -  Unassigned,  Open-Drain, Digital
    // P3.2  -  Unassigned,  Open-Drain, Digital
    // P3.3  -  Unassigned,  Open-Drain, Digital
    // P3.4  -  Unassigned,  Open-Drain, Digital
    // P3.5  -  Unassigned,  Open-Drain, Digital
    // P3.6  -  Unassigned,  Open-Drain, Digital
    // P3.7  -  Unassigned,  Open-Drain, Digital

    P2MDIN    = 0x97;
    P2MDOUT   = 0x80;
	//P2 = 0;
    // XBR0      = 0x01;
    // XBR1      = 0x42;
    XBR0      = 0x01;
    XBR1      = 0x40;
}

void Oscillator_Init()
{
    OSCICN    = 0x83; // SYSCLK=12 Mhz
    //OSCICN    = 0x81; // SYSCLK=12/4 Mhz
    //OSCLCN    |= 0x80;
}

void PCA_Init()
{
    PCA0MD    &= ~0x40;
    PCA0MD    = 0x00;
}

void UART_Init()
{
    SCON0     = 0x10;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    PCA_Init();
	Timer_Init();
    //SPI_Init1();
	UART_Init();
    Port_IO_Init();
    Oscillator_Init();
	P2 |=  0x80;
	P2 &= ~0x80;
}

// void UART0_ISR (void) interrupt 4
// {
	// RI0 = 0;
	// vek[i] = SBUF0;
	// if(vek[i] == 13) 
	// {
		// if(vek[i - 2] == vek[i - 3])
		// {
			// EA = 0;
			// fptr();
		// }	
		// i = 0;
	// }
	// else 
		// i++;
// }

// void UART0_ISR (void)
// {
	// unsigned int cnt = 0;
	// unsigned char i = 0;
	
	// while(1)
	// {
		// RI0 = 0;
		// while(!RI0)
		// {
			// Wait_ms(1);
			// cnt++;
			// if(cnt >= 3000) 
				// return;
		// }
		// RI0 = 0;
		// vek[i] = SBUF0;
		// if(vek[i] == 13) 
		// {
			// if(vek[i - 2] == vek[i - 3])
			// {
				// fptr();
			// }
		// }		
		// else 
		// {
			// i++;
			// i %= 10; 
		// }
	// }
// }

*/