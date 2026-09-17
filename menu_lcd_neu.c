#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <intrins.h>
#include <string.h>
#include "ConstDef.h"                 // SFR declarations
#include "eeprom128.h"
#include "eeprom_128drv.h"
#include "display.h"
#include "main.h"
#include "regelung.h"
#include "PCF8563_WI.h"
#include "ma4_20.h"
#include "Bus_LCD.h"
#include "attr.h"

#define MENU_LENGTH 20

//~ #define REG_DES 0x7f
//~ #define REG_PH  0xfb
#define REG_PH  0x7f
#define REG_DES 0xfb
#define ESC     0xbf
#define ENTER   0xdf
#define UP      0xef
#define DOWN    0xf7
#define DOS     0xfe
#define DES_ESC (REG_DES & ESC)

#define DELAY_RES 300
#define DELAY_SWITCH (DELAY_RES / 10)

//~ #define ANF_SZ_ADR    OFFSET_EEPR +  3
//~ #define ANZ_SPR_ADR   OFFSET_EEPR +  6
//~ #define ANZ_TXT_ADR   OFFSET_EEPR +  7
//~ #define MENU_ANF_ADR  OFFSET_EEPR +  9
//~ #define ANF_MENU_TXT  OFFSET_EEPR + 10
//~ #define ANF_SPR_DB    OFFSET_EEPR + 12

//~ #define VAR_TXT_LEN 8
//~ #define VAR_TXT_LEN 6
//~ #define DIR_VAR 300

extern code WERTE tab1[];
extern code WERTE tab2[];
void Wert_speichern(void);
void key_press(void);
void menu_rt(void);
int Wert_grenze(unsigned int pos, int i);
void txt_anf(unsigned int nmr);
void read_text(unsigned char*);
void lingua(void);
void speichern_int(unsigned int);
void speichern_cal_mv(unsigned char, unsigned char);
//~ void text_fn(unsigned char n);
unsigned char umenu_fn(unsigned char*);
unsigned char var_fn(unsigned char*);
unsigned char txtDB_fn(unsigned char*);
unsigned char m1_fn(unsigned char*); // dummy
unsigned char const_fn(unsigned char*);
void balken(unsigned char pos, unsigned char prc);
void tank_lev(unsigned char, unsigned char, unsigned char, bit);
void tanksM(void);
unsigned char tankMarker(unsigned char l, unsigned char overflow);


// code char* dir = " --->>>";
// code char* dir = " ~";
code char* striche = "  ---";
code char* tklein = "C";

code char* basic[] =
{		 //  v   h
/*00*/	"00 000 012 ",		//"Des:           Redox"
/*01*/	"01 018 004 ",		// Des-Zahl 
/*02*/	"00 000 039 ", 		// "Q:             ph (oder °C)"
//~ /*03*/	"01 030 031 ",		// Q-Zahl
/*03*/	"01 018 031 ",		// Q-Zahl
/*04*/	"01 160 031 ",		// pH-, T-Zahl
/*05*/	"03 000 025 180", 	// horiz. Linie
/*06*/	"03 000 050 240", 	// horiz. Linie
/*07*/	"04 085 000 050", 	// vert.  Linie
//~ /*08*/	"01 150 004 ", 		// Redox-Zahl
/*08*/	"01 138 004 ", 		// Redox-Zahl
/*09*/	"00 000 054 ",	     // Statuszeile
/*10*/	"00 000 000 ",	     // Procent für Des
/*11*/	"00 120 026 ",       // Temperatur klein
/*12*/	"00 010 054 ",	     // Bargraf
/*13*/	"00 121 012 ",		 //"m2_in_txt"
};
code char* format[] = {
/* 0 */ "%6d",
/* 1 */ "%6.1f",
/* 2 */ "%6.2f",
/* 3 */ "%6.3f",
};

#define basic_len  strlen(basic[0]) 

code unsigned char *Font0_Col0     = "00 000 ";

code unsigned char pos_anz[3][2] = {
	{20,23},{120, 23},{20, 48}
};


bit plus, 
	minus,
	KEY_PR,
	dpd_bit,
	flag,
	cur_dir,
	flush;
	

xdata RRAM rram;
xdata unsigned char row[60],
					tmp0[40],
					tmp1[40],
					anz_spr,
					MENU_MODE,
					KEY,
					key_del,
					s_zeichen[250],
					status_cnt,
					var_akt,
					dir_new,
					cur_pos,
					menu_vec[MENU_LENGTH],
					menu_anf,
					txt_akt,
					cal_nr,
					min_max[6];

