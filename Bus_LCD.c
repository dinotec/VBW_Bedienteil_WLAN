#include <intrins.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ConstDef.h"                 
#include "default_werte_ely.h"
#include "eeprom128.h"
#include "main.h"                 
#include "bus.h"                 
#include "regelung_ely.h"
#include "relais.h"
#include "PCF8563_WI.h"                 


/*
char data * xdata str;         //  ptr in xdata to data char 
int xdata * data numtab;      // ptr in data to xdata int 
long code * idata powtab;     //  ptr in idata to code long
*/
extern xdata unsigned char KEY;

code unsigned char *str_anf  = "<<<";
code unsigned char *str_end  = ">>>";
code unsigned char *str_ok   = "OK";

//~ unsigned char xdata * xdata str;
extern xdata UINT vek[];
xdata EMPF_BUF sbuf0;
bit EMPF0,
	empf_anf,
	OK_0,
	RD,
	KP;


void bus_ini(void) {
	memset(&sbuf0, 0, sizeof(sbuf0));
	OK_0  = 0;
	TREN0 = 0;
	empf_anf = 0;
	RD = 0;
	KP = 0;
	not_zeit_cnt = vek[not_zeit].ui * 60;

}

void UART0_ISR (void) interrupt 4 {
	if (RI0) {
		RI0 = 0;
		empf_anf = 1; 
		sbuf0.buf[sbuf0.cnt] = SBUF0;
		if(sbuf0.buf[sbuf0.cnt] == 13)
			EMPF0 = 1;
		else 
			if(++sbuf0.cnt >= SBUF_LAENGE) sbuf0.cnt = 0;
    }
	else {
		TI0 = 0;
		if(upd_cnt==3) {
			if(++send1.cnt < BLOCK_LEN) {
				SBUF0 = send1.buf[send1.cnt];
			} else {
				TREN0 = 0;
			}
		} else {
			if(sbuf0.buf[++sbuf0.cnt] == 0) {	
				TREN0 = 0;
				memset(&sbuf0, 0, sizeof(sbuf0));
			} else {
				SBUF0 = sbuf0.buf[sbuf0.cnt];
			}
		}
	}
}

unsigned char find(unsigned char c) {
	unsigned char n;
	
	for(n = 0; n < SBUF_LAENGE; ++n) {
		if(sbuf0.buf[n] == c)
			break;
	}
	if(n < SBUF_LAENGE - 2) {
		if(sbuf0.buf[n]==sbuf0.buf[n+1] && sbuf0.buf[n+1]==sbuf0.buf[n+2]) // <<< oder >>>
			n += 3;
		else 
			n = 0;
	}
	else 
		n = 0;
	return n;
}

bit t1_to_lev(int t1) {
	#define FUENF_PRC 30
	if(abs(t1 - vek[t1_ix].i) < FUENF_PRC)
		return 1;
	else
		return 0;
}

