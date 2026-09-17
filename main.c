#include <absacc.h>
#include <math.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ConstDef.h"                 // SFR declarations
#include "eeprom128.h"         
#include "Bus_LCD.h"         
#include "menu_lcd.h"         
#include "regelung.h"         
#include "display.h"         
#include "bus.h"         
#include "relais.h"         
#include "PCF8563_WI.h"         
#include "modbus.h"         
#include "regelung_ely.h"         
#include "eeprom_128drv.h"         

//extern code WERTE tab1[];
//extern code unsigned char id[];

bdata UINT STATUS1;
sbit DOSZEIT_RL3  = STATUS1.b[1]^0;		   
sbit DOSZEIT_RL4  = STATUS1.b[1]^1;		   
sbit FLOW_WRN     = STATUS1.b[1]^2;		   
sbit INIT_DELAY	  = STATUS1.b[1]^3;		   
sbit PH_NULL_ERR  = STATUS1.b[1]^4;  // Steilheitsnullerror		   
sbit PH_STH_ERR   = STATUS1.b[1]^5; // Steilheiterror		   
sbit ON_OFF	      = STATUS1.b[1]^6; // 		   
sbit POT_STH_ERR  = STATUS1.b[1]^7;		   

sbit ASR          = STATUS1.b[0]^0; 		   
sbit ASR_MAN      = STATUS1.b[0]^1;		   
sbit COLLECT_ALR  = STATUS1.b[0]^2; 		   
sbit EXTSTOP_BIT  = STATUS1.b[0]^3; 		   
sbit I_ALR    	  = STATUS1.b[0]^4; 		   
sbit FLOW_ALR     = STATUS1.b[0]^5; 		   
sbit SOLE_ALR     = STATUS1.b[0]^6; 		   
sbit NIVEAU_ALR   = STATUS1.b[0]^7; 		   

bdata UINT STATUS_ELY;
sbit T_ALR     = STATUS_ELY.b[1]^0; 
sbit T_WRNH    = STATUS_ELY.b[1]^1; 
sbit T_WRNL    = STATUS_ELY.b[1]^2; 
sbit I_ALRH    = STATUS_ELY.b[1]^3; 
sbit I_ALRL    = STATUS_ELY.b[1]^4; 
sbit FLOW_ALRH = STATUS_ELY.b[1]^5; 
sbit FLOW_ALRL = STATUS_ELY.b[1]^6; 
sbit U_ALRH    = STATUS_ELY.b[1]^7; 

sbit U_WRNH    = STATUS_ELY.b[0]^0; 
sbit TANK_NOT  = STATUS_ELY.b[0]^1; 
sbit U_ALRL    = STATUS_ELY.b[0]^2; 
sbit E_ANLALR  = STATUS_ELY.b[0]^3; 
sbit ENTHAERT  = STATUS_ELY.b[0]^4; 
sbit BUS_RST   = STATUS_ELY.b[0]^5; 

//~ bit U_ALRL;
bit dos_aus;

//~ sbit MB_WRN_PLSCNT = MB_STATUS.b[1]^6; // Bit 6  = Warnung Plausibilitätskontrolle 
//~ sbit MB_AER_ACTIV  = MB_STATUS.b[1]^7; // Bit 7  = ASR aktiv Messwert eingefroren
//~ sbit MB_EIN_VRZGRN = MB_STATUS.b[0]^0; // Bit 8  = Einschaltverzögerungszeit läuft 
//~ sbit MB_ALR_DES_OB = MB_STATUS.b[0]^1; // Bit 9  = Desinfektion Alarmwert oben
//~ sbit MB_WRN_DES_OB = MB_STATUS.b[0]^2; // Bit 10 = Desinfektion Warnwert oben
//~ sbit MB_WRN_DES_UN = MB_STATUS.b[0]^3; // Bit 11 = Desinfektion Warnwert unten
//~ sbit MB_ALR_DES_UN = MB_STATUS.b[0]^4; // Bit 12 = Desinfektion Alarmwert unten
//~ sbit MB_WRN_DES    = MB_STATUS.b[0]^5; // Bit 13 = Desinfektion Systemwarnung
//~ sbit MB_POT_STH_ERR= MB_STATUS.b[0]^6; // Bit 14 = POT-Steilheit Fehler
//~ sbit MB_POT_STH_ERR= MB_STATUS.b[0]^6; // Bit 14 = POT-Steilheit Fehler