enum {pmin, pmax, smin, smax, plm, slm};

xdata menustack menu_stack;	


xdata unsigned int	offset,
					anz_txt;

static xdata unsigned char blinken;
static xdata signed char speed;
static xdata unsigned char sts_ctr;
static xdata unsigned char speed_cnt;
static xdata unsigned char index;
static xdata int var_wert;
xdata float flw_cmp;

xdata struct var_act
{
	int   nr[8];
	char pos[8];
} vr;

code unsigned char (*fptr[])(unsigned char*) = {umenu_fn, var_fn, txtDB_fn, m1_fn, m1_fn, const_fn};

unsigned int adresse_2(void) {
	xdata UINT ui;
	adr_128k += OFFSET_EEPR;
	ptr_128k  = ui.b;
	len_128k  = 2;
	ee_rdseq_128k();
	return ui.ui;
}
unsigned long adresse_3(void) {
	xdata FLOAT f;
	f.ul = 0;
	adr_128k += OFFSET_EEPR;
	ptr_128k  = f.b;
	len_128k  = 3;
	ee_rdseq_128k();
	return (f.ul >> 8);
}

void lingua(void) {
	adr_128k  = 3; // Sonderzeichen
	adr_128k  = adresse_3(); 
	adr_128k += (vek[sprache].ui - de_txt) * 3;
	adr_128k  = adresse_3(); 
	adr_128k += OFFSET_EEPR;
	len_128k  = sizeof(s_zeichen);
	memset(s_zeichen, 0xff, len_128k);
	ptr_128k  = s_zeichen;
	ee_rdseq_128k();
}


void menu_ini(void) {
//	xdata unsigned char txt_cnt;
	WDT_RT();
	adr_128k = 6 + OFFSET_EEPR;
	anz_spr  = ee_rdb_128k();
	if(vek[sprache].ui < de_txt || vek[sprache].ui > sr_txt) vek[sprache].ui = de_txt;
	vek[sprache].ui = de_txt;
	adr_128k   = 7;
	anz_txt    = adresse_2(); // Anzahl der Texten
	adr_128k   = 9 + OFFSET_EEPR;
	menu_anf   = ee_rdb_128k();
	cur_pos    = 1;
	dir_new    = 0;
	lingua();
    //~ i_anteil();
	//~ regelung_ini();
	//~ des_alarm_fn();
	memset(&menu_stack, 0, sizeof(menu_stack));
	memset(menu_vec, 0, sizeof(menu_vec));
	memset(&vr,0,sizeof(vr));
	memset(&rram, 0, sizeof(rram));
	sts_ctr = 0;
	
	dpd_bit = 0;
	//---------------
	MENU_MODE  = 0;
	plus       = 0;
	minus      = 0;
	blinken    = 0;
	speed      = 0;
	speed_cnt  = 0;
	KEY_PR     = 0;
	key_del    = 0;
	flush      = 1;
}

unsigned char komma(unsigned char nr) {
	switch(nr) {
		case cl_sth: case t_korr: case cl_konz:
			return 1;	
		break;
		case cl_val: case cl_dpd: case cl_alr_ob:
		case cl_wr_ob: case cl_soll: case cl_wr_un: case cl_alr_un:
		case cl_p: case cl_hys: 
			return 2;	
		break;
		default:
			return 0;
		break;
	}
}

void zeile_fuellen(unsigned char zeile, unsigned char text_nr) {
	unsigned char //cnt,
				  n;
	//signed char c;
	
	memset(row,  0, sizeof(row));
	memset(tmp1, 0, sizeof(tmp1));
	memset(tmp0, 0, sizeof(tmp0));
	flag = 0;
	if(!zeile)
		sprintf(row, "%s%03d ", Font0_Col0, (int)0);	
	else
		sprintf(row, "%s%03d   ",Font0_Col0, (int)zeile*8);	
	
	if(zeile == cur_pos) {
		row[11] = '>';
		cur_dir = 1;
		txt_akt = text_nr;
	} else {
		cur_dir = 0;
	}
	txt_anf(text_nr);
	ptr_128k = tmp1;
	ee_rdseq_128k();
	for(n = 0; tmp1[n] > 0; ++n) {
		unsigned char ret;
		ret = 0;
		if(tmp1[n] < 7)  
			ret = (*fptr[tmp1[n]-1])(tmp1 + n);
		if(ret >= anl_typ && ret <= set) {
			n = 0xff;
			if(cur_dir && MENU_MODE==11)
				txt_anf(var_wert);
			else
				txt_anf(vek[ret].ui);
			ptr_128k = tmp1;
			memset(tmp1,0,sizeof(tmp1));
			ee_rdseq_128k();
		}		
	}		
	if(flag) {
		unsigned char fill, len;
		len = strlen(row) + strlen(tmp0);
		fill = 51 - len;
		len = strlen(row);
		while(--fill > 0)
			row[len++] = 0x20;
		row[len] = 0;
		if((cur_dir && flush) || !cur_dir)
			strcat(row, tmp0);
	}		
	display_wr(row);	
}

