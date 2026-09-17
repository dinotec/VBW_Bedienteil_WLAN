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
#include "ma4_20.h"         
#include "PCF8563_WI.h"         
#include "modbus.h"         

#define MIN_30 900	// 15 Min.
#define MIN_10 300	// 5 Min.

extern code WERTE tab1[];
extern code WERTE ph_lim;
void add_alarm(unsigned int num);
void add_warn(unsigned int num);
char des_led_rt(void);
char ph_led_rt(void);

extern code unsigned char id[];

bdata UINT STATUS1;
sbit INIT_DELAY	  = STATUS1.b[1]^0;		   
sbit STHT_PH_WARN = STATUS1.b[1]^1;  // Steilheitsnullerror		   
sbit STHT_PH_ERR  = STATUS1.b[1]^2; // Steilheiterror		   
sbit NULL_PH_ERR  = STATUS1.b[1]^3; // Steilheiterror		   
sbit ON_OFF	      = STATUS1.b[1]^4; // 		   
sbit DES_AUS      = STATUS1.b[1]^5;		   
sbit STOP_EXT     = STATUS1.b[1]^6;		   

sbit PH_AUS       = STATUS1.b[0]^0;		   
sbit MB_AER_START = STATUS1.b[0]^1; // Bit 13 = AER vom Modbus starten
sbit ASR_MAN      = STATUS1.b[0]^2;		   
sbit COLLECT_ALR  = STATUS1.b[0]^3; 		   
sbit POT_STH_ERR  = STATUS1.b[0]^4; 		   

bdata UINT MB_STATUS;
sbit MB_DOS_STOP   = MB_STATUS.b[1]^0; // Bit 0  = Abschaltung der Dosierung ( Reglerstop)
sbit MB_EXT_FREE   = MB_STATUS.b[1]^1; // Bit 1  = externe Freigabe
sbit MB_DES_LEER   = MB_STATUS.b[1]^2; // Bit 2  = Leermeldung Desinfektionsmittel
sbit MB_ZIRK_PUM   = MB_STATUS.b[1]^3; // Bit 3  = Zirkulationspumpe läuft
sbit MB_MWM        = MB_STATUS.b[1]^4; // Bit 4  = Messwassermangel
sbit MB_WRN_AER    = MB_STATUS.b[1]^5; // Bit 5  = Warnung Funktionstest nach AER
sbit MB_WRN_PLSCNT = MB_STATUS.b[1]^6; // Bit 6  = Warnung Plausibilitätskontrolle 
sbit MB_AER_ACTIV  = MB_STATUS.b[1]^7; // Bit 7  = ASR aktiv Messwert eingefroren
sbit MB_EIN_VRZGRN = MB_STATUS.b[0]^0; // Bit 8  = Einschaltverzögerungszeit läuft 
sbit MB_ALR_DES_OB = MB_STATUS.b[0]^1; // Bit 9  = Desinfektion Alarmwert oben
sbit MB_WRN_DES_OB = MB_STATUS.b[0]^2; // Bit 10 = Desinfektion Warnwert oben
sbit MB_WRN_DES_UN = MB_STATUS.b[0]^3; // Bit 11 = Desinfektion Warnwert unten
sbit MB_ALR_DES_UN = MB_STATUS.b[0]^4; // Bit 12 = Desinfektion Alarmwert unten
sbit MB_WRN_DES    = MB_STATUS.b[0]^5; // Bit 13 = Desinfektion Systemwarnung
sbit MB_POT_STH_ERR= MB_STATUS.b[0]^6; // Bit 14 = POT-Steilheit Fehler


extern void Init_Device(void);
void status_rt(void);
void WDT_RT(void);


xdata STATUSZEILE status_zeile;
//~ xdata unsigned int rl_prc;

bit CLCKTCK,
	alrm,
	no_verb,
	displ_rst,
	rls,
	mwm;

 bdata char REL;
 sbit ASR  = REL^0; // Asr	  
 sbit REL1 = REL^1; // Relais 1 Zirkulationspumpe	  
 sbit REL2 = REL^2; // Relais 2	  
 sbit REL3 = REL^3; // Relais 3 Alarmrelais	  
 sbit RLAN = REL^4; // Relais0 manuell einschalten	  
	
