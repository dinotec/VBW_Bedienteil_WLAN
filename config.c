/////////////////////////////////////
//  Generated Initialization File  //
/////////////////////////////////////

#include "C8051F340.h"

// #define UART_1
// Peripheral specific initialization functions
void Reset_Sources_Init()
{
    int i = 0;
    VDM0CN    = 0x80;
    for (i = 0; i < 350; i++);  // Wait 100us for initialization
    RSTSRC    = 0x02;
}

void PCA_Init()
{
	PCA0CN    = 0x40;
    PCA0MD   &= ~0x40;
    PCA0MD    = 0x00;
    PCA0CPL4  = 0xE9;
    PCA0CPH4 = 255;
	PCA0MD   |= 0x20;
}

// void Timer_Init()
// {
	// TCON      = 0x50;
    // TMOD      = 0x22;
    // CKCON     = 0x0A;
    // TH0       = 0x06;
	// TH1       = 0x98; // 57600 baud
// }

// void UART_Init()
// {
    // SCON0     = 0x30;
// }

//------------------
void Timer_Init()
{
    TMOD      = 0x21;
	CKCON     = 0x01; // System clock divided by 4	
    // TH0       = 0x06;
    // TH0       = 0x8a;
    // TL0       = 0xd0;
    TH0       = 0xf4;
    TL0       = 0x48;
	TH1       = 0x64; // com0 - 9600 baud
	TR0       = 1;
	TR1       = 1;
}

void UART_Init()
{
    SCON0     = 0x30;
	SBRLL1    = 0x8F; // 9600 Baud
    SBRLH1    = 0xFD;
	SCON1     = 0x10; //	UART1 reception enabled
    SBCON1    = 0x43;
}

//------------------


void SPI_Init()
{
    SPI0CFG   = 0x70;
// #ifndef UART_1
	SPI0CN    = 0x01;  // sonst UART1 funktioniert nicht
// #endif
	SPI0CKR   = 0x01;
}

void Port_IO_Init()
{

    P0MDOUT   = 0x05;
	P2MDOUT   = 0x80;
	// P3MDOUT   = 0x81;
	P3MDOUT   = 0xe1;
	// P4MDOUT   = 0x7F;
	P4MDOUT   = 0x7c; // 0x74;
	P0SKIP    = 0x08; // wegen SPI 4. Pin
	XBR0      = 0x03; // SPI I/O, UART0 I/O Enable
	XBR1      = 0x40; // Crossbar enabled
	XBR2      = 0x01; // 1: UART1 TX1, RX1 routed to Port pins.
}

void Oscillator_Init()
{
	OSCICN    = 0x83; // Internal H-F Oscillator Enabled, SYSCLK derived from Internal H-F Oscillator divided by 1.
}

void Interrupts_Init()
{
	EIE2      = 0x02; // UART1 interrupt enabled
	IP        = 0x10; //UART0 interrupts set to high priority level.
	//~ EIP2        = 0x02; //UART1 interrupts set to high priority level.
	IE        = 0x92;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    PCA_Init();
    Reset_Sources_Init();
    Timer_Init();
    UART_Init();
    SPI_Init();
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
}