float div_fn(unsigned char n) {
	float f;
	f = vek[n].ui / 100.0;
	return f;
}

void txt_anf(unsigned int nmr) {
	unsigned int m0, 
					   txtStart, 
					   txtStop;
	//10..11 - MenuTxt Startadr
	adr_128k  = 10;
	m0 = adresse_2();
	m0 += (nmr * 2);
	adr_128k  = m0; 
	txtStart  = adresse_2();
	adr_128k  = m0 + 2;
	txtStop   = adresse_2();
	len_128k  = txtStop - txtStart;
	adr_128k  = txtStart + OFFSET_EEPR;
}

bit rel_frq(unsigned char nr) {
	bit b;
	(vek[nr].ui == an_aus_txt || vek[nr].ui == puls_pause_txt) ? (b = 0) : (b = 1);
	return b;
}
bit rel_pp(unsigned char nr) {
	bit b;
	(vek[nr].ui == an_aus_txt || vek[nr].ui == puls_frq_txt) ? (b = 0) : (b = 1);
	return b;
}

bit text_ok(unsigned int nr) {
	bit b;
	b = 1;
	switch(nr) {
		case 8: case 19:
			//~ b = vek[msr].ui == 178 || vek[code0].ui == 87;
			b = vek[msr].ui == ja_txt;
		break;
		break;
		//---------------------
		default:
		break;
	}
	return b;
}

void menu_fuellen(unsigned char pos) {
	xdata unsigned char r,
						anz,
						i;
	xdata UINT ui;
	
	memset(&rram,0,sizeof(rram));
	memset(tmp0, 0, sizeof(tmp0));
	// richtige Menu finden
	adr_128k  = menu_anf + menu_stack.menu_nr[0];
	ui.ui = adresse_2();
	anz = ui.b[1] - ui.b[0];
	ptr_128k = tmp0;
	adr_128k = ui.b[0] + OFFSET_EEPR;
	len_128k = anz;
	ee_rdseq_128k();
	//---- menu_vec[] füllen
	memset(menu_vec,0,sizeof(menu_vec));
	for(r = 0, i = 0; i < anz; ++i) {
		if(text_ok(tmp0[i]))
			menu_vec[r++] = tmp0[i];
	}
	memset(tmp0, 0, sizeof(tmp0));
	zeile_fuellen(0, menu_vec[0]);
	for(i = 1; (i + pos) < r && i < 8; ++i)
		zeile_fuellen(i, menu_vec[i + pos]);
	//~ return r;
}	

//~ unsigned char cursor_nach_unten(unsigned char max, unsigned char pos) {	
unsigned char cursor_nach_unten(unsigned char pos) {	
	//~ if((cur_pos + pos)  <  (max - 1)) { 
	if((cur_pos + pos)  <  strlen(menu_vec + 1)) { 
		if(cur_pos < 7)
			++cur_pos;
		else
			++pos;	
	}		
	return pos;
}
	
unsigned char cursor_nach_oben(unsigned char pos) {
	if(pos) 
		--pos;
	else if(cur_pos > 1)
		--cur_pos;
	return pos;
}

