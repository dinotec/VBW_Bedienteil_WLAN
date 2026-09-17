#ifndef _CONSTDEF_
#define _CONSTDEF_

//~ #define OEM
//~ #define TEST

#include "c8051f340.h"                 // SFR declarations
#include "enumerate.h"                 
#include "MeldeListeDef.h"                 

// Gerätetyp 00502
// #define DEVICETYPEH 34	// 502/224 + 32
// #define DEVICETYPEL 86	// 502%224 + 32
// Gerätetyp 08580
//~ #define DEVICETYPEH 70	// 8580/224 + 32
//~ #define DEVICETYPEL 100	// 8580%224 + 32


#define TEST_MODE 567
#define OFFSET_EEPR 1024
#define ZEICHEN 60
#define ZEILEN 12

#define LED_PWM_R0 P4 &= ~0x10
#define LED_PWM_R1 P4 |=  0x10
#define LED_PWM_G0 P4 &= ~0x20
#define LED_PWM_G1 P4 |=  0x20
#define LED_PWM_B0 P4 &= ~0x40
#define LED_PWM_B1 P4 |=  0x40


// #define U4_min   116
// #define U4_max   232
// #define U4_def   174
//~ #define PH_STH_def 58.0
#define PH7 700
#define SAVE_FLAG 0xa5
//~ #define MV_PRO_PH 58


//------Durchfluß--------------------------
#define FL1 (3.6e6 / 2223.0)
//~ #define FL2 (3.6e6 / 795.0)
#define FL2 (3.6e6 / 754.0)
#define CELLEFILL_SOLE 730 // Zelle füllen Sole 0,73 S. Puls
#define BTR_VERZ 3
#define ZEHN_MIN 600
#define DREI_MIN 180 // 600 zu lang auf 180 Sek reduzieren

// --- Niveaumessung durch T1 -----
#define LEV_MIN 230
#define LEV_MAX 940
#define LEV_RUN 310
#define LEV_UNG 470


// status 
//~ sbit RUN            = stts.b[1]^0; // Anlage produziert   
//~ sbit SOLE_FIL       = stts.b[1]^1; // Soletank nachfüllen   
//~ sbit ANL_FIL        = stts.b[1]^2; // Anlage oder Zelle nachfüllen   
//~ sbit ENTHAERTER_REG = stts.b[1]^3; // Enthärter regeneriert		   
//~ sbit H2_ALRM        = stts.b[1]^4; // Wasserstoffalarm   
//~ sbit FLOW0_FAIL     = stts.b[1]^5; //    
//~ sbit POT_FAIL    = stts.b[0]^4; 		   

#define RUN            0x01
#define SOLE_FIL       0x02
#define ANL_FIL        0x04
#define ENTHAERTER_REG 0x08
#define H2_ALRM        0x10
#define EXT_STOP       0x20
#define SOLE_KONT      0x40
#define CELLE_FIL      0x80
#define FLOW0_FAIL     0x100
#define FLOW1_FAIL     0x200
#define BIN_TOP        0x400
#define BIN_BOTT       0x800

#define POT_FAIL       0x1000

#define ENTH_LED 0x08
#define RUN_LED  0x10
#define SOLE_LED 0x04
#define LED_AUS  0x00
#define MAX_T    900

#define ALARM_RST_BUS 54074

#define MENU_ATTR_ADR OFFSET_EEPR + 12

//~ #define i0 (lvar.tmp_c[0])
//~ #define i1 (lvar.tmp_c[1])
//~ #define i2 (lvar.tmp_c[2])
//~ #define int0 (lvar.tmp_ui[0])
//~ #define f0 (lvar.tmp_f[0])
//~ #define f1 (lvar.tmp_f[1])
//~ #define l0 (lvar.tmp_l[0])
//~ 
//~ #define arr_c (lvar.tmp_c)

// Softwarestand
// #define SOFTVER 510

// Zuordnungen Hardware
// sbit SENDLED = P2^6;
// sbit TREN = P2^7;		// Sendefreigabe RS-485				
// sbit TREN = P2^5;		// Sendefreigabe RS-485 	F410				
// sbit RXD = P0^5;		// UART Empfangspin
sbit TXD1          = P0^6;
sbit ALARM_LED     = P2^0;	// Alarm
//~ sbit PH_AUS_LED    = P2^1;	// Regelung pH
//~ sbit PUMPE_PH_LED  = P2^2;
//~ sbit DES_AUS_LED   = P2^3;	// Regelung Desinfektion
//~ sbit PUMPE_DES_LED = P2^4;
sbit PH_AUS_LED    = P2^3;	// Regelung pH
sbit PUMPE_PH_LED  = P2^4;
sbit DES_AUS_LED   = P2^1;	// Regelung Desinfektion
sbit PUMPE_DES_LED = P2^2;
//~ sbit LED5          = P2^5;
sbit LED_FLOW      = P1^1;
sbit ON_OFF_LED    = P2^6; // ON-OFF
sbit WD_TOOGLE     = P2^7;