xdata unsigned int min,
				   on_delay,
				   des_alr_delay,
				   des_alr_delay_cnt,
				   asr_10min,
				   asr30min;

void main (void) {
	xdata unsigned char msctr,
						cnt_verb,
						alrm_cnt,
						mbb_cnt,
						delay;
						
	xdata char secctr,
			   prc_des;
	bit dspl,
		bl,
		send_fl,
		sek_bit;
	xdata float fmwert,
				fmwert_1;
	Init_Device();
	WDT_RT();
	ee_ini();
	bus_ini();
	display_reset();
	CLCKTCK = 0;
	msctr   = 0;
	secctr  = 0;
	cnt_verb= 0;
	alrm_cnt= 0;
	alrm    = 0;
	no_verb = 0;
	delay   = 0;
	min     = 0;
	asr_10min = 0;
	asr30min  = 0;
	des_alr_delay_cnt = 0;
	//~ memset(rl_prc,0,sizeof(rl_prc));
	INIT_DELAY = 1;
	
	dspl    = 0;
	rls     = 0;
	sek_bit    = 0;
	bilder2();
	// GW_ON = 0;
	display_flush(0,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
	display_flush(1,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
	WDT_RT();
	com1_ini();	
	menu_ini();
	displ_rst = 0;
	send_fl   = 0;
	prc_des   = 0;
	mbb_cnt   = 0;
	fmwert    = 0.0;
	fmwert_1  = 0.0;
	
	while (1) {	    	  
		if (CLCKTCK == 1) { 	
			CLCKTCK = 0;		  
			++msctr;
			if(bus_free < 5)
				++bus_free;
			else{
				if(send_sts){
					send_sts = 0;
					send_status(); // P20 Protokol
				}
			}
			
			//~ if (msctr == 5) {
			if (msctr % 2) {
				if(mbb_cnt != mbbuf.cnt)
					mbb_cnt = mbbuf.cnt;
				else if(!mod_bus)
					modbus_rt();
			}
			
			if (msctr > 9) {	
				msctr = 0;				// 10ms Routinen
				WDT_RT();
				key_press();
				
				if(EMPF0) {
					EMPF0    = 0;
					empf_anf = 0;
					//~ EIE2 = 0x00; // UART1 interrupt disabled
					read_uart0();
					//~ EIE2 = 0x02; // UART1 interrupt enabled
					if(OK_0){
						OK_0 = 0;
						rls  = 1;
					}
					delay = 0;
					send_fl = 0;
				}
				
				if (abs(secctr - 50) < 4) {
					secctr = 54;
					if(rls) {
						if(!send_fl) {
							get_data();
							send_com();
							send_fl = 1;		
						}	 
					}
					else {
						relais_ini();
						send_com();
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
					on_delay = vek[on_dly].ui * 60;
					INIT_DELAY  = 1;
					sek_bit     = 0;
					DES_AUS_LED = 1;
					PH_AUS_LED  = 1;
					ON_OFF_LED  = 1;
					//~ memset(rl_prc,0,sizeof(rl_prc));
					vek[dos_P].ui = 0;
				}
				else {	
					if(displ_rst) {
						displ_rst = 0;
						display_reset();
					}
					if(KEY_PR || MENU_MODE==11)	{	
						KEY_PR = 0;
						if(send_fl == 0)
							menu_rt();
						WDT_RT();
						dspl = 1;
					}
					++alrm_cnt;
					bl = (alrm_cnt % 41) < 20;
					
				
					if(DES_AUS) PUMPE_DES_LED =  1, DES_AUS_LED = 0;// REL0 = 0;
					else       PUMPE_DES_LED =  0, DES_AUS_LED = 1;// REL0 = 1;
					
					if(PH_AUS) PUMPE_PH_LED =  1, PH_AUS_LED = 0, REL1 = 0;
					else        PUMPE_PH_LED =  0, PH_AUS_LED = 1, REL1 = 1;

					//~ alrm = 0;
					ALARM_LED  = bl | ~alrm;
					if(mwm || REL3)
						ALARM_LED = 0;
					LED_FLOW   = bl & mwm;	
					if(on_delay) {
						ON_OFF_LED = bl;
						//~ ALARM_LED  = 1;
					}
					
					if (sek_bit) {				    
						sek_bit   = 0;
						displ_rst = 0;
						secctr    = 0;	//      Sekundenroutinen
						//~ EIE2     &= ~0x02;
						MB_STATUS.ui = 0;
						memset(&status_zeile,0,sizeof(status_zeile));

						if(!rls) {
							if(++cnt_verb > 5)
							{
								cnt_verb = 0;
								no_verb  = 1;
							}	
							if(no_verb)
								add_warn(no_connect_txt);

						} 
						else {	
							no_verb  = 0;
							cnt_verb = 0;
							
							//~ mwm = (vek[flow_aktl].ui < vek[min_flow].ui);	
							if(vek[flow_aktl].ui < vek[min_flow].ui)
									add_warn(wass_mangl_txt);
							mwm = (bit)(vek[status].ui & FLOW0_FAIL);	
							mwm &= (bit)vek[flow_rate].ui;
							if(mwm)	{
								//~ ALARM_LED  = 1;
								on_delay = vek[on_dly].ui * 60;
								INIT_DELAY = 1;
								vek[dos_P].ui = 0;
								add_alarm(wass_mangl_txt);
								MB_MWM = 1;
							}
							REL3 = mwm;
							t_compensation();
							(*fdes_rechnen)();
							//~ M=LM+((Z-LM)/N)
							//~ vek[des_mwert].i = fmwert + ((float)((float)vek[des_med].i - fmwert) / (float)vek[mttl].i);
							fmwert = fmwert_1 + ((float)((float)vek[des_med].i - fmwert_1) / (float)vek[mttl].i);
							vek[des_mwert].i = fmwert;
							pH_rechnen();
							if(on_delay ) {
								--on_delay;
								if(!mwm)
									add_warn(on_dly_txt);
								vek[dos_P].ui = 0;
								MB_EIN_VRZGRN = 1;
								MB_DOS_STOP   = 1;
							}
							else {	
								alrm       = 0;
								ON_OFF_LED = 0;
								INIT_DELAY = 0;
								//--- Desinfektion-----------
								if(!DES_AUS) {
									if(!asr_10min && !mwm)
										prc_des = (*fdes_regeln)();
									else
										prc_des = 0, MB_DOS_STOP = 1; 
									menge_Regelung();
									switch(vek[strg_art].ui) {
										case mess_txt:
											if(prc_des > 0) 	// cl senken
												vek[dos_P].ui = 0;
											else 
												vek[dos_P].ui = -1 * prc_des;	
										break;
										case kombi_txt:
											{
												xdata float mpf;
												mpf  = vek[dos_P].ui;
												mpf *= vek[kombi_menge].i;
												mpf *= 1e-4;
												mpf *= prc_des;
												mpf *= -1;
												vek[dos_P].ui += mpf;
											}
										break;
										default:
										break;
									}
								}
								else {
									add_warn(des_pumpe_txt);
									vek[dos_P].ui = 0;
									MB_DOS_STOP   = 1;
								}
								//--- Desinfektionende-----------
								if(PH_AUS) {
									add_warn(pH_pumpe_txt);
									MB_ZIRK_PUM = 0;
								}
								else 
									MB_ZIRK_PUM = 1;
								status_rt();
							} // on_delay == 0
							WDT_RT();
							PCF8563_read();
							//~ if(vek[des].ui == cl_txt || vek[des].ui == oz_txt || vek[des].ui == pc_txt)
							{
								bit aer;
								aer = ASR_MAN | MB_AER_START;
								if(vek[asr_day].ui > 0) {
									if(vek[std].ui == vek[asr_std].b[1])
										if(vek[min_1].ui == vek[asr_min].ui)
											aer = 1;
									if(vek[asr_day].ui == 2)
										if(vek[std].ui == asr2)
											if(vek[min_1].ui == vek[asr_min].ui)
												aer = 1;
								}
								if(aer)	{				
										ASR          = 1;
										asr30min     = MIN_30;	// 30 min
										asr_10min    = MIN_10;	// 10 min
										ASR_MAN      = 0;
										MB_AER_START = 0;
								}
								else
									ASR = 0;	
							}			
							if(asr30min) {
								//~ xdata unsigned int ixx;
								--asr30min;
								//~ ixx = asr_30min;
								//~ ixx = asr30min / 60;
								
								vek[asr_30min].ui = asr30min / 60;
								if(asr_10min) --asr_10min, MB_AER_ACTIV = 1;
							}	
							if(++min > 3600) { // 1 Stunde
							
								min = 0;
								vek[code0].ui = 0;
								if(MENU_MODE)
									menu_ini();
							}
						} // rls == 0
						if(STOP_EXT)
							vek[dos_P].ui = 0, MB_DOS_STOP = 1;
						menu_rt();  // Display beschreiben ca. 40 ms
						WDT_RT();	
						fmwert_1 = fmwert;
						dspl = 1;
						ma_out(0);	
						ma_out(1);	
						ma_out(2);	
						ma_out(3);	
						alrm |= mwm;
						COLLECT_ALR  = REL3;
						COLLECT_ALR |= alrm;
						//~ EIE2 |= 0x02; 
					}
				} // ON_OFF == 0  
				if(delay > 250)	{	
					rls = 0;
					PUMPE_DES_LED = PUMPE_PH_LED = ALARM_LED = 1;
					delay = 0;
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
						display_flush(0,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
						display_flush(1,1,0); // 1. oder 2. Display; normal oder umdrehen; Bild invertieren
						TR0 = 1;
						secctr += 4;
					}
						
				WDT_RT();
			}
		}
	}
}

void add_alarm(unsigned int num)
{
	if(status_zeile.ctr_alr < STATUS_LEN) {
		status_zeile.alarm[status_zeile.ctr_alr] = num; 
		++status_zeile.ctr_alr;
	}
}
void add_warn(unsigned int num)
{
	if(status_zeile.ctr_warn < STATUS_LEN) {
		status_zeile.warn[status_zeile.ctr_warn] = num; 
		++status_zeile.ctr_warn;
	}
}

bit des_alrm(void)
{
	if(des_alr_delay_cnt < des_alr_delay) {	
		++des_alr_delay_cnt;
		return 0;
	}
	else
		return 1;
}

void status_rt(void) {
	bit alrm_st;
	alrm_st = 0;
	REL3  = 0;
	switch(vek[des].ui) {
		case cl_txt:
			if(!asr_10min) {
				if(vek[cl].i >= vek[cl_alr_ob].i) {	
					add_alarm(alr_ob_des_txt);
					alrm_st |= des_alrm();
					MB_ALR_DES_OB = 1;
				}
				else if(vek[cl].i < vek[cl_alr_un].i) {	
					add_alarm(alr_un_des_txt);
					alrm_st |= des_alrm();
					MB_ALR_DES_UN = 1;
				}
				else if(vek[cl].i >= vek[cl_wr_ob].i) {
					add_warn(warn_ob_des_txt);
					des_alr_delay_cnt = 0;
					MB_WRN_DES_OB     = 1;
				}
				else if(vek[cl].i < vek[cl_wr_un].i) {
					add_warn(warn_un_des_txt);
					des_alr_delay_cnt = 0;
					MB_WRN_DES_UN     = 1;
				}
			}
			if(vek[status].ui & POT_FAIL) {
				add_warn(warn_aer_txt);
				MB_WRN_AER = 1;		
			}
		break;	
		case cldx_txt:
			if(!asr_10min) {
				if(vek[cldx].i >= vek[cldx_alr_ob].i) {	
					add_alarm(alr_ob_des_txt);
					alrm_st |= des_alrm();
					MB_ALR_DES_OB = 1;
				}
				else if(vek[cldx].i < vek[cldx_alr_un].i) {	
					add_alarm(alr_un_des_txt);
					alrm_st |= des_alrm();
					MB_ALR_DES_UN = 1;
				}
				else if(vek[cldx].i >= vek[cldx_wr_ob].i) {
					add_warn(warn_ob_des_txt);
					des_alr_delay_cnt = 0;
					MB_WRN_DES_OB     = 1;
				}
				else if(vek[cldx].i < vek[cldx_wr_un].i) {
					add_warn(warn_un_des_txt);
					des_alr_delay_cnt = 0;
					MB_WRN_DES_UN     = 1;
				}
			}
			if(vek[status].ui & POT_FAIL) {
				add_warn(warn_aer_txt);		
				MB_WRN_AER = 1;		
			}
		break;	
		case pc_txt:
			if(!asr_10min) {
				if(vek[pc].i >= vek[pc_alr_ob].i) {	
					add_alarm(alr_ob_des_txt);
					alrm_st |= des_alrm();
					MB_ALR_DES_OB = 1;
				}
				else if(vek[pc].i <= vek[pc_alr_un].i) {	
					add_alarm(alr_un_des_txt);
					alrm_st |= des_alrm();
					MB_ALR_DES_UN = 1;
				}
				else if(vek[pc].i >= vek[pc_wr_ob].i) {
					add_warn(warn_ob_des_txt);
					des_alr_delay_cnt = 0;
				}
				else if(vek[pc].i <= vek[pc_wr_un].i) {
					add_warn(warn_un_des_txt);
					des_alr_delay_cnt = 0;
				}
			}
			if(vek[status].ui & POT_FAIL) {
				add_warn(warn_aer_txt);		
				MB_WRN_AER = 1;		
			}
		break;	
		case oz_txt:
			if(!asr_10min) {
				if(vek[oz].i >= vek[oz_alr_ob].i) {	
					add_alarm(alr_ob_des_txt);
					alrm_st |= des_alrm();
					MB_ALR_DES_OB = 1;
				}
				else if(vek[pc].i <= vek[pc_alr_un].i) {	
					add_alarm(alr_un_des_txt);
					alrm_st |= des_alrm();
					MB_ALR_DES_UN = 1;
				}
				else if(vek[oz].i >= vek[oz_wr_ob].i) {
					add_warn(warn_ob_des_txt);
					des_alr_delay_cnt = 0;
				}
				else if(vek[oz].i <= vek[oz_wr_un].i) {
					add_warn(warn_un_des_txt); 
					des_alr_delay_cnt = 0;
				}
			}
			if(vek[status].ui & POT_FAIL) {
				add_warn(warn_aer_txt);		
				MB_WRN_AER = 1;		
			}
		break;	
		default:
		break;	
	}

	if(vek[ph_mess].ui  == ja_txt) {
		if(STHT_PH_ERR) {	
			add_alarm(ph_sth_err_txt);
			alrm_st  = 1;
		}
		if(NULL_PH_ERR) {	
			add_alarm(ph_null_err_txt);
			alrm_st  = 1;
		}
	}
	
	STOP_EXT = 0;
	if(vek[status].ui & EMPTY_CL) {
		add_alarm(des_leer_txt), alrm_st = 1;
		STOP_EXT = 1;
		REL3     = 1;
	}
	
	if(vek[status].ui & EXT_STOP) {
		add_warn(ext_stop_txt);
		STOP_EXT = 1;
	}
	if((vek[status].ui & FREE_DOS) == 0) {
		add_warn(ext_free_txt);
		//~ add_warn(ext_stop_txt);
		STOP_EXT    = 1;
		MB_EXT_FREE = 1;
	}	
	if(vek[status].ui & WARN_DES) {
		add_warn(warn_des_txt);
		MB_WRN_DES = 1;
	}	
	
	if(POT_STH_ERR) {
		add_alarm(pot_sth_err_txt);
		MB_POT_STH_ERR = 1;
		alrm_st  = 1;
	}	
	
	alrm |= alrm_st;
}
//-----------------------------------------------------------------------------
// Timer0 Routine
//-----------------------------------------------------------------------------
// Zyklus 1ms
void t0_rt (void) interrupt 1 
{
	CLCKTCK = 1;
    TH0     = 0xf4;
    TL0     = 0x48;
	KEY     = P1;
	if(key_del)
		--key_del;
}

void WDT_RT(void)
{
	PCA0CPH4 = 255;     	
	WD_TOOGLE = 1;
	WD_TOOGLE = 0;
}

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


















