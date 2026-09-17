#include <string.h>
#include <stdlib.h>
#include "ConstDef.h"
#include "eeprom_128drv.h"
#include "eeprom128.h"
//~ #include "bus_register.h"
#include "menu_lcd.h"
#include "main.h"
#include "default_werte_ely.h"

#define BK_OFFSET 0x300
//~ #define MBUS_READ  0x04
//~ #define MBUS_WRITE 0x06

// #define GW_ON(B) (B) ? (P4 |= 0x08) : (P4 &= 0xf7);

void speichern_int(unsigned int u);
bit WRITEREG(unsigned char REG, unsigned int wr);

extern code unsigned char id[];
extern code WERTE tab1[];
extern code unsigned char reg[];


xdata EMPF_BUF1 sbuf1,
                mbbuf;


xdata unsigned char bus_free,
					ret_adr;
bit send_sts, adr_p19, mod_bus;
	// EE_BVSE;				// Freigabe erteilen

void com1_ini(void) {
	TREN1 = 0;
	//~ P4 |= 0x04; // GW_ON   = 1;
	P4 &= ~0x04; // GW_ON   = 1;
	memset(&sbuf1,0,sizeof(sbuf1));
	memset(&mbbuf,0,sizeof(mbbuf));
	bus_free = 0;
	send_sts = 0;
	adr_p19  = 0;
	mod_bus  = 0;
	// EE_BVSE = 0;				// Freigabe erteilen
	vek[wrteptr_ix].ui = 0;
}

void ident_daten(unsigned char* ptr) {
	#define DEVICETYPE (atoi(id))
	#define DEVICETYPEH ((DEVICETYPE / 224) + 32)
	#define DEVICETYPEL ((DEVICETYPE % 224) + 32)
	*ptr        = DEVICETYPEH;
	*(ptr + 1)  = DEVICETYPEL;
	//~ *(ptr + 2)  = vek[lng_adr2].b[1];
	//~ *(ptr + 3)  = vek[lng_adr1].b[1];
	//~ *(ptr + 4)  = vek[lng_adr0].b[1];
	*(ptr + 2)  = 0x30;
	*(ptr + 3)  = (vek[dev_nr_ix].b[0] >= 0x20) ? (vek[dev_nr_ix].b[0]) : (vek[dev_nr_ix].b[0]+0x20);
	*(ptr + 4)  = (vek[dev_nr_ix].b[1] >= 0x20) ? (vek[dev_nr_ix].b[1]) : (vek[dev_nr_ix].b[1]+0x20);
}

bit CHKWC(unsigned char val,unsigned char cmp) {	
	#define WILDCARD 0x17
	if (val == WILDCARD || val == cmp) 
		return 1;
	else
		return 0;
}

void send_byte(unsigned char b) {
	TREN1 = 1;
	while(!(SCON1 & 0x20)) ;
	SBUF1 = b;
}

unsigned int reg_fn(unsigned char c) {
	switch(reg[c]) {
		case bus_strom_prc:
			return prc_fn(vek[el_A_mv_ix].ui, werte_el[i_soll][vek[anl_typ_t].b[1]-a30_txt]);
			break;
		case bus_flow0_prc:
			return prc_fn(vek[flow0_soll_ix].ui, vek[flow0_ix].ui);
			break;
		case stat_1:
			return STATUS1.ui;
			break;
		case stat_ely:
			return STATUS_ELY.ui;	
		default:
			return vek[reg[c]].ui;
			break;
	}
}