extern void Init_Device(void);
void status_rt(void);
void WDT_RT(void);
void led_rt(unsigned char state);
void add_alarm(unsigned int num, bit set);
void add_warn(unsigned int num);
//~ char des_led_rt(void);
//~ char ph_led_rt(void);
code void (*fptr_alrm[])(void);

code char ip_str[] = {'I', 'P', 0x0d,0};
code char rd_str[] = {'R', 'D', 0x0d,0};


xdata STATUSZEILE status_zeile;

bit CLCKTCK,
	alrm,
	no_verb,
	displ_rst,
	rls,
	mwm,
	ddos_bit,
	ANL_ALR,
	OVFL_ALR,
	TANK_VOLL,
	soft_strg_bit;

xdata unsigned char dly_H2,
					dly_btr, swimm_cnt,
					upd_cnt;
					//~ i;
xdata unsigned int min,
				   on_delay,
				   des_alr_delay,
				   des_alr_delay_cnt,
				   ph_alr_delay,
				   ph_alr_delay_cnt,
				   asr_10min,
				   asr30min,
				   len_strg;


void ondelay_fn() {
	on_delay   = vek[on_dly].ui * 60;
	INIT_DELAY = 1;
}

void main (void) {
	unsigned char msctr,
						cnt_verb,
						alrm_cnt,
						mbb_cnt,
						delay;
						
	char secctr;
			   
	bit dspl,
		bl,
		send_fl,
		sek_bit;
	//~ xdata float fmwert,
				//~ fmwert_1;

	Init_Device();
	WDT_RT();
	init_mempool (&XBYTE [0x0f80], 0x80);
	STATUS1.ui    = 0;
	STATUS_ELY.ui = 0;
	T_ALR = 0;
	ee_ini();
	bus_ini();
	display_reset();
	CLCKTCK = 0;
	msctr   = 0;
	secctr  = 0;
	cnt_verb= 0;
	alrm_cnt= 0;
	alrm    = 0;
	mwm     = 0;
	no_verb = 0;
	delay   = 0;
	min     = 0;
	dly_H2  = 0;
	dly_btr = BTR_VERZ;
	sec_cnt = 0;
	asr_10min = 0;
	asr30min  = 0;
	swimm_cnt = 0;
	des_alr_delay_cnt = 0;
	ph_alr_delay_cnt  = 0;
	
	//~ memset(out_rel,0,sizeof(out_rel));
	//~ INIT_DELAY = 1;
	
	dspl    = 0;
	rls     = 0;
	sek_bit    = 0;
	//~ bilder2();
	//~ // GW_ON = 0;
	//~ display_flush(0,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
	//~ display_flush(1,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
	if(!UC_1608) {
		lcdInit(); //mit diese Zeile funktioniert das Display nicht.
		WDT_RT();
		bilder2();
		display_flush_pcd5(0);
	} else {
		display_reset();
		bilder2();
		display_flush(0,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
		WDT_RT();
		display_flush(1,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
	}
	WDT_RT();
	com1_ini();	
	menu_ini();
	ely_ini();
	displ_rst = 0;
	send_fl   = 0;
	mbb_cnt   = 0;
	rl[0].prc = rl[1].prc = 0;
	//~ on_delay = vek[on_dly].ui * 60;
	ondelay_fn();
	relais_ini_msr();
	btr_art_b0 = standby_txt;
	ANL_ALR = OVFL_ALR = 0;
	luft_cnt_vor  = 0;	
	luft_cnt_nach = 0;	
	//~ (vek[anl_typ_t].ui ==  a30_txt) ? (start_vgang = 0) : (start_vgang = 1); 
	start_vgang = 0; 
	v_start_cnt = 0;
	vers_anlf   = 0;
	versuch_1   = 0;
	versuch_2   = 0;
	upd_cnt     = 0;

	TREN0 = 0;
	
	while (1) {	    	  
		if (CLCKTCK == 1) { 	
			CLCKTCK = 0;		  
			++msctr;
			if(modbus_enabled) {
				if(msctr == 5) {
					if(send_sts){
						send_sts = 0;
						send_status(); // P20 Protokol
					}
					if(mbb_cnt != empf1.cnt)
						mbb_cnt = empf1.cnt;
					else if(!mod_bus)
						modbus_rt();
				}
			}

			//~ if (msctr % 2) {
				//~ if(mbb_cnt != mbbuf.cnt)
					//~ mbb_cnt = mbbuf.cnt;
				//~ else if(!mod_bus)
					//~ modbus_rt();
			//~ }
			
			if (msctr > 9) {	
				msctr = 0;				// 10ms Routinen
				WDT_RT();
				key_press(0);
				if(EMPF0) {
					EMPF0    = 0;
					empf_anf = 0;
					//~ EIE2 = 0x00; // UART1 interrupt disabled
					read_uart0();
					//~ EIE2 = 0x02; // UART1 interrupt enabled
					if(OK_0) {
						OK_0 = 0;
						rls  = 1;
					}
					delay = 0;
					send_fl = 0;
				}
			
				#if 1
				if(EMPF_1 == 1) { // WLAN ESP
                    if(wlan_cmd()) {
						test_update_eepr();
					}
				}
				#endif
				if (RD) {
					RD = 0;
					switch(upd_cnt) {
						case 1:
							set_update();
							send_com();
							adr_128k = 0;
							++upd_cnt;
							break;
						case 2:
							++upd_cnt;
							ee_rdseq_128k(1);
							soft_strg_bit = 0;
							send1_com ();
							delay = 0;
						break;
						case 3:
							if(adr_128k < len_strg) {
								ee_rdseq_128k(1);
								send1_com ();
							} else {
								sprintf(send1.buf,"FINISH");
								send1_com ();
								++upd_cnt;
							}
						break;
						case 4:
							upd_cnt = 0;
							EIE2    = 0x02;
							soft_strg_bit = 1;
							memset(txt_dflt[0], 0, ZEICHEN);
						break;
						default:
							if(rls) {
								if(!send_fl) {
									get_data();
									send_com();
									send_fl = 1;		
								}
							} else {
								relais_ini();
								send_com();
							}
						break;
					}
				}				
				
				++delay;
				if(ON_OFF) {	
					if(!displ_rst) {
						displ_rst = 1;
						display_reset();
					}
					LED_FLOW = 1;
					dspl = 1;
					alrm = 0;
					P4 &= 0x8f;
					memset(&rram, 0, sizeof(rram));
					//~ on_delay = vek[on_dly].ui * 60;
					//~ INIT_DELAY  = 1;
					ondelay_fn();
					sek_bit     = 0;
					ON_OFF_LED  = 1;
					led_rt(LED_AUS);
					rl[0].prc = rl[1].prc = 0;
				}
				else {	
					if(displ_rst) {
						displ_rst = 0;
						display_reset();
					}
					if(KEY_PR || MENU_MODE==11)	{	
						KEY_PR = 0;
						if(send_fl == 0 || !rls)
							menu_rt();
						WDT_RT();
						dspl = 1;
					}
					++alrm_cnt;
					bl = (alrm_cnt % 41) < 20;
					
				
					//~ ALARM_LED  = bl | ~alrm;
					DES_AUS_LED  = bl | ~alrm; // alarm LED
					//~ if(mwm || REL3)
						//~ ALARM_LED = 0;
					//~ LED_FLOW   = bl & mwm;	
					//~ if(on_delay) {
						//~ ON_OFF_LED = bl;
					//~ }
					
					if (sek_bit) {				    
						sek_bit   = 0;
						displ_rst = 0;
						secctr    = 0;	//      Sekundenroutinen
						E_ANLALR  = ANL_ALR;
						//~ EIE2     &= ~0x02;
						//~ MB_STATUS.ui = 0;
						memset(&status_zeile,0,sizeof(status_zeile));
						if(!rls) {
						//~ if(0) {
							if(++cnt_verb > 5) {
								cnt_verb = 0;
								no_verb  = 1;
							}	
							if(no_verb) {
								add_warn(no_connect_txt);
							}	

						} 
						else {	
							no_verb  = 0;
							cnt_verb = 0;
								
							alrm       = 0;
							ON_OFF_LED = 0;
							//~ INIT_DELAY = 0;
							
							//  ----- Elektrolyse --------------
							#define ELEKTROLYSE
							#ifdef ELEKTROLYSE
							if(vek[lev_art_t].b[1] == swimm_txt) {
								if(vek[lev_prod_ix].i == 0) {
									#define SWIMM_DELAY 10
									if(swimm_cnt < SWIMM_DELAY)
										++swimm_cnt;
									else	
										NIVEAU_ALR = 1;
								} else {
									swimm_cnt = 0;
								}
									//~ add_alarm(alr_prod_txt,1);
							}
							//~ if(vek[status].b[1] & H2_ALRM) {
								//~ ANL_ALR = 1;
								//~ led_rt(LED_AUS);
							//~ } else if(vek[lev_prod_ix].ui > vek[prodlev_ovfl].ui) {
								//~ OVFL_ALR   = 1;
								//~ led_rt(LED_AUS);
							//~ } 
							//~ else {	
								if(vek[status_ix].b[1] & ENTHAERTER_REG) {
									add_warn(enth_txt);
									btr_art_b0 = solefill_txt;
									cnt_ini();
									vek[el_V_mv_ix].i = 0;
									led_rt(ENTH_LED);
									ENTHAERT = 1;
									dly_H2 = 0;
								} else {
									ENTHAERT = 0;
									if(vek[status_ix].b[1] & H2_ALRM) {
										if(dly_H2 < 15) // 15 Sek. Verzögerung 
											++dly_H2;
										else {	
											ANL_ALR = 1;
											led_rt(LED_AUS);
											goto ANLALR;
										}
									} else if(vek[lev_prod_ix].ui > vek[prodlev_ovfl].ui) {
										OVFL_ALR   = 1;
										led_rt(LED_AUS);
									} 
									if(vek[btr_art_t].b[1] == run_txt && (COLLECT_ALR | FLOW_WRN)) { 
										ANLALR:
										btr_art_b0 = standby_txt;
									}
									if(fptr_ely[btr_art_b0 - aus_txt]()) {
										if(vek[btr_art_t].b[1] == run_txt) {
											switch(btr_art_b0) {
												case run_txt:
													if(ANL_ALR)
														btr_art_b0 = standby_txt;
													else if(!sec_cnt || TANK_VOLL)
														btr_art_b0 = solefill_txt;
													else if(i_kleiner_5A) {
														if(!versuch_1) {
															btr_art_b0 = cellfill_txt;
															versuch_1 = 1;
															set_sec(zelle_fill_sek);
														} else if(!versuch_2) {	
															btr_art_b0 = cellfill_txt;
															versuch_2 = 1;
															set_sec(zelle_fill_sek);
														} else {	
															btr_art_b0 = standby_txt;
															I_ALR = I_ALRL = 1;
														}	
													} else if(!start_vgang) {
														btr_art_b0 = anl_fill_txt;
														set_sec(anl_fill_sek);
													} else {	
														btr_art_b0 = standby_txt;
													}
													if(ventilator_rl())
														luft_cnt_nach = vek[luft_nach].ui * 60;
													else	
														luft_cnt_nach = 0;
												break;
												case solefill_txt:
													if(SOLE_ALR) {
														btr_art_b0 = standby_txt;
													} else if(TANK_VOLL) {
														btr_art_b0 = standby_txt;
														TANK_VOLL  = 0;
														if(ventilator_rl())
															luft_cnt_nach = vek[luft_nach].ui*60;
														else	
															luft_cnt_nach = 0;
													} else {
														btr_art_b0 = run_txt;
														ely_ini();
														if(ventilator_rl())
															luft_cnt_vor = vek[luft_vor].ui;
														else	
															luft_cnt_vor = 0;
													}
												break;
												case cellfill_txt:
													if(versuch_1 && !versuch_2) {
														btr_art_b0 = run_txt;
														ely_ini();
													} else if(versuch_1 && versuch_2) {	
														btr_art_b0 = run_txt;
														ely_ini();
													}	
												break;
												case anl_fill_txt:
													btr_art_b0 = run_txt;
													ely_ini();
												break;
												case standby_txt:
													btr_art_b0 = solefill_txt;
													ely_ini();
												break;
											}
										} else {
											btr_art_b0 = aus_txt;
										}
									} 
								}
							//~ }
							#endif
							//  ----- Elektrolyse Ende, Messtechnik --------------
							//~ if(vek[msr_t].ui != no_msr_txt && vek[btr_art_t].b[1] != aus_txt) {
							if(vek[msr_t].ui != no_msr_txt) { // Änderung: 11.08.2017
								if(vek[status_ix].ui & EXT_STOP) {
									add_warn(ext_stop_txt);
									EXTSTOP_BIT = 1;
									//~ rl[0].prc = rl[1].prc = 0;
								} else 	
									EXTSTOP_BIT = 0;
								mwm = 0;
								if(vek[mw_art_t].ui == flow_txt) {
									if(vek[fl_aktl_mw_ix].ui < vek[min_fl_mw].ui)
										mwm = 1;
								} else {
									//~ if(vek[status_ix].ui & FLOW1_FAIL)
									if(vek[status_ix].ui & FLOW0_FAIL)
										mwm = 1;
								}
								t_compensation();
								pH_rechnen();
								cl_rechnen();
								if(dos_aus)
									add_warn(dos_aus_txt);
								if(mwm) {
									add_warn(wass_mangl_txt);
									//~ MB_MWM     = 1;
									//on_delay   = vek[on_dly].ui * 60;
									//INIT_DELAY = 1;
									ondelay_fn();
									rl[0].prc = rl[1].prc = 0;
								} else {
									xdata char i;
									if(on_delay) { 
										--on_delay;
										add_warn(on_dly_txt);
										//~ MB_EIN_VRZGRN = 1;
										//~ MB_DOS_STOP   = 1;
									} else {
										INIT_DELAY = 0;	
										//~ if(dos_aus)
											//~ add_warn(dos_aus_txt);
											
										for(i = 0; i < 2; ++i) {
											if(rl[i].ind > 3) { // phdown_fn, phup_fn, rx_Regelung, cl_Regelung
												if(dos_aus) {
													rl[i].prc = 0;
												} else { 
													if(rl[i].ind > 5 && (vek[lev_prod_ix].i < 5)) { // Produkttanklevel < 5 cm
														add_alarm(tank_leer_txt,0);
														rl[i].prc = 0;
													} else {
														if(rl[i].ddos > 0) {
															--rl[i].ddos;
															rl[i].prc = 100;	
															add_warn(rl[i].ddos_txt);
														} else {
															rl[i].prc = (fptr_rel[rl[i].ind])();
															rl_doszeit(i);
															if(rl[i].dos_EEPR == SAVE_FLAG) {
																add_alarm(rl[i].dos_ovfl_txt,0);
																rl[i].prc = 0;
															}
															//~ (*fptr_alrm[rl[i].ind - 3])();
														} 
													}
													//~ (*fptr_alrm[rl[i].ind - 3])();
													(*fptr_alrm[rl[i].ind - 4])();
												}
											}
										}
										
									}
								}
							} else
								rl[0].prc = rl[1].prc = 0, mwm = 0;		
							//~ status_rt();
							{
								xdata unsigned char i;
								for(i = 0; i < 2; ++i) {
									if(rl[i].ind <= 3) { // frei_fn, vent_fn, aimb_fn, alarm_fn
										rl[i].prc = (fptr_rel[rl[i].ind])();
									}
								}
							}
							WDT_RT();
							if(!wifi_update)
								PCF8563_read();
							if(vek[msr_t].ui >= cl_txt && vek[msr_t].ui <= cl_ph_txt) {
								bit aer;
								//~ aer = ASR_MAN | MB_AER_START;
								aer = ASR_MAN;
								if(vek[asr_day].ui > 0) {
									if(vek[std_int].ui == vek[asr_std].b[1])
										if(vek[min_int].ui == vek[asr_min].ui)
											aer = 1;
									if(vek[asr_day].ui == 2)
										if(vek[std_int].ui == asr2)
											if(vek[min_int].ui == vek[asr_min].ui)
												aer = 1;
								}
								if(aer)	{				
										ASR          = 1;
										#define MIN_30 900	// 15 Min.
										#define MIN_10 300	// 5 Min.
										asr30min     = MIN_30;	// 30 min
										asr_10min    = MIN_10;	// 10 min
										rl[0].prc   |= 0x80;
										ASR_MAN      = 0;
										//~ MB_AER_START = 0;
								}
								else
									ASR = 0;	
							}			
							if(asr30min) {
								--asr30min;
								vek[aer_min_ix].ui = asr30min / 60;
								if(asr_10min) --asr_10min;// MB_AER_ACTIV = 1;
							}	
							if(++min > 3600) { // 1 Stunde
								min = 0;
								vek[code0_x].ui = 0;
								if(MENU_MODE)
									menu_ini();
							}
						} // rls == 0
						if(!upd_cnt && !wifi_update)
							menu_rt();  // Display beschreiben ca. 40 ms
						WDT_RT();	
						dspl = 1;
						if(abs(uin_cnt - vek[min_int].b[1]) == 30)	// 30 Min.
							uin_save_flg = 0;
						COLLECT_ALR |= alrm;
						// WLAN-Kommunikation
						if(!modbus_enabled){
							if(!wifi_update && !upd_cnt) {
								memset(&send1,0,sizeof(send1));
								if(!ip_ini) {
									strncpy(send1.buf,ip_str,3);
									send_byte(send1.buf[0]);
								} else if(strlen(send1.buf) == 0) {
									strncpy(send1.buf, rd_str, 3);
									send_byte(send1.buf[0]);
								}
							} else {
								show_update();
							}
						}
						if(BUS_RST) {
							BUS_RST = 0;
							if(COLLECT_ALR) {
								EIE2 = 0; // UART1 interrupt disabled
								reset_fn();
								EIE2 = 0x02; // UART1 interrupt enabled
							}
						}
					} // if (sek_bit)
				} // ON_OFF == 0  
				if(delay > 250)	{	
					rls = 0;
					delay = 0;
					vek[el_V_mv_ix].i    = 0;
					vek[el_A_mv_ix].i    = 0;
					vek[flow0_ix].i      = 0;
					vek[t0_ix].i         = 0;
					vek[lev_prod_ix].i   = 0;
					vek[cl_mv_ix].i      = 0;
					vek[ph_mv_ix].i      = 0;
					vek[rx_ix].i         = 0;
					vek[fl_aktl_mw_ix].i = 0;
					vek[t1_ix].i         = 0;
					vek[cur_ix].i        = 0;
					vek[status_ix].i     = 0;
				}
				if(++secctr > 99)
					secctr = 0, sek_bit = 1;			
			}
			if(dspl) {
				if(!TREN0)
					if(!EMPF0 && !empf_anf) {
						dspl = 0;
						TR0 = 0;
						WDT_RT();
						// ca. 30 ms	
						
						EIE2 = 0; // UART1 interrupt disabled
						if(!UC_1608) {
							display_flush_pcd5(0);
						} else { 
							display_flush(0,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
							display_flush(1,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
						}
						EIE2 = 0x02; // UART1 interrupt enabled
						TR0 = 1;
						secctr += 4;
					}
				WDT_RT();
			}
		}
	}
}

void led_rt(unsigned char state) {
	P2 |= 0x1c;
	P2 &= ~state;
}

void add_alarm(unsigned int num, bit set) {
	if(status_zeile.ctr_alr < STATUS_LEN) {
		status_zeile.alarm[status_zeile.ctr_alr] = num; 
		++status_zeile.ctr_alr;
	}
	alrm = set;
}

void add_warn(unsigned int num) {
	if(status_zeile.ctr_warn < STATUS_LEN) {
		status_zeile.warn[status_zeile.ctr_warn] = num; 
		++status_zeile.ctr_warn;
	}
}

bit des_alrm(void) {
	if(des_alr_delay_cnt < des_alr_delay) {	
		++des_alr_delay_cnt;
		return 0;
	} else
		return 1;
}

bit ph_alrm(void) {
	if(ph_alr_delay_cnt < ph_alr_delay) {	
		++ph_alr_delay_cnt;
		return 0;
	} else
		return 1;
}


//~ void status_rt(void) {
	
	//~ if((vek[status].ui & FREE_DOS) == 0) {
		//~ add_warn(ext_free_txt);
		//~ add_warn(ext_stop_txt);
		//~ EXTSTOP_BIT    = 1;
		//~ MB_EXT_FREE = 1;
	//~ }	
	//~ if(vek[status].ui & WARN_DES) {
		//~ add_warn(warn_des_txt);
		//~ MB_WRN_DES = 1;
	//~ }	
//~ }
//-----------------------------------------------------------------------------
// Timer0 Routine
//-----------------------------------------------------------------------------
// Zyklus 1ms
void t0_rt (void) interrupt 1 {
	CLCKTCK = 1;
    TH0     = 0xf4;
    TL0     = 0x48;
	KEY     = P1;
	if(key_del)
		--key_del;
}

void WDT_RT(void) {
	PCA0CPH4 = 255;     	
	WD_TOOGLE = 1;
	WD_TOOGLE = 0;
}

void rx_alrm_fn(void) {
	if(vek[rx_ix].i < rx_mv_min) {	
		add_alarm(alr_rx_txt,0);
	} else if(vek[rx_ix].i > rx_mv_max) {	
		add_alarm(alr_rx_txt,0);
	} else if(vek[rx_ix].i <= vek[rx_alr_un].i) {	
		if(des_alrm())
			add_alarm(alr_rx_txt,0);
		else
			add_warn(warn_rx_txt);
	} else if(vek[rx_ix].i <= vek[rx_wr_un].i)
		add_warn(warn_rx_txt), des_alr_delay_cnt = 0;
	else
		des_alr_delay_cnt = 0;	
}

void ph_alrm_fn(void) {
	if(PH_STH_ERR)	
		add_alarm(ph_sth_err_txt,0);
	
	if(PH_NULL_ERR)	
		add_alarm(null_err_txt,0);

	if(vek[ph_ix].ui <= vek[ph_alr_un].ui) {	
		if(ph_alrm())
			add_alarm(alr_un_ph_txt,0);
		else
		 	add_warn(warn_un_ph_txt); 
	} else if(vek[ph_ix].ui <= vek[ph_wr_un].ui)
		add_warn(warn_un_ph_txt), ph_alr_delay_cnt = 0;
	else if(vek[ph_ix].ui >= vek[ph_alr_ob].ui) {	
		if(ph_alrm())
			add_alarm(alr_ob_ph_txt,0);
		else
		 	add_warn(warn_ob_ph_txt); 
	} else if(vek[ph_ix].ui >= vek[ph_wr_ob].ui)
			add_warn(warn_ob_ph_txt), ph_alr_delay_cnt = 0;
	else
		ph_alr_delay_cnt = 0;		
}

void cl_alrm_fn(void) {
	if(!asr_10min) {
		if(vek[cl_val_ix].i <= vek[cl_alr_un].i) {	
			if(des_alrm())
				add_alarm(alr_un_cl_txt,0);//MB_ALR_DES_UN = 1;
			else
				add_warn(warn_un_cl_txt);//MB_WRN_DES_UN = 1;
		} else if(vek[cl_val_ix].i <= vek[cl_wr_un].i) {
			add_warn(warn_un_cl_txt);
			des_alr_delay_cnt = 0;
			//~ MB_WRN_DES_UN     = 1;
		} else if(vek[cl_val_ix].i >= vek[cl_alr_ob].i) {	
			if(des_alrm())
				add_alarm(alr_ob_cl_txt,0);//MB_ALR_DES_OB = 1;
			else 
				add_warn(warn_ob_cl_txt);//MB_WRN_DES_OB = 1;
			
		} else if(vek[cl_val_ix].i >= vek[cl_wr_ob].i) {
			add_warn(warn_ob_cl_txt);
			des_alr_delay_cnt = 0;
			//~ MB_WRN_DES_OB     = 1;
		} else
			des_alr_delay_cnt = 0;
	}
	if(vek[status_ix].ui & POT_FAIL) {
		//~ add_warn(warn_aer_txt);
		//~ MB_WRN_AER = 1;		
	}
	if(POT_STH_ERR) {
		add_alarm(pot_sth_err_txt,0);
		//~ MB_POT_STH_ERR = 1;
	}	
}

code void (*fptr_alrm[])(void) = {ph_alrm_fn, ph_alrm_fn, rx_alrm_fn, cl_alrm_fn};



//~ char des_led_rt(void)
//~ {
	//~ xdata char rt;
	//~ if(DES_AUS)
		//~ rt = 0x02;
	//~ else {
		//~ if(vek[dos_P].ui)
			//~ rt = 0x04;
		//~ else	
			//~ rt = 0x01;
	//~ }
	//~ return rt;
//~ }
	
//~ char ph_led_rt(void)
//~ {	
	//~ xdata char rt;
	//~ rt = 0x01;
	//~ if(PH_AUS)
		//~ rt = 0x02;
	//~ else
		//~ rt = 0x04;	
	//~ return rt;
//~ }


















