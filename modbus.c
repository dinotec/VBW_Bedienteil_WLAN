#include <string.h>
#include <stdlib.h>
#include "ConstDef.h"
#include "eeprom128.h"
#include "eeprom_128drv.h"
#include "main.h"
#include "menu_lcd.h"
#include "bus.h"


#define MBUS_READ  0x04
#define MBUS_WRITE 0x06
#define REG_MAX 22	// maximal 22 Registern auf einmal lesen
#define ADR_MAX 108	// maximale Adresse

xdata UINT CRC;
extern code unsigned char reg[];
extern code WERTE tab1[];
extern code unsigned char id[];

xdata unsigned char bus_free,
					ret_adr;
//bit send_sts, mod_bus;

code unsigned char wr_reg[] = {cl_alr_ob, cl_wr_ob,cl_soll,cl_wr_un,cl_alr_un,cl_p,cl_i,
							   ph_alr_ob,ph_wr_ob,ph_soll,ph_wr_un,ph_alr_un,ph_p,ph_zi,
							   rx_soll,rx_wr_un,rx_alr_un,rx_p};

bit WRITEREG(unsigned char REG, unsigned int wr);

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


// CRC-Prüfsumme berechnen ("inverse" Berechnung speziell für Modbus)
// Startwert ist 0xFFFF
void calc_crc(unsigned char v) {
	unsigned char n;

	CRC.ui ^= v;				
	
	for (n = 0; n < 8; ++n)	{
		if (CRC.ui & 1) {
			CRC.ui >>= 1;
			CRC.ui ^= 0xA001;			// Generatorpolynom
		}
		else CRC.ui >>= 1;
	}
}	
//~ 04 (0x04) Read Input Registers 
//~ Function code 1 Byte 0x04 
//~ Starting Address 2 Bytes 0x0000 to 0xFFFF 
//~ Quantity of Input Registers 2 Bytes 0x0001 to 0x007D 
//~ CRC 2 bytes
void modbus_rt(void) {
	if(empf1.cnt >= 8) {
		xdata char ix, err;
		
		CRC.ui = 0xffff;
		for(ix = 0; ix < 6; ++ix)
			calc_crc(empf1.buf[ix]);
		if(CRC.b[0] == empf1.buf[7] && CRC.b[1] == empf1.buf[6]) {
			if(empf1.buf[0] == vek[sh_adr].ui) {
				if(empf1.buf[1] == MBUS_READ) {
					xdata UINT adr, anz;
					adr.b[0] = empf1.buf[2];
					adr.b[1] = empf1.buf[3];
					anz.b[0] = empf1.buf[4];
					anz.b[1] = empf1.buf[5];
					err = 0;
					if(anz.ui > REG_MAX || adr.ui > ADR_MAX)
						err = 0x02;
					//~ else if((anz.ui + adr.ui) > (REG_MAX + 1))
						//~ err = 0x02;
					else if((anz.ui + adr.ui) > ADR_MAX+1)
						err = 0x02;
					else {
						empf1.buf[2] = 2*anz.ui; 
						for(ix = 0; ix < anz.ui; ++ix, ++adr.ui) {
							//~ if(adr.ui == 30) {
								//~ vek[wrteptr].ui = MB_STATUS.ui;
							//~ }
							//~ else
								//~ vek[wrteptr].ui = vek[reg[adr.ui + offst()]].ui;
								vek[wrteptr_ix].ui = reg_fn(adr.ui);
							empf1.buf[3+ix*2] = vek[wrteptr_ix].b[0];
							empf1.buf[4+ix*2] = vek[wrteptr_ix].b[1];
						}
						vek[wrteptr_ix].ui = 0;
						CRC.ui = 0xffff;
						anz.ui *= 2;
						anz.ui += 2;
						++anz.i; 
						for(ix = 0; ix < anz.ui; ++ix)
							calc_crc(empf1.buf[ix]);
						empf1.buf[  anz.ui] = CRC.b[1];
						empf1.buf[++anz.ui] = CRC.b[0];
						empf1.cnt = ++anz.ui;
					}
				}
				else if(empf1.buf[1] == MBUS_WRITE) {
					xdata UINT adr, val;
					adr.b[0] = empf1.buf[2];
					adr.b[1] = empf1.buf[3];
					val.b[0] = empf1.buf[4];
					val.b[1] = empf1.buf[5];
					err = 0;
					
					if(adr.ui >= 8 && adr.ui <= 25) {
						xdata unsigned int var;
						var = reg[adr.ui];
						//~ var = reg_fn(adr.ui);
						if(val.i >= tab1[var].min && val.i <= tab1[var].max) {
							vek[var].i = val.i;
							speichern_int(var);
							empf1.cnt = 8;
						}
						else
							err = 0x03;		
					} else if(adr.ui == 26) {
						if(val.ui == ALARM_RST_BUS) {
							reset_fn();
						}
					} else 
						err = 0x03;
					
				}
				else
					err = 0x01;
				if(err) {
					empf1.buf[1] |= 0x80; 
					empf1.buf[2]  = err;
					CRC.ui = 0xffff;
					for(ix = 0; ix < 3; ++ix)
						calc_crc(empf1.buf[ix]);
					empf1.buf[3]  = CRC.b[1];
					empf1.buf[4]  = CRC.b[0];
					empf1.cnt     = 5;
				}
				memcpy(&send1, &empf1, sizeof(empf1));
				mod_bus = 1;
				send1.cnt = 0;
				send_byte(send1.buf[0]);
			}
		} else {
			xdata unsigned char sum,c,a,d;
			a = 0;
			d = 0;
			for(c = 0; c < empf1.cnt; ++c) {
				if(empf1.buf[c] == 0 || empf1.buf[c] == 0x1f)
					++a;
				if(empf1.buf[c] == 0x0d) {
					d = c;
					break;
				}
			}
			if(d > 0)
				for(c = a, sum = 0; c < d - 1; ++c)
					sum += empf1.buf[c];
			sum |= 0x20;
			if(sum == empf1.buf[d - 1]) {
				if(empf1.buf[a] == 0x08) { // 1.2.13	Kurzadresse zuweisen
					unsigned char ident[5];
					ident_daten(ident);
					if(!memcmp(ident, empf1.buf + a + 1, 5)) {
						vek[sh_adr].ui = empf1.buf[a + 6] - 0x20;
						//~ vek[sh_adr].ui = empf1.buf[6] ;
						// ix = vek[sh_adr].ui;
						speichern_int(sh_adr);
						memset(&empf1, 0, sizeof(empf1));
						send_byte(0x0d);
					}
				} else if(empf1.buf[a] == 0x09) { // Statusmeldung
					unsigned char ident[5];
					ident_daten(ident);
					for(c = a + 1; c < a + 6; ++c)
						if(!CHKWC(empf1.buf[c], ident[c - 1]))
							break;
					if(c == a + 6) {
						send_sts = 1;
						ret_adr = empf1.buf[a + 6];
					}
				} else if(empf1.buf[a + 1] == (vek[sh_adr].ui + 0x20)) {
					switch(empf1.buf[a]) {
						case 0x11: // Register auslesen
							if(empf1.buf[a + 2] >= 0x20) {	
								xdata unsigned int w;
								c = empf1.buf[a + 2] - 0x20;
								memset(&send1,0,sizeof(send1));
								if(c == 180)
									w = KEY;
								else if(c >= 192 && c <= 207) {
									#define BK_OFFSET 0x300
									adr_128k = BK_OFFSET + reg[c];
									w = ee_rdb_128k();
								} else
									//~ w = vek[reg[c]].ui;
									w = reg_fn(c);
								send1.buf[3] = 0x30 + ( w        & 0x0f);
								send1.buf[2] = 0x30 + ((w >>  4) & 0x0f);
								send1.buf[1] = 0x30 + ((w >>  8) & 0x0f);
								send1.buf[0] = 0x30 + ((w >> 12) & 0x0f);
								for(c = 0; c < 4; ++c)
									send1.buf[4] += send1.buf[c]; 		
								send1.buf[4] |= 0x20;
								send1.buf[5]  = 0x0d;
								empf1.cnt = 6;
								send_byte(send1.buf[0]);
							}
						break;	
						case 0x12: // 1.2.9	Registerinhalt verändern
							if(empf1.buf[a + 2] >= 0x20) {	
								xdata UINT reg_data;
								c = empf1.buf[a + 2] - 0x20;
								reg_data.ui   = empf1.buf[a + 3] & 0x0f;
								reg_data.ui <<= 4;
								reg_data.ui  |= empf1.buf[a + 4] & 0x0f;
								reg_data.ui <<= 4;
								reg_data.ui  |= empf1.buf[a + 5] & 0x0f;
								reg_data.ui <<= 4;
								reg_data.ui  |= empf1.buf[a + 6] & 0x0f;
								memset(&empf1,0,sizeof(empf1));
								if(WRITEREG(c, reg_data.ui))
									send_byte(0x0d);
								else	
									memset(&empf1,0,sizeof(empf1));
							} else
								memset(&empf1,0,sizeof(empf1));
						break;	
						case 0x14: // 1.2.11	Datentransfer
						break;
						default:
							memset(&empf1,0,sizeof(empf1));
						break;	
					}
				} else
					memset(&empf1,0,sizeof(empf1));
			} else 
				memset(&empf1,0,sizeof(empf1));
		} 
		
	}	
}