void key_press(void) {
	static xdata unsigned char key_pr = 0xff;
	static xdata unsigned int res_del;// ansaugen;
	KEY |= 0x02;	// wegen LED_FLOW
	
	if(key_pr != KEY) {	
		KEY_PR = 1;
		min    = 0;
		switch(KEY) {	
			case ESC: // Esc
				if(ON_OFF || !MENU_MODE) {
					break;
				}
				if(MENU_MODE==11) {
					MENU_MODE = 1;
					flush     = 1;
				} else {
					if(menu_stack.size > 0) {
						//~ MENU_MODE = 10;
						//~ menu_akt  = menu_saved;
						unsigned char i;
						for(i = 0; i < STACK_LAENGE - 1; ++i) {
							menu_stack.menu_nr[i] = menu_stack.menu_nr[i+1]; 
							menu_stack.pos[i]     = menu_stack.pos[i+1]; 
						}
						--menu_stack.size;
						cur_pos   = menu_stack.pos[0];
						MENU_MODE = 1;
						speed     = 0;
						speed_cnt = 0;
					} else
						menu_ini();
				}
			break;	
			case ENTER: // Enter
				if(key_del)	{
					--key_del;
					break;
				}
				else
					key_del = 0x7f;
				if(ON_OFF){
					unsigned char cch;
					adr_128k = 2*contr + 1;
					cch = ee_rdb_128k();
					if(cch != vek[contr].b[1]) {
						adr_128k = contr * 2;
						ee_wrb_128k(0);
						++adr_128k;
						ee_wrb_128k(vek[contr].b[1]);
					}
					break;
				}
					
					if(MENU_MODE < 2) {	
						++MENU_MODE;
					}
					if(MENU_MODE == 11)	{
						if(vek[var_akt].ui != var_wert) {	
							vek[var_akt].ui = var_wert;
							if(var_akt < set_txt)
								Wert_speichern();	
						}
						flush     = 1;
						MENU_MODE = 1;
						//~ switch(vr.nr[menu_stack.pos[0]]) {
						switch(var_akt) {
							case sprache: 
								lingua(); 
							break;
							case contr:
								displ_rst = 0;
							break;
							case set_txt:
								switch(txt_akt) {
									case 30:	// Kalibriren
									break;
									case 31: // Kalibrierungsreset
									break;
								}
							break;
							/*
							case std: case min_1: case day: case mon: case year:
								new_time();
							case strg_art:
								regelung_ini();
								rls = 0;
							break;
							case soll_menge: case concentr: case pump_P: case hub_max:
								rls = 0;
							break;
							break;
							case ph_dpd: 
								if(var_wert == ja_txt) {
									//~ vek[index].i = vek[ph_mv].i;
									vek[index].i = ph_mv_corr;
									speichern_int(index);
									SteilheitPH();
									rls = 0;
								}
								vek[ph_dpd].ui = nein_txt;
							break;
							case ph_reset:
								if(var_wert == ja_txt) {
									vek[ph_mv_eich1].i = tab2[ph_mv_eich1 - ph_mv_eich1].dflt;
									speichern_int(ph_mv_eich1);
									vek[ph_mv_eich2].i = tab2[ph_mv_eich2 - ph_mv_eich1].dflt;
									speichern_int(ph_mv_eich2);
									vek[ph_innen].i = tab1[ph_innen].dflt;
									speichern_int(ph_innen);
									SteilheitPH();
									vek[ph_reset].ui = nein_txt;
									rls = 0;
								}
							break;
							case rx_dpd:
								if(var_wert == ja_txt){
									vek[rx_corr].i = vek[rx_eich].i - vek[rx_mv].i;
									speichern_int(rx_corr);
									vek[rx_dpd].ui = nein_txt;
									rls = 0;
								}
							break;
							case rx_reset:
								if(var_wert == ja_txt) {
									vek[rx_corr].i = 0;
									speichern_int(rx_corr);
									vek[rx_eich].i = tab1[rx_eich].dflt;
									speichern_int(rx_eich);
									vek[rx].i = vek[rx_mv].i + vek[rx_corr].i;
									rls = 0;
								}
							break;
							case cl_dpd: case pc_dpd: case oz_dpd: case cldx_dpd: 
								dpd_bit = 1;
							break;
							case cl_reset:
								if(var_wert == ja_txt) {
									vek[cl_sth].i = 250;
									speichern_int(cl_sth);
									vek[cl_reset].ui = nein_txt;
								}
							break;
							case asr_start:
								if(var_wert == ja_txt) {
									ASR_MAN = 1;
								}
							break;
							case on_dly :
								on_delay = vek[on_dly].ui * 60;
							break;	
							case flow_komp:
								flw_cmp = vek[flow_komp].i / 10000.0;
							break;
							case ph_mess: case des: case rl_0_frq: 
							case rl_0_min: case rl_0_pp: case rl_0_art:
								rls = 0;
							break;	
							case reset:
								if(var_wert == ja_txt) 
									Set_default();
							break;	
							*/
							default : break;
						}
					}
			break;
			case UP: // ++
				if(ON_OFF) {
					if(vek[contr].b[1] < tab1[contr].max) {
						++vek[contr].b[1];
						displ_rst = 0;
					}
					break;
				}
				if(!MENU_MODE) {
					if(++sts_ctr >= (status_zeile.ctr_alr + status_zeile.ctr_warn))
						sts_ctr = 0;
				} else if(MENU_MODE != 11)
						MENU_MODE = 5;
			break;	
			case DOWN: // --
				if(ON_OFF) {
					if(vek[contr].b[1] > tab1[contr].min) {
						--vek[contr].b[1];
						displ_rst = 0;
					}
					break;
				}
				if(!MENU_MODE) {
					if(--sts_ctr >= (status_zeile.ctr_alr + status_zeile.ctr_warn))	{	
						sts_ctr = status_zeile.ctr_alr + status_zeile.ctr_warn - 1;
						if(sts_ctr >= (status_zeile.ctr_alr + status_zeile.ctr_warn))
							sts_ctr = 0;
					}
				} else if(MENU_MODE != 11)
						MENU_MODE = 6;
			break;	
			case DOS: // Regelung 
				ON_OFF = ~ON_OFF;
				adr_128k = status * 2;
				ee_wrb_128k((unsigned char)ON_OFF);
				if(ON_OFF)
					P2 |= 0x7f;
				else {	
					ee_ini();
					menu_ini();
					//~ regelung_ini();
				}
			break;	
			default:
				speed     = 0;
				speed_cnt = 0;
			break;	
		}
	}
	else if(MENU_MODE == 11) {
		if(KEY == UP) {	
			plus   = 1;
			KEY_PR = 1;
		}
		else if(KEY == DOWN) {
			minus  = 1;
			KEY_PR = 1;
		}
	}
	else if(KEY == REG_DES) {	
		if(++res_del == DELAY_SWITCH) {
			DES_AUS = ~DES_AUS;
			adr_128k = status*2 + 1;
			ee_wrb_128k((unsigned char)DES_AUS);
			DES_AUS_LED = ~DES_AUS; 
		}
	}
	else if(KEY == REG_PH )	{	
		//~ if(vek[ph_mess].i == ja_txt)
		if(++res_del == DELAY_SWITCH) {
			PH_AUS = ~PH_AUS;
			adr_128k = status*2 + 2;
			ee_wrb_128k((unsigned char)PH_AUS);
			PH_AUS_LED = ~PH_AUS;
		}
	}
	else if(KEY == ESC)	{
		if(++res_del > DELAY_RES) {	
			if(INIT_DELAY) {
				INIT_DELAY = 0;
				on_delay   = 0;
				res_del    = 0;
			}
		}
	}	
	else if(KEY == DES_ESC && !MENU_MODE) {
		if(DES_AUS) {
			RLAN = 1;
		}
	}
	else
		res_del = 0, RLAN = 0;
	key_pr = KEY;
}