sbit UC_1608       = P2^5; // Display mit UC1608-Kontroller


sbit TREN0 = P3^0;
sbit TREN1 = P3^1;
// Display
// sbit A0    = P1^6;
// sbit SCL   = P1^4;
// sbit SDA   = P1^3;
// sbit IRS   = P1^2;
// sbit RESET = P0^7;
// sbit CSA   = P0^6;
// sbit CSB   = P0^2;
// Hinterleuchtung
// sbit BCKL_PWM = P1^7;
sbit SCL       = P3^2;
sbit SDA       = P3^3;
sbit WR_EN     = P3^4;
// sbit SDA     = P3^3;
// sbit SCL     = P3^2;
// sbit SCL_RTC = P3^5;       // IIC-Bus PCF8563
// sbit SDA_RTC = P3^6;		// IIC-Bus	PCF8563
// sbit GW_ON   = P3^7;
#define EMPBUFLEN 80

#define BLOCK_LEN 128
typedef struct SEND_BUF1
{
	unsigned char buf[BLOCK_LEN];
	unsigned char cnt;
} SEND_BUF1;

typedef struct EMPF_BUF1
{
	unsigned char buf[BLOCK_LEN];
	unsigned char cnt;
} EMPF_BUF1;

#define WIDTH 240
#define HEIGHT 64
#define DIM_1  (HEIGHT / 8) 
#define DIM_16 (HEIGHT / 4)
#define LEN_60 (WIDTH  / 4)
#define LENGTH (WIDTH * DIM1)
#define BUF_LEN 256

/*
char data * xdata str;         //  ptr in xdata to data char 
int xdata * data numtab;      // ptr in data to xdata int 
long code * idata powtab;     //  ptr in idata to code long

typedef union RRAM {
	unsigned char vec[HEIGHT/8][WIDTH];
	unsigned char buf_tmp[256];
	unsigned char buf_2[(HEIGHT/8)*WIDTH];
} RRAM;

typedef union RRAM {
	signed char       vec[DIM1][WIDTH];
	unsigned char buf_tmp[DIM1*2] [WIDTH / 2];
	unsigned int   buf_ui[DIM1][WIDTH / 2];
	int             buf_i[DIM1][WIDTH / 2];
	unsigned long   buf_l[DIM1][WIDTH / 4];
	float           buf_f[DIM1][WIDTH / 4];
	
} RRAM;
*/

typedef union RRAM {
	char vec[DIM_1][WIDTH];
	
	unsigned char tmp_buf[BUF_LEN]; //  0 .. 15
	//~ unsigned char tmp_buf[32][60]; //  0 .. 15
	//~ unsigned int   ui_buf[16][60]; //  8
	//~ int             i_buf[16][60]; //  9
	//~ unsigned long   l_buf[ 8][60]; //  6 
	//~ float           f_buf[ 8][60]; //  7		
} RRAM;


typedef union UINT { // byte-addressable int
  unsigned char b[2];
  int i;
  unsigned int ui;
} UINT;

typedef union FLOAT                    // byte-addressable FLOAT
{
  unsigned char b[4];
  UINT vui[2];
  float f;
  unsigned long ul;
} FLOAT;

typedef struct WERTE {
	int dflt;
	int min;
	int max;
} WERTE;

typedef struct MSR_RL {
	char ind;
	char prc;
	char dos_EEPR;
	unsigned int dos_zeit;
	unsigned int dos_zeit_max;
	unsigned int ddos;
	unsigned char dos_ovfl_txt;
	unsigned char ddos_txt;
} MSR_RL;

#define STACK_LAENGE 5
typedef struct menustack  {
	unsigned char  menu_nr[STACK_LAENGE];
	unsigned int  txt_nr[STACK_LAENGE];
	unsigned char  pos[STACK_LAENGE];
	unsigned char size;
} menustack;
// xdata struct var_act
// {
	// int   nr[8];
	// char pos[8];
// } vr;

#define STATUS_LEN 20
typedef struct STATUSZEILE {
	unsigned int alarm[STATUS_LEN];
	unsigned int  warn[STATUS_LEN];
	unsigned char ctr_alr;
	unsigned char ctr_warn;
	// unsigned char ctr;
} STATUSZEILE;

#define SBUF_LAENGE 128
typedef struct EMPF_BUF {
	unsigned char buf[SBUF_LAENGE];
	unsigned char cnt;
} EMPF_BUF;

typedef struct DATETIME {
	unsigned char std;
	unsigned char min;
	unsigned char sek;
	unsigned char tag;
	unsigned char monat;
	unsigned char jahr;
	unsigned char wtag;
} DATETIME;

typedef struct var_act
{
	int   nr[8];
	char pos[8];
} var_act;

//~ typedef struct Kalibrierung {
	//~ int val;
	//~ int  sth;
	//~ int mv_in;
	//~ int dpd;
	//~ unsigned char rst_txt;
	//~ unsigned char aer_min;
	//~ void (*fp_sth)(void);
//~ } Kalibrierung;

#endif