void read_uart0(void) {
	xdata unsigned char p0, p1; 
	
	p0 = find(str_anf[0]);
	if(p0 > 0) {	
		p1 = find(str_end[0]);
		if(p1 > p0) {	
			// Daten senden:<<<DATA: RX_U PH_U DES_U TEMP0 TEMP1 FLOW0 FLOW1 STATUS
			// Daten senden: U I FLOW_0 TEMP0 LEVEL CL_U PH_U RX_U FLOW1 TEMP1 CUR_IN STATUS
			switch(sbuf0.buf[p0]) {
				case 'D':
					for(p1 = 0; p1 < 14; ++p1) {
						while(sbuf0.buf[p0] != ' ')
							++p0;
						++p0;
						switch(p1) {
							case 0: 
								vek[el_V_mv_ix].i  = atoi(sbuf0.buf + p0);
							break; 
							case 1: 
								//~ vek[el_A_mv_ix].i = atoi(sbuf0.buf + p0) * 10; 
								vek[el_A_mv_ix].i = atoi(sbuf0.buf + p0); 
							break; 
							case 2: 
								vek[flow0_ix].i = atoi(sbuf0.buf + p0); 
							break; 
							case 3: 
								vek[t0_ix].i  = atoi(sbuf0.buf + p0);
								vek[t0_ix].i += vek[t0_korr].i;
							break; 
							case 4: 
								vek[lev_prod_ix].i = atoi(sbuf0.buf + p0);
								vek[lev_prod_ix].i /= 10;
							break; 
							case 5:
								vek[cl_mv_ix].i  = atoi(sbuf0.buf + p0);
								vek[cl_mv_ix].i -= vek[cl_mv0_in].i;
							break; 
							case 6: 
								vek[ph_mv_ix].i = atoi(sbuf0.buf + p0);
							break; 
							case 7: 
								vek[rx_ix].i = atoi(sbuf0.buf + p0);
							break; 
							case 8: 
								vek[fl_aktl_mw_ix].i = atoi(sbuf0.buf + p0);
								if(vek[mw_art_t].ui == flow_txt) {
									if(vek[fl_aktl_mw_ix].i > 0) {
										if(vek[fl_rate_mw].ui > 0) {
											xdata float tmp_f; 
											#define MS_PRO_H 3.6e6 	// ms pto Stunde
											tmp_f = MS_PRO_H / vek[fl_aktl_mw_ix].ui;
											tmp_f /= vek[fl_rate_mw].ui;
											// --- Linearisierung -----------
											tmp_f *= 0.57665;
											tmp_f += 28.62972;
											vek[fl_aktl_mw_ix].ui = tmp_f;
											if(vek[fl_aktl_mw_ix].ui < 30)
												vek[fl_aktl_mw_ix].ui = 0;
										}
										else
											vek[fl_aktl_mw_ix].ui = 0;
									}
								}	
							break; 
							case 9: 
								vek[t1_ix].i = atoi(sbuf0.buf + p0);
								//~ if(vek[lev_art_t].b[1] == swimm_txt) {
									//~ if(t1_to_lev(LEV_MIN))
										//~ vek[lev_prod_ix].i = vek[prodlev_min].i - 5;
									//~ else if(t1_to_lev(LEV_RUN))
										//~ vek[lev_prod_ix].i = (vek[prodlev_min].i + vek[prodlev_max].i) / 2;
									//~ else if(t1_to_lev(LEV_MAX))
										//~ vek[lev_prod_ix].i = vek[prodlev_max].i + 5;	
									//~ else {
										//~ vek[lev_prod_ix].i = 0;
									//~ }
								//~ }
								break; 
							case 10: 
								vek[cur_ix].i = atoi(sbuf0.buf + p0);
								break; 
							case 11: 
								vek[status_ix].i = atoi(sbuf0.buf + p0);
								if(vek[lev_art_t].b[1] == swimm_txt) {
									if((vek[status_ix].i & BIN_TOP) && (vek[status_ix].i & BIN_BOTT))
										vek[lev_prod_ix].i = vek[prodlev_max].i + 5;	
									else if((vek[status_ix].i & BIN_BOTT) && ((vek[status_ix].i & BIN_TOP) == 0))
										vek[lev_prod_ix].i = (vek[prodlev_min].i + vek[prodlev_max].i) / 2;
									else if(((vek[status_ix].i & BIN_TOP) == 0) && ((vek[status_ix].i & BIN_BOTT) == 0))
										vek[lev_prod_ix].i = vek[prodlev_min].i - 5;
									else {
										vek[lev_prod_ix].i = 0;
									}
								} else {
									if(vek[status_ix].i & BIN_TOP) {
										if(vek[not_btr_t].ui == ja_txt) {
											TANK_NOT = 1; // Warnung, Produktionstopp
											not_zeit_cnt = vek[not_zeit].ui * 60;
										}
										TANK_VOLL    = 1;
										vek[lev_prod_ix].i = vek[prodlev_max].i + 5;	
									} else {
										if(TANK_NOT) {
											vek[lev_prod_ix].i = (vek[prodlev_min].i + vek[prodlev_max].i) / 2;
											if(not_zeit_cnt > 0)
												--not_zeit_cnt;
											//~ else 
												//~ ; // Produktion einschalten in der stanby_fn()	
										}
									}
								}
								break; 
							case 12:
								vek[prv_ch0_ix].i = atoi(sbuf0.buf + p0);
								break;
							case 13:
								vek[prv_ch1_ix].i = atoi(sbuf0.buf + p0);
								break;
							default: break; 
						}
						#ifdef TEST
							vek[el_V_mv_ix].i  = 6500;
							vek[el_A_mv_ix].i  = 2300;
							vek[flow0_ix].i    = 340; 
							vek[t0_ix].i       = 350;
							vek[lev_prod_ix].i = 8;
							vek[prv_ch0_ix].i  = 1234;
							vek[prv_ch1_ix].i  = 0;
							vek[status_ix].i   = 0;
						#endif
					}
				break;
				case 'O':
					if(sbuf0.buf[++p0] == 'K') {
						OK_0 = 1;
						++p0;
						vek[dev_nr_ix].i = atoi(sbuf0.buf + p0);
						while(sbuf0.buf[p0] != ' ')
							++p0;
						vek[soft_strg_ix].i = atoi(sbuf0.buf + p0);
						vek[soft_strg_ix].i = (vek[soft_strg_ix].i / 100) + (vek[soft_strg_ix].i%100)*100;
					}
				break;	
				case 'R':
					if(sbuf0.buf[++p0] == 'D')
						RD = 1;
				break;	
				case 'T': // zeit und datum T: 12 20 12 1 17  -> std min Tag Monat Jahr
					++p0;
					p1 = sscanf(sbuf0.buf + p0, "%u %u %u %u %u", vek+std_int, vek+min_int, vek+day_int,vek+mon_int, vek+year_int);
					if(p1 == 5) {
						if(!test_var(std_int))
							if(!test_var(min_int))
								if(!test_var(day_int))
									if(!test_var(mon_int))
										if(!test_var(year_int)) {
											new_time();
											RD = 1;
										}
					}
				break;	
				case 'S': // seriennummer
					++p0;
					vek[dev_nr_ix].i = atoi(sbuf0.buf + p0);
					WDT_RT();
					if(vek[dev_nr_ix].i > 0) {
						speichern_int(dev_nr_ix);
						RD = 1;
					}
				break;	
				case 'H': // hardwarenummer
					++p0;
					vek[mdate_in].i = atoi(sbuf0.buf + p0);
					WDT_RT();
					if(!test_var(mdate_in)) {
						speichern_int(mdate_in);
						RD = 1;
					}
				break;	
				case 'K':
					RD = 1;
					KP = 1;
				break;
				default:
				break;	
			}
		}
	}
	memset(&sbuf0.buf, 0, sizeof(sbuf0));
}