// MENU_MODE: 1 - Menuanfang; 2 - Untermenu oder Wert verstellen; 5 - Menu nach oben; 6 - Menu nach unten
//			 10 - Zurück  aus Untermenu, danach = 1; 11 - blinken , 12 - txt_cnt = 0;


void menu_rt(void) {
	static xdata unsigned char del_cnt;
	static xdata unsigned mn_pos=0;
	xdata unsigned char tmp; 
	//~ xdata unsigned char mn_gr;
	
		switch(MENU_MODE) {
			case 1: // Menuanfang
				dir_new = 0;
				menu_fuellen(mn_pos);
			break;
			case 2: // Untermenu oder Wert verstellen
				if(dir_new > 0) {
					xdata unsigned char i;
					menu_stack.pos[0] = cur_pos;
					for(i = STACK_LAENGE - 1; i > 0; --i ) {
						menu_stack.menu_nr[i] = menu_stack.menu_nr[i - 1];
						menu_stack.pos[i]     = menu_stack.pos[i - 1];
					}
					menu_stack.menu_nr[0] = dir_new;
					++menu_stack.size;
					cur_pos = 1;
					mn_pos  = 0;
					MENU_MODE  = 1;
				}
				else if (var_akt <= code0) {// Wert verstellen
					//~ if((vek[code0].ui != 11) && (vek[code0].ui != 87) && (vek[code0].ui != 178) && vr.nr[menu_stack.pos[0]] != code0) {
						//~ MENU_MODE = 1;
					//~ }
					//~ else {	
						// blinken
					var_wert = vek[var_akt].ui;
					MENU_MODE = 11;
				}
			break;	
			case 5: // Menu nach oben
				mn_pos = cursor_nach_oben(mn_pos);
				MENU_MODE = 1;
			break;	
			case 6: // Menu nach unten
				mn_pos = cursor_nach_unten(mn_pos);
				MENU_MODE = 1;
			break;	
			case 10: // Zurück  aus Untermenu
				cur_pos   = 1;
				MENU_MODE = 1;
			break;
			case 11:
				if(plus){
					if(!(++del_cnt % 2)) {
						++var_wert;
					}
					if(++speed_cnt > 60) {	
						++speed;
						speed_cnt = 0;
					}
					var_wert += speed;
					var_wert  = Wert_grenze(var_akt, var_wert);
					blinken   = 0;
					plus      = 0;
				}
				else if(minus) {
					if(!(++del_cnt % 2)) {
						--var_wert;
					}
					if(++speed_cnt > 60) {	
						--speed;
						speed_cnt = 0;
					}
					var_wert += speed;
					var_wert  = Wert_grenze(var_akt, var_wert);
					minus     = 0;
					blinken   = 0;
				}
				if(++blinken % 6 < 3) {
					flush = 1;	
				}
				else {
					flush = 0;
				}
				menu_fuellen(mn_pos);
			break;				
			default:
				//~ b = (bit)(vek[status].ui & 0x0008) | no_verb;
				memset(&rram,0,sizeof(rram));
				for(tmp = 5; tmp < 8; ++tmp) {
					memset(row,0,sizeof(row));
					strcpy(row, basic[tmp]);
					display_wr(row);
				}
				//~ memset(row,0,sizeof(row));
				//~ strcpy(row,"04 180 000 050"); 	// vert.  Linie
				//~ display_wr(row);
				line_v(180, 0, "50");
				{
					static unsigned char lev ;
					if( ++lev > 120) lev = 0;
					balken(0,lev);		
					balken(1,lev);		
					balken(2,lev);
					tanksM();
					tank_lev(195, 2, min_max[plm], 0);
					tank_lev(220, 2, min_max[slm], 1);
					if(++vek[lev_prod].ui >= vek[prodlev_ovfl].ui) vek[lev_prod].ui = 0;
					if(++vek[lev_sole].ui >= vek[soleLev_ovfl].ui) vek[lev_sole].ui = 0;
				}		
		/*		
				if(mwm | no_verb) {	
					sprintf(txt_dflt[3],"%s%s", basic[3], striche); // % Regler
					sprintf(txt_dflt[4],"%s%s", basic[4], striche); // % Regler
					sprintf(txt_dflt[1],"%s%s", basic[1], striche); // % Regler
					sprintf(txt_dflt[8],"%s%s", basic[8], striche); // % Regler
					//~ sprintf(txt_dflt[10],"%s%bu", basic[10], 0); // % Regler
					sprintf(txt_dflt[10],"%s", basic[10]); // % Regler
					//~ strcat(txt_dflt[10],"%");
					if(vek[ph_mess].i == ja_txt){
						sprintf(txt_dflt[11],"%s%s%s", basic[11], striche, tklein); // temperatur
					}
					else
						wasser_bild();	
				}
				else {	
					//~ sprintf(txt_dflt[3],"%s%5.1f", basic[3],(float)(vek[bwue_aktl].ui / 1e3)); // % Regler
					sprintf(txt_dflt[3],"%s%6.1f", basic[3],(float)(bwue_aktl_l / 1e3)); // % Regler
					sprintf(txt_dflt[8],"%s%5d", basic[8], vek[rx].i);
					if(!asr_10min) {
						if(vek[strg_art].ui == mess_txt) {
							//~ sprintf(txt_dflt[10],"%s%3bu", basic[10], vek[dos_P].ui); // % Regler
							sprintf(txt_dflt[10],"%s%bu", basic[10], vek[dos_P].b[1]); // % Regler
							strcat(txt_dflt[10],"%");
						}
						else {
						sprintf(txt_dflt[10],"%s   ", basic[10]); // % Regler
						}
					}
					else
						sprintf(txt_dflt[10],"%sAER", basic[10]); // % Regler
					
					if(vek[ph_mess].i == ja_txt) {
						sprintf(txt_dflt[4],"%s%5.2f", basic[4],(float)vek[ph].ui / 100);
						//~ sprintf(txt_dflt[11],"%s%5.1f", basic[11], (float)vek[t0].i/10.0); // temperatur
						sprintf(txt_dflt[11],"%s%2.1f", basic[11], (float)vek[t0].i/10.0); // temperatur
						strcat(txt_dflt[11],tklein);
					}
					else {
						wasser_bild();
						sprintf(txt_dflt[4],"%s%5.1f", basic[4], (float)vek[t0].i/10.0);
					}
					switch(vek[des].ui) {
						case cl_txt: // Freies Chlor
							//~ sprintf(txt_dflt[1],"%s%5.2f",basic[1],(float)vek[cl].ui / 100.0);
							sprintf(txt_dflt[1],"%s%5.2f",basic[1],(float)vek[des_mwert].ui / 100.0);
						break;	
						case pc_txt: // Poolcare
							//~ sprintf(txt_dflt[1],"%s%5d",basic[1], vek[pc].i);
							sprintf(txt_dflt[1],"%s%5d",basic[1], vek[des_mwert].i);
						break;	
						case oz_txt: // Ozon
							//~ sprintf(txt_dflt[1],"%s%5.2f",basic[1], (float)vek[oz].ui / 100.0);
							sprintf(txt_dflt[1],"%s%5.2f",basic[1], (float)vek[des_mwert].ui / 100.0);
						break;	
						case cldx_txt: // 
							//~ sprintf(txt_dflt[1],"%s%5.2f",basic[1], (float)vek[cldx].ui / 100.0 );
							sprintf(txt_dflt[1],"%s%5.2f",basic[1], (float)vek[des_mwert].ui / 100.0 );
						break;	
						default:
						break;	
					}
				}
				{
					xdata unsigned int tmp;
					tmp = strlen(basic[9]);
					txt_dflt[9][tmp] = 0;
							
					if(sts_ctr && sts_ctr >= status_zeile.ctr_alr + status_zeile.ctr_warn)
						--sts_ctr;
					if(sts_ctr < status_zeile.ctr_alr)
						tmp = status_zeile.alarm[sts_ctr];
					else
						tmp = status_zeile.warn[sts_ctr - status_zeile.ctr_alr];
					
					memset(buf_d,0,sizeof(buf_d));
					
					if(tmp) {
						static bit bgr;
						b = 0;
						if(bgr & !b) {
							bargr_fn();
						}
						else {
							sprintf(buf_d,"%bu+%bu ", status_zeile.ctr_alr, status_zeile.ctr_warn);
							txt_anf(tmp);
							sprintf(txt_dflt[9],"%s%s", basic[9],buf_d);
							read_text(txt_dflt[9]);
						}
						bgr = ~bgr;
					}
					else {
						bargr_fn();
					}
				}
			*/
			break;	
			 
		}
}