void UART1_ISR (void) interrupt 16 {
	bus_free = 0;
	if(SCON1 & 0x01) {
		unsigned char c;
		SCON1 &= ~0x01;
		c = SBUF1;
		if(SCON1 & 0x80) {	// Receive FIFO Overrun Flag
			memset(&sbuf1,0,sizeof(sbuf1));
			SCON1 &= ~0x80;
			return;
		}
		// Modbus
		mbbuf.buf[mbbuf.cnt] = c;
		
		if(++mbbuf.cnt >= sizeof(mbbuf.buf))
			mbbuf.cnt = 0;
		// Modbus Ende
		
		if(c > 0 && c != 0x1f) {
			sbuf1.buf[sbuf1.cnt] = c;
			if(sbuf1.buf[sbuf1.cnt] == 0x0d)
			{
				unsigned char sum;
				for(c = 0, sum = 0; c < sbuf1.cnt - 1; ++c)
					sum += sbuf1.buf[c];
				sum |= 0x20;
				if(sum == sbuf1.buf[sbuf1.cnt - 1]) {	
					if(sbuf1.buf[0] == 0x08) { // 1.2.13	Kurzadresse zuweisen
						unsigned char ident[5];
						ident_daten(ident);
						if(!memcmp(ident, sbuf1.buf+1,5)) {
							vek[sh_adr].ui = sbuf1.buf[6] - 0x20;
							//~ vek[sh_adr].ui = sbuf1.buf[6] ;
							// ix = vek[sh_adr].ui;
							speichern_int(sh_adr);
							memset(&sbuf1,0,sizeof(sbuf1));
							send_byte(0x0d);
						}
					} else if(sbuf1.buf[0] == 0x09) { // Statusmeldung
						unsigned char ident[5];
						ident_daten(ident);
						for(c = 1; c < 6; ++c)
							if(!CHKWC(sbuf1.buf[c], ident[c - 1]))
								break;
						if(c == 6) {
							send_sts = 1;
							ret_adr = sbuf1.buf[6];
						}
					} else if(sbuf1.buf[1] == (vek[sh_adr].ui + 0x20)) {
						switch(sbuf1.buf[0]) {
							case 0x11: // Register auslesen
								if(sbuf1.buf[2] >= 0x20) {	
									xdata unsigned int w;
									c = sbuf1.buf[2] - 0x20;
									memset(&sbuf1,0,sizeof(sbuf1));
									if(c == 180)
										w = KEY;
									else if(c >= 192 && c <= 207) {
										adr_128k = BK_OFFSET + reg[c];
										w = ee_rdb_128k();
									} else
										//~ w = vek[reg[c]].ui;
										w = reg_fn(c);
									sbuf1.buf[3] = 0x30 + ( w        & 0x0f);
									sbuf1.buf[2] = 0x30 + ((w >>  4) & 0x0f);
									sbuf1.buf[1] = 0x30 + ((w >>  8) & 0x0f);
									sbuf1.buf[0] = 0x30 + ((w >> 12) & 0x0f);
									for(c = 0; c < 4; ++c)
										sbuf1.buf[4] += sbuf1.buf[c]; 		
									sbuf1.buf[4] |= 0x20;
									sbuf1.buf[5]  = 0x0d;
									send_byte(sbuf1.buf[0]);
								}
							break;	
							case 0x12: // 1.2.9	Registerinhalt verändern
								if(sbuf1.buf[2] >= 0x20) {	
									xdata UINT reg_data;
									c = sbuf1.buf[2] - 0x20;
									reg_data.ui   = sbuf1.buf[3] & 0x0f;
									reg_data.ui <<= 4;
									reg_data.ui  |= sbuf1.buf[4] & 0x0f;
									reg_data.ui <<= 4;
									reg_data.ui  |= sbuf1.buf[5] & 0x0f;
									reg_data.ui <<= 4;
									reg_data.ui  |= sbuf1.buf[6] & 0x0f;
									memset(&sbuf1,0,sizeof(sbuf1));
									if(WRITEREG(c, reg_data.ui))
										send_byte(0x0d);
									else	
										memset(&sbuf1,0,sizeof(sbuf1));
								} else
									memset(&sbuf1,0,sizeof(sbuf1));
							break;	
							case 0x14: // 1.2.11	Datentransfer
							break;
							default:
								memset(&sbuf1,0,sizeof(sbuf1));
							break;	
						}
					} else
						memset(&sbuf1,0,sizeof(sbuf1));
				} else {
					// P19 Protokol
					if(sbuf1.cnt > 2) {
						sum = sbuf1.buf[sbuf1.cnt - 2] & 0x0f;
						sum <<= 4; 
						sum |= (sbuf1.buf[sbuf1.cnt - 1] & 0x0f);
						if(sbuf1.buf[sbuf1.cnt - 3] == 0x0a) { //1.1.1	Gerät anwählen
							if(sum == vek[sh_adr].ui) {
								adr_p19 = 1;
								sbuf1.buf[0] = sbuf1.buf[sbuf1.cnt - 2];
								sbuf1.buf[1] = sbuf1.buf[sbuf1.cnt - 1];
								sbuf1.buf[2] = 0x30;
								sbuf1.buf[3] = 0x30;
								sbuf1.buf[4] = 0x0d;
								sbuf1.buf[5] = 0;
								sbuf1.cnt    = 0;
								send_byte(sbuf1.buf[0]);
							} else {
								memset(&sbuf1,0,sizeof(sbuf1));
								adr_p19 = 0;
							}		
						} else if(sbuf1.buf[sbuf1.cnt - 3] == 0x24) { // Register auslesen
							if(adr_p19) {
								xdata unsigned char offset;
								//~ offset = offst();
								memset(&sbuf1,0,sizeof(sbuf1));
								//if(sum == 7)
								//	vek[wrteptr].ui = (int)COLLECT_ALR;
								//else
									vek[wrteptr_ix].ui = vek[reg[sum + offset]].ui;
								sbuf1.buf[0] = 0x30	+ (vek[wrteptr_ix].b[0] >> 4);
								sbuf1.buf[1] = 0x30	+ (vek[wrteptr_ix].b[0] & 0x0f);
								sbuf1.buf[2] = 0x30	+ (vek[wrteptr_ix].b[1] >> 4);
								sbuf1.buf[3] = 0x30	+ (vek[wrteptr_ix].b[1] & 0x0f);
								sbuf1.buf[4] = 0x0d;
								vek[wrteptr_ix].ui = 0;
								send_byte(sbuf1.buf[0]);	
							}	
						}
					} else
						memset(&sbuf1,0,sizeof(sbuf1));
				}	
			} else {	
				if(++sbuf1.cnt >= sizeof(sbuf1.buf))
					sbuf1.cnt = 0;
			}
		} else
			memset(&sbuf1,0,sizeof(sbuf1));
	} else {
		SCON1 &= ~0x02;
		if(mod_bus) {
			if(++sbuf1.cnt < mbbuf.cnt) {
				WDT_RT();
				while(!(SCON1 & 0x20)) ;
				SBUF1 = sbuf1.buf[sbuf1.cnt];
			} else {	
				TREN1 = 0;
				memset(&sbuf1,0,sizeof(sbuf1));
				memset(&mbbuf,0,sizeof(mbbuf));
				mod_bus = 0;
			}
		} else {
			if(sbuf1.buf[++sbuf1.cnt] > 0) {	
				while(!(SCON1 & 0x20)) ;
				SBUF1 = sbuf1.buf[sbuf1.cnt];
			} else {	
				TREN1 = 0;
				memset(&sbuf1,0,sizeof(sbuf1));
			}
		}
	}
}	