void send_com() {
	sbuf0.buf[strlen(sbuf0.buf)] = 13;
	sbuf0.cnt = 0;
	TREN0 = 1;
	SBUF0 = sbuf0.buf[0];
}

void send1_com() { // überträgt BLOCK_LEN Bytes
	send1.cnt = 0;
	TREN0     = 1;
	SBUF0     = send1.buf[0];
	while(TREN0)
		WDT_RT();
	
}
void set_update(void) {
	//~ memset(&send1,0,sizeof(send1));
	//~ sprintf(send1.buf,"%sUPDATE%s\r",str_anf,str_end); 
	memset(&sbuf0,0,sizeof(sbuf0));
	sprintf(sbuf0.buf,"%sUPDATE%s",str_anf,str_end); 
}

void get_data(void) {
	xdata unsigned char btr;
	memset(&sbuf0,0,sizeof(sbuf0));
	//~ vek + sole_puls, vek + flow_soll, &RL[2].procent, &RL[3].procent, vek + btr_art
	//~ sprintf(sbuf0.buf,"%sD%u %u %bu %bu,", str_anf, vek[sole_puls].ui, vek[flow0_soll].ui, r2_prc, r3_prc); 
	//~ sprintf(sbuf0.buf + strlen(sbuf0.buf),"%bu", vek[btr_art].b[1]);  
	if(luft_cnt_vor)
		btr = standby_txt;
	else
		btr = btr_art_b0;	
	//~ sprintf(sbuf0.buf,"%sD%u %u %bu %bu %bu", str_anf, vek[sole_puls_ix].ui, vek[flow0_soll_ix].ui, rl[0].prc, rl[1].prc, btr_art_b0 - aus_txt); 
	sprintf(sbuf0.buf,"%sD%u %u %bu %bu %bu", str_anf, vek[sole_puls_ix].ui, vek[flow0_soll_ix].ui, rl[0].prc, rl[1].prc, btr - aus_txt); 
	strcat(sbuf0.buf,str_end);
}

void relais_ini(void) {
	memset(&sbuf0,0,sizeof(sbuf0));
	//~ sprintf(sbuf0.buf,"%sR%bu %bu %bu %bu,",str_anf, mod[0], mod[1], mod[2], mod[3]); // 16 Zeichen
	//~ sprintf(sbuf0.buf + strlen(sbuf0.buf),"%u %u %u %u", rel_mod[0].ui, rel_mod[1].ui, rel_mod[2].ui, rel_mod[3].ui); // 24 Zeichen
	if(!KP) {
		xdata unsigned char typ_messbereich, typ_mw;
		typ_messbereich = vek[anl_typ_t].b[1] - a30_txt;
		if(vek[mgl_t].ui == mgl_1_txt)
			typ_messbereich |= 0x10;
		
		if(vek[mw_art_t].ui == swimm_txt) // 1 -Schwimmschalter, 0 - Pulseingang
			typ_mw = 1;
		else 
			typ_mw = 0;	
		if(vek[lev_art_t].ui == swimm_txt)
			typ_mw |= 2;
		//~ sprintf(sbuf0.buf,"%sR%u %bu,",str_anf, werte_el[i_soll][vek[anl_typ_t].b[1]-a30_txt], vek[anl_typ_t].b[1] - a30_txt);
		sprintf(sbuf0.buf,"%sR%u %bu,",str_anf, werte_el[i_soll][vek[anl_typ_t].b[1]-a30_txt], typ_messbereich);
		sprintf(sbuf0.buf + strlen(sbuf0.buf),"%bu %u %bu %bu,", vek[r3_art_t].b[1] - an_aus_txt, vek[r3_frq].ui, vek[r3_pp].b[1], vek[r3_min].b[1]);
		sprintf(sbuf0.buf + strlen(sbuf0.buf),"%bu %u %bu %bu %bu",  vek[r4_art_t].b[1] - an_aus_txt, vek[r4_frq].ui, vek[r4_pp].b[1], vek[r4_min].b[1], typ_mw);
	} else {
		KP = 0;
		sprintf(sbuf0.buf,"%sKey: %bu",str_anf, ~KEY);
	}
	strcat(sbuf0.buf,str_end); // strlen(sbufo.buf) = 43
}