int Wert_grenze(unsigned int pos, int i) {
	switch(pos) {
		case cl_alr_ob:   case cl_wr_ob:    case cl_soll: case cl_wr_un:
		case prodlev_ovfl: case prodlev_max:
		case soleLev_ovfl: case soleLev_max: 
			if(i < vek[pos+1].i)
				i = vek[pos+1].i;
		break;
		default: break;		
	}		
	switch(pos) {
		case cl_wr_ob:    case cl_soll: case cl_wr_un: case cl_alr_un:
		case prodlev_max: case prodlev_min: 
		case soleLev_max: case soleLev_min: 
			if(i > vek[pos-1].i)
				i = vek[pos-1].i;
		break;
		default: break;
	}				
	
	if(i > tab1[pos].max) {
		i = tab1[pos].max;
	} else if(i < tab1[pos].min) {
		 i = tab1[pos].min;
	}
	return i;
}

void Wert_speichern(void) {
	adr_128k = var_akt * 2;
	ee_wrb_128k(vek[var_akt].b[0]);
	++adr_128k;
	ee_wrb_128k(vek[var_akt].b[1]);
}

void speichern_int(unsigned int u) {
	adr_128k = u * 2;
	ee_wrb_128k(vek[u].b[0]);
	++adr_128k;
	ee_wrb_128k(vek[u].b[1]);
}
void speichern_cal_mv(unsigned char ind, unsigned char n) {
	adr_128k  = ind;
	adr_128k *= 2;
	ee_wrb_128k(vek[n].b[0]);
	++adr_128k;
	ee_wrb_128k(vek[n].b[1]);
}