void send_status(void) {
	// unsigned char n;
	memset(&sbuf1,0,sizeof(sbuf1));
	sbuf1.buf[1] = 0x15;
	sbuf1.buf[2] = ret_adr;
	ident_daten(sbuf1.buf+3);
	sbuf1.buf[8] = vek[sh_adr].ui + 0x20;
	//~ strncat(sbuf1.buf + 1, id + 5, 16);
	strncat(sbuf1.buf + 9, id + 5, 16);
	// strncat(sbuf1.buf + 1, "pcd, Testversion",16);
	adr_128k = BK_OFFSET;
	len_128k = 16;
	ptr_128k = sbuf1.buf + 25;
	ee_rdseq_128k();
	// for(n = 0; n < 16; ++n)
		// sbuf1.buf[25+n] = ee_rdb_128k();
		
	sbuf1.buf[41] = 0x16;
	for(sbuf1.cnt = 0; sbuf1.cnt < 42; ++sbuf1.cnt) { 
		if(sbuf1.cnt && !sbuf1.buf[sbuf1.cnt])
			sbuf1.buf[sbuf1.cnt] = 0x20;
		sbuf1.buf[42] += sbuf1.buf[sbuf1.cnt];
	}
	sbuf1.buf[42] |= 0x20;
	sbuf1.buf[43]  = 0x0d;
	sbuf1.buf[44]  = 0x1f;
	sbuf1.cnt = 0;
	// TREN1 = 1;
	// while(!(SCON1 & 0x20)) ;
	// SBUF1 = 0;
	send_byte(0);
}

code unsigned char wr_reg[] = {cl_alr_ob, cl_wr_ob,cl_soll,cl_wr_un,cl_alr_un,cl_p,cl_i,
							   ph_alr_ob,ph_wr_ob,ph_soll,ph_wr_un,ph_alr_un,ph_p,ph_zi,
							   rx_soll,rx_wr_un,rx_alr_un,rx_p};

bit WRITEREG(unsigned char REG, unsigned int wr) {
	bit b;
 	b = 0;
	if(REG==223)
		vek[wrteptr_ix].b[1] = wr, b = 1;
	else {	
		if(vek[wrteptr_ix].b[1] == REG) {
			xdata unsigned char i;
			for(i = 0; i < sizeof(wr_reg); ++i) {
				if(reg[REG] == wr_reg[i])
					break;
			}
			if(i < sizeof(wr_reg)) {
					if(wr >= tab1[reg[REG]].min && wr <= tab1[reg[REG]].max) {	
						if(vek[reg[REG]].ui != wr) {
							vek[reg[REG]].ui = wr;
							speichern_int(reg[REG]);
						}
						b = 1;
					}
			} 
				vek[wrteptr_ix].b[1] = 0;
		} 
	}
	return b;	
}

