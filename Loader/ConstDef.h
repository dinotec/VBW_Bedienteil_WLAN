#ifndef _CONSTDEF_
#define _CONSTDEF_

#include "c8051f340.h"                 // SFR declarations
#include "bilder.h"
#define ADR_L 0xa0 
#define ADR_H 0xa2

#define ADR_L_UPD 0xa4 
#define ADR_H_UPD 0xa6

#define BUF_LEN 256
#define WIDTH 240
#define HEIGHT 64

#define OFF_ID    0x40 // Offset für Versionerkennung
#define MAX_LEN_FLASH 0xe400 // 
#define MAX_LEN_MENU 0x1fbfe // 0x20000 - 0x400 - 2 = 0x1fc00 : dec = 130046 -2 Byte für CRC, 1024 Byte für interne Nutzung
 
// typedef union UINT
// {
	// unsigned int ui;
	// int i;
	// unsigned char b[2];
// } UINT;
typedef union ULONG
{
	unsigned long ul;
	unsigned char b[4];
} ULONG;

typedef union RW_BUF
{
	unsigned char  flash_buf[512];
	unsigned char  eepr_r[4][128]; // 64 kB EEPROM
	unsigned char eepr_wr[2][256]; // 128 kB EEPROM
} RW_BUF;

typedef union RRAM
{
	unsigned char vec[HEIGHT/8][WIDTH];
	unsigned char buf_tmp[256];
	unsigned char buf_2[(HEIGHT/8)*WIDTH];
} RRAM;


sbit SCL       = P3^2;
sbit SDA       = P3^3;
sbit WR_EN     = P3^4;
sbit WD_TOOGLE = P2^7; 
sbit ON_OFF_LED = P2^6; // ON-OFF

#endif