unsigned char txtDB_fn(unsigned char* ptr) {
	xdata unsigned int sprDB, 
	                   d; 
	xdata unsigned long txtStart,
	                    txtStop;
	xdata unsigned char //y, 
	                    tmp[40];
	                    
	memset(tmp,0,sizeof(tmp));
	adr_128k = 12;
	sprDB = adresse_2();
	d = atoi(ptr + 1);
	txtStart = sprDB + d*3 + anz_txt*3*(vek[sprache].ui - de_txt);
	txtStop  = txtStart + 3;
	adr_128k = txtStart; 
	txtStart = adresse_3(); 
	adr_128k = txtStop; 
	txtStop  = adresse_3(); 
	len_128k = txtStop - txtStart;
	ptr_128k = tmp;
	adr_128k = txtStart + OFFSET_EEPR;
	ee_rdseq_128k();
	
	if(flag)
		sprintf(tmp0+strlen(tmp0)," %s",tmp);
	else {
		if(row[strlen(row)-1]== ' ')
			sprintf(row + strlen(row),"%s ", tmp);
		else
			sprintf(row + strlen(row)," %s ", tmp);
	}
	return 0;
}

unsigned char umenu_fn(unsigned char* ptr) {
	if(cur_dir)
		dir_new = atoi(ptr + 1);
	sprintf(row+strlen(row),"~");
	return 0;
}