bit WRITEREG(unsigned char REG, unsigned int wr) {
	bit b;
 	b = 0;
	if(REG == 223)
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
			} else if(REG == 26) {
				if(wr == ALARM_RST_BUS && COLLECT_ALR)
					BUS_RST = 1; //reset_fn();
				b = 1;	
			} 
			vek[wrteptr_ix].b[1] = 0;
		} 
	}
	return b;	
}

void send_status(void) {
	// unsigned char n;
	memset(&send1,0,sizeof(send1));
	send1.buf[1] = 0x15;
	send1.buf[2] = ret_adr;
	ident_daten(send1.buf+3);
	send1.buf[8] = vek[sh_adr].ui + 0x20;
	//~ strncat(sbuf1.buf + 1, id + 5, 16);
	strncat(send1.buf + 9, id + 5, 16);
	// strncat(sbuf1.buf + 1, "pcd, Testversion",16);
	adr_128k = BK_OFFSET;
	len_128k = 16;
	ptr_128k = send1.buf + 25;
	ee_rdseq_128k();
	// for(n = 0; n < 16; ++n)
		// sbuf1.buf[25+n] = ee_rdb_128k();
		
	send1.buf[41] = 0x16;
	for(send1.cnt = 0; send1.cnt < 42; ++send1.cnt) { 
		if(send1.cnt && !send1.buf[send1.cnt])
			send1.buf[send1.cnt] = 0x20;
		send1.buf[42] += send1.buf[send1.cnt];
	}
	send1.buf[42] |= 0x20;
	send1.buf[43]  = 0x0d;
	send1.buf[44]  = 0x1f;
	send1.cnt = 0;
	empf1.cnt = 45;
	// TREN1 = 1;
	// while(!(SCON1 & 0x20)) ;
	// SBUF1 = 0;
	send_byte(0);
}