unsigned char var_fn(unsigned char* ptr) {
	xdata unsigned char val;
	xdata int ix;
	val = atoi(ptr + 1);
	ix = vek[val].i;	
	//~ ix = val;	
	if(cur_dir) {
		var_akt = val;
		if(MENU_MODE == 11)
			ix = var_wert;
	}	
	flag = 1;
	//~ if((val < anl_typ) || (val >= code0 && val <= status)) {
	if((val < anl_typ) || (val >= code0)) {
		xdata unsigned char k;
		k = komma(val);
		if(!k)
			sprintf(tmp0+strlen(tmp0),format[k], ix);
		else {
			xdata float fx;
			fx = ix;
			if(k == 1)      fx /= 10;
			else if(k == 2) fx /= 100;
			else            fx /= 1000;
			
			sprintf(tmp0+strlen(tmp0),format[k], fx);
		}
		val = 0;
	}
	return val;
}

unsigned char m1_fn(unsigned char* ptr) { return 0;}

unsigned char const_fn(unsigned char* ptr) {
	xdata unsigned char len;
	xdata unsigned char* p;
	if(flag) {
		len = strlen(tmp0);
		p = tmp0;
	} else {
		len = strlen(row);
		p = row;
	}
	while(*(++ptr) >= 0x20) {
		p[len] = *ptr;
		++len;
	}
	p[len] = 0;
	return 0;
}

//~ void line_v(unsigned char col, unsigned char row, unsigned char *plen) {
//~ void line_h(unsigned char col, unsigned char row, unsigned char *plen) {
	
void balken(unsigned char pos, unsigned char prc) {
	unsigned char n,
				  len,
				  str[3],
				  cl,
				  rw,
				  prc1;
	prc1 = prc / 6;
	cl = pos_anz[pos][0];
	rw = pos_anz[pos][1];
	for(n = 0, len = 2; n < prc1; ++n, ++len) {
		sprintf(str,"%bu",len);
		line_v(  cl, rw - len, str);
		line_v(++cl, rw - len, str);
		cl += 2;
	}
	switch(pos) {
		case 0:  n =  5, rw =  1, str[0] = 'I'; break;
		case 1:  n = 30, rw =  1, str[0] = 'U'; break;
		default: n =  5, rw = 87, str[0] = 'Q'; break;
	} 
	memset(row,0,sizeof(row));
	sprintf(row,"00 %3bu %3bu %c", rw, n + 11, str[0]); //
	display_wr(row);
	memset(row,0,sizeof(row));
	sprintf(row,"00 %3bu %3bu %bu",rw, n, prc); 
	strcat(row,"%");
	display_wr(row);
	
}

unsigned char tankMarker(unsigned char l, unsigned char overflow) {
	xdata float f;
	f  = 46 * (float)vek[l].ui ;
	f /= (float)vek[overflow].ui;
	return f;
}

void tanksM(void) {
	min_max[pmin] = tankMarker(prodlev_min, prodlev_ovfl);	
	min_max[pmax] = tankMarker(prodlev_max, prodlev_ovfl);	
	min_max[smin] = tankMarker(soleLev_min, soleLev_ovfl);	
	min_max[smax] = tankMarker(soleLev_max, soleLev_ovfl);	
	min_max[plm]  = tankMarker(lev_prod,    prodlev_ovfl);	
	min_max[slm]  = tankMarker(lev_sole,    soleLev_ovfl);	
}

void tank_lev(unsigned char col, unsigned char row, unsigned char lev, bit t) {
	xdata unsigned char x, b, str[3];
	line_v(col,    row,    "46");
	line_v(col+16, row,    "46");
	line_h(col,    row,    "16");
	line_h(col,    row+46, "17");
	for(x = 0; x < 9; ++x) {
		b = 46 + row - lev;
		str[0] = 0x30 + lev/10;
		str[1] = 0x30 + lev%10;
		str[2] = 0;
		line_v(col + 4 + x, b, str);
	}
	if(!t) x = 0;
	else   x = 2;
	line_h(col, 46 + row - min_max[pmin + x],"16");
	line_h(col, 46 + row - min_max[pmax + x],"16");
}
