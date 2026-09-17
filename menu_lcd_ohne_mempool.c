#include <stdlib.h>
#include <stdio.h>
#include <intrins.h>
#include <string.h>
#include "ConstDef.h"                 // SFR declarations
#include "eeprom128.h"
#include "eeprom_128drv.h"
#include "display.h"
#include "main.h"
#include "relais.h"
#include "regelung.h"
#include "PCF8563_WI.h"
#include "ma4_20.h"
#include "Bus_LCD.h"
#include "bus.h"
#include "regelung_ely.h"
#include "default_werte_ely.h"

#define  MENU_LENGTH 50
#define POT_NULL_MIN -1000

//~ #define REG_DES 0x7f
//~ #define REG_PH  0xfb
#define REG_PH  0x7f
#define REG_DES 0xfb
#define ESC     0xbf
#define ENTER   0xdf
#define UP      0xef
#define DOWN    0xf7
#define DOS     0xfe
#define MSR_ELY (UP & ESC)


#define ANF_SZ_ADR    OFFSET_EEPR +  3
#define ANZ_SPR_ADR   OFFSET_EEPR +  6
#define MENU_ANF_ADR  OFFSET_EEPR +  9
//~ #define MENU_ATTR_ADR OFFSET_EEPR + 12
#define ANF_TXT_ADR   OFFSET_EEPR + 15
#define ANZ_TXT_ADR   OFFSET_EEPR +  7

//~ #define VAR_TXT_LEN 8
#define VAR_TXT_LEN 6
#define DIR_VAR 300
#define TMP_30 30

extern code WERTE tab1[];
//~ extern code WERTE tab2[];
//~ extern code WERTE contr_dflt;
//~ code unsigned char unsichtbar[] = {4,18,19,26,97,106,125}; // ,126
//~ code unsigned char unsichtbar[] = {4,18,19,26,102,109,129}; // ,126
code unsigned char unsichtbar[] = {4,18,19,/*26,*/102,109,137}; // ,126

//~ extern code unsigned int *werte_el[];

// bit text_filter(unsigned int txt_nr);
//~ void var_print(unsigned char *ptr, float w);
void Wert_speichern(void);
void key_press(bit);
void menu_rt(void);
void Wert_grenze(unsigned int pos);
void txt_anf(unsigned int nmr);
void read_text(unsigned char*);
void lingua(void);
void speichern_int(unsigned int u);
bit txt_var_fn(unsigned int v);
void tanksM(void);
void tank_lev(unsigned char col, unsigned char row, unsigned char lev);
void strom_lev(unsigned char col, unsigned char row, unsigned int lev);
unsigned int prc_fn(unsigned int ist, unsigned int soll);
void set_nmbr_txt(unsigned int nr);


// code char* dir = " --->>>";
// code char* dir = " ~";
code char* striche = "---";
code char* fmt_sek = " %u s";
//~ code char* dino[] = {
	//~ "dinotec_GmbH",					//   
	//~ "Spessartstr._7",				//   
	//~ "D-63477-Maintal",				//   
	//~ "Tel:0049-(0)6109-6011-0",		//   
	//~ "Hotline:0049-(0)6109-6011-72", //  
	//~ "www.dinotec.de",				//   
	//~ "service@dinotec.de"			//   
//~ };

//~ code char* tklein = "C";

enum {
/* 0 */ vbw_typ,
/* 1 */ u_zelle,
/* 2 */ u_zahl,
/* 3 */ t_satz,
/* 4 */ t_zahl,
/* 5 */ i_prc,
/* 6 */ h_line1,
/* 7 */ h_line2,
/* 8 */ v_line,
/* 9 */ sts_line,
/*10 */ Q_prc,
/*11 */ leer
};

enum {pmin, pmax, plm};
code char* zellenschrift[] = 
{
	"U-Zelle,V           I",
	"U-cell,V            I",
};

enum {st_zeile = 9};

code char* basic[] =
{		 //  v   h
/*00*/	"00 000 000 VBW ",		
//~ /*01*/	"00 000 013 U-Zelle,V           I",		
/*01*/	"00 000 013 ",		
/*02*/	"01 060 005 ",		// U-Zahl 
/*03*/	"00 000 038 T,C                Q", 		
/*04*/	"01 060 030 ",		// T-Zahl
/*05*/	"00 125 005 ",		// I %
/*06*/	"03 000 025 220", 	// horiz. Linie
/*07*/	"03 000 050 240", 	// horiz. Linie
/*08*/	"04 118 000 050", 	// vert.  Linie
/*09*/	"00 000 054 ",	     // Statuszeile
/*10*/	"00 125 029 ",       // Q %
/*11*/	"00 120 026 ",       // Temperatur klein
};

code char* basic_msr[] =
{
/*00*/	"00 000 000 VBW ",		
/*01*/	"00 000 013 ",	// CL2 
/*02*/	"01 050 005 ",  // Gr. Schrift 
/*03*/	"00 000 038 ",	// pH
/*04*/	"01 050 030 ",  // Gr. Schrift
/*05*/	"03 000 025 240", // horiz. Linie
/*06*/	"03 000 050 240", // horiz. Linie
/*07*/	"04 118 000 050", // vert.  Linie
/*08*/	"00 120 013 ",    // Redox 
/*09*/	"00 000 054 ",
/*10*/	"01 175 005 ",    //  Redox
/*11*/	"00 130 038 ",    // T°C, L/h
/*12*/	"01 050 005 "    // Gr. Schrift CL2
};


code char* meih[] = {
	"mg/l", 
	" ˜/Ÿ", 
};	 



code unsigned char *Font0_Col0     = "00 000 ";

bit plus, 
	minus,
	KEY_PR,
	dpd_bit,
	show_msr,
	ph,cl,rx,
	uin_save_flg,
	uin_set_flg,
    wifi_reset,
    connected;
	

xdata RRAM rram;
xdata unsigned char txt_cnt_max,
					txt_dflt[ZEILEN][ZEICHEN],
					anz_spr,
					MENU_MODE,
					KEY,
					key_del,
					s_zeichen[250],
					status_cnt,
					min_max[3],
					nmbr_txt,
					ssid[15],
					serv_cnt;
char wlan_sig;
//~ idata  unsigned char MENU_MODE;
				
xdata menustack menu_stack;	
char uin_cnt;

unsigned int	offset;

unsigned long	menu_anf,
				menu_attr;


static xdata unsigned char blinken;
static xdata signed char speed;
static xdata unsigned char sts_ctr;
static xdata unsigned char speed_cnt;
//~ static xdata unsigned char index;
xdata int var_wert;
xdata int menu_vec[MENU_LENGTH];
xdata float flw_cmp;

xdata struct var_act vr;

unsigned long adresse(bit pos) {
	xdata unsigned long x;
	x = ee_rdb_128k(); //  ++adr_128k int ee_rdb_128k
	x <<= 8;
	x |= ee_rdb_128k(); //  ++adr_128k int ee_rdb_128k
	if(pos) {	
		x <<= 8;
		x |= ee_rdb_128k(); //  ++adr_128k int ee_rdb_128k
	}
	return x;
}


void lingua(void) {
	adr_128k = ANF_SZ_ADR;
	adr_128k = adresse(1); 
	adr_128k += vek[sprache_t].ui * 3;
	adr_128k += OFFSET_EEPR;
	adr_128k = adresse(1); 
	adr_128k += OFFSET_EEPR;
	len_128k = sizeof(s_zeichen);
	memset(s_zeichen, 0xff, len_128k);
	ptr_128k = s_zeichen;
	ee_rdseq_128k(0);
}

void flw_cmp_fn(void) {flw_cmp = vek[flow_komp].i / 10000.0;}

void menu_ini(void) {
	xdata unsigned char txt_cnt;
	//~ xdata unsigned int ix;
	WDT_RT();
	adr_128k = ANZ_SPR_ADR;
	anz_spr  = ee_rdb_128k();
	//~ ix = vek[sprache_t].ui;
	//~ ix = 0;
	if(vek[sprache_t].ui >= anz_spr) vek[sprache_t].ui = 0;
	adr_128k   = ANZ_TXT_ADR;
	offset     = adresse(0); // Anzahl der Texten
	offset    *= 3;
	adr_128k   = MENU_ANF_ADR;
	menu_anf   = adresse(1);
	menu_anf   += OFFSET_EEPR;
	adr_128k   = MENU_ATTR_ADR;
	menu_attr  = adresse(1);
	menu_attr += OFFSET_EEPR;
	
	lingua();
    i_anteil();
	regelung_ini();
	flw_cmp_fn();
	//~ des_alarm_fn();
	memset(&menu_stack, 0, sizeof(menu_stack));
	memset(menu_vec,    0, sizeof(menu_vec));
	sts_ctr = 0;
	
	menu_stack.menu_nr[0] = 0;
	menu_stack.txt_nr[0]  = 1;
	menu_stack.pos[0]     = 1;
	dpd_bit  = 0;
	nmbr_txt = 0;
	//~ show_msr = 0;
	//---------------
	MENU_MODE  = 0;
	plus       = 0;
	minus      = 0;
	blinken    = 0;
	speed      = 0;
	speed_cnt  = 0;
	KEY_PR     = 0;
	key_del    = 0;
	uin_save_flg = 0;
	uin_set_flg  = 0;
	memset(&vr,0,sizeof(vr));
	memset(&rram, 0, sizeof(rram));
	//--------------------------------------------------------
				txt_cnt_max = sizeof(txt_dflt)/sizeof(txt_dflt[0]);
				for(txt_cnt = 0; txt_cnt < txt_cnt_max; ++txt_cnt) {	
					memset(txt_dflt[txt_cnt],0,sizeof(txt_dflt[txt_cnt]));
					if(!show_msr) {
						strcpy(txt_dflt[txt_cnt], basic[txt_cnt]);
						if(txt_cnt == 1) {
							strcat(txt_dflt[txt_cnt], zellenschrift[!vek[sprache_t].ui==0]); 
						}
					} else
						strcpy(txt_dflt[txt_cnt], basic_msr[txt_cnt]);
				}
				txt_anf(vek[anl_typ_t].ui);
				read_text(txt_dflt[vbw_typ]);
				cl = vek[msr_t].b[1] < ph_txt;
				ph = vek[msr_t].b[1] > cl_txt;
				rx = vek[msr_t].b[1] == cl_ph_rx_txt || vek[msr_t].b[1] == ph_rx_txt ;


				//~ txt_anf(des_dspl);
				//~ read_text(txt_dflt[0] + strlen(txt_dflt[0]));
				//~ strcat(txt_dflt[0],medien[vek[des].ui - cl_txt]);
				//~ if(vek[sprache].ui == 5 || vek[sprache].ui == 6)
					//~ strcat(txt_dflt[0],meih[1]);
				//~ else	
					//~ strcat(txt_dflt[0],meih[0]);
				
				//~ strcat(txt_dflt[0]," ");
				//~ txt_anf(rx_dspl);
				//~ read_text(txt_dflt[0] + strlen(txt_dflt[0]));
				//~ 
				//~ txt_anf(q_dspl);
				//~ read_text(txt_dflt[2] + strlen(txt_dflt[2]));
				//~ if(vek[ph_mess].ui == ja_txt) {
					//~ strcat(txt_dflt[2]," ");
					//~ txt_anf(ph_dspl);
					//~ read_text(txt_dflt[2] + strlen(txt_dflt[2]));
				//~ }
				//~ else {
					//~ wasser_bild();
					//~ strcat(txt_dflt[2],"                  ");
					//~ if(vek[t_comp].ui == auto_txt)
						//~ strcat(txt_dflt[2],tklein);
				//~ }
}

unsigned char komma(unsigned int nr) {
	unsigned char c;
	switch(nr) {
		case cl_dpd_in: case cl_val_ix:  case cl_alr_ob: case cl_wr_ob: case cl_soll:
		case cl_wr_un:  case cl_alr_un:  case cl_p:      case cl_hys: 
		case ph_ix:     case ph_dpd2_in: case ph_alr_ob: case ph_wr_ob: case ph_soll:
		case ph_wr_un:  case ph_alr_un:  case ph_p:      case ph_hys:   case totband_ph:
			c = 2; 
		break;
		case t0_ix: case t_korr:  case t0_korr:         
		case cl_sth_in: case cl_1_sth_in: case cl_konz:
		case r3_min: case r4_min: //case flow_ely:
			c = 1;
		break;
		default:
			c = 0;
		break;	
	}
	if(vek[mgl_t].ui == mgl_1_txt) {
		if(nr==cl_val_ix || (nr >= cl_alr_ob && nr <= cl_alr_un))
			c = 3;
	}
	return c;
}

void add_var(unsigned char rw, unsigned char *p) {
	xdata unsigned char cnt, c;
	cnt = strlen(txt_dflt[rw]);
	//~ c = 37 - (cnt - strlen(Font0_Col0) - 6) - strlen(p);
	c = 37 - (cnt - strlen(Font0_Col0) - 6);
	if(strlen(p) > c)
		c = 1;
	else
		c -= strlen(p);	
	for(; c > 0; --c) {
		txt_dflt[rw][cnt++] = ' ';
	}
	txt_dflt[rw][cnt] = 0;
	strcat(txt_dflt[rw],p);
}


void zeile_fuellen(unsigned char zeile, unsigned int text_nr) {
	xdata unsigned char cnt;
	xdata signed char c;
	memset(txt_dflt[zeile], 0, sizeof(txt_dflt[0]));
	
	#define zf_buf rram.tmp_buf[4] // rram.tmp_buf[1]
	memset(zf_buf, 0, LEN_60);
	
	if(!zeile)
		sprintf(txt_dflt[zeile], "%s%03d ", Font0_Col0, (int)0);	
	else
		sprintf(txt_dflt[zeile], "%s%03d   ",Font0_Col0, (int)zeile*8);	
	
	txt_anf(text_nr);
	read_text(txt_dflt[zeile]);
	
	if(zeile == menu_stack.pos[0]) {
		txt_dflt[zeile][11] = '>';
		set_nmbr_txt(text_nr);
	}

	c = strpos(txt_dflt[zeile], 0x01);
	if(c != -1) { // Variable gefunden
		vr.pos[zeile] = c;
		vr.nr[zeile]  = atoi(txt_dflt[zeile] + 1 + c);
		if(vr.nr[zeile] >= DIR_VAR) { // Verzeichnis
			txt_dflt[zeile][c] = 0;
			if(zeile) { 
				for(cnt = ZEICHEN - 1; cnt > 13; --cnt)
					txt_dflt[zeile][cnt] = txt_dflt[zeile][cnt-1];
				txt_dflt[zeile][13] = '~';//dir[0];
			}
			c = strpos(txt_dflt[zeile], '[');
			if(c != -1) {
				text_nr = vek[r3_strg_t].ui;
				memset(zf_buf, 0, LEN_60);
				strcpy(zf_buf, txt_dflt[zeile] + c + 1);
				txt_dflt[zeile][c] = 0;
				txt_anf(text_nr);
				read_text(txt_dflt[zeile]);
				strcat(txt_dflt[zeile], zf_buf);
			} else {
				c = strpos(txt_dflt[zeile], ']');
				if(c != -1) {
					text_nr = vek[r4_strg_t].ui;
					memset(zf_buf, 0, LEN_60);
					strcpy(zf_buf, txt_dflt[zeile] + c + 1);
					txt_dflt[zeile][c] = 0;
					txt_anf(text_nr);
					read_text(txt_dflt[zeile]);
					strcat(txt_dflt[zeile], zf_buf);
				}
			}
		} else {
			unsigned char km;
			//~ memset(zf_buf, 0, LEN_60);
			cnt = c;
			txt_dflt[zeile][vr.pos[zeile]] = 0;
			if(vr.nr[zeile] == ip_en) {
				sprintf(zf_buf, "%bu.%bu.%bu.%bu", ip_adr.b[0], ip_adr.b[1], ip_adr.b[2], ip_adr.b[3]);
			} else if(vr.nr[zeile] == ssid_en) {
				km = strlen(ssid);
				strncpy(zf_buf, ssid, km);
				zf_buf[km] = 0;
			} else if(vr.nr[zeile] == sign_en) {
				int ix;
				ix = (100 + wlan_sig)*2;
				if(ix < 0)
					ix = 0;
				if(ix > 100)
					ix = 100;			
				sprintf(zf_buf, "%3d", ix);
			} else if(vr.nr[zeile] == conn_en) { 
				if(connected)
					txt_anf(ja_txt);
				else
					txt_anf(nein_txt);
				read_text(zf_buf);
			} else if(vr.nr[zeile] == server_en) { 
				sprintf(zf_buf, "%3bu", serv_cnt);
			} else if(vr.nr[zeile] == wreset_en) { 
				txt_anf(nein_txt);
				read_text(zf_buf);
			}  
			else if(txt_var_fn(zeile)) {
				if(vr.nr[zeile] == sprache_t)
					txt_anf(vek[sprache_t].ui + sprachen_txt);
				else
					txt_anf(vek[vr.nr[zeile]].ui);
				//~ read_text(txt_dflt[zeile]);
				read_text(zf_buf);
			} else {
				unsigned int ix;
				if(vr.nr[zeile] == cl_sth_in)
					vr.nr[zeile] = cl_sth;
				ix = vek[vr.nr[zeile]].ui;
				km = komma(vr.nr[zeile]);
				if(km==3)
					sprintf(zf_buf, "%6.3f", (float)vek[vr.nr[zeile]].i / 1000.0);
				//~ else if(km==4)
					//~ sprintf(tmp,"%6.3f", (float)vek[vr.nr[zeile]].i / 1000.0);
				else if(km==2)
					sprintf(zf_buf, " %5.2f", (float)vek[vr.nr[zeile]].i / 100.0);
				else if(km==1)
					sprintf(zf_buf, " %5.1f", (float)vek[vr.nr[zeile]].i / 10.0);
				else if(km==62)
					sprintf(zf_buf, " %7.2f", (float)vek[vr.nr[zeile]].i / 100.0);
				else
					sprintf(zf_buf, " %5d", vek[vr.nr[zeile]].i);
			} 
		}
		add_var(zeile, zf_buf);
	} else
		vr.pos[zeile] = sizeof(txt_dflt[zeile]);
	memset(zf_buf, 0, LEN_60);	
}


void txt_anf(unsigned int nmr) {
	adr_128k  = ANF_TXT_ADR;
	adr_128k += (nmr - 1) * 3;
	adr_128k += offset * vek[sprache_t].ui;
	adr_128k  = adresse(1);
	adr_128k += OFFSET_EEPR;
	adr_128k  = adr_128k;
}

void read_text(unsigned char *ptr) {
	#define rt_buf rram.tmp_buf[2]
	
	ptr_128k = rt_buf;
	len_128k = LEN_60;
	ee_rdseq_128k(0);
	strcat(ptr, rt_buf);
	memset(rt_buf, 0, LEN_60);
}

bit text_ok(unsigned int nr) {
	bit b;
	xdata unsigned char i;
	
	for(i = 0; i < sizeof(unsichtbar); ++i) {
		if(nr == unsichtbar[i])
			break;
	}
	//~ if(i < sizeof(unsichtbar) && vek[code0_in].ui != 178)
	//~ if(i < sizeof(unsichtbar) && (vek[code0_in].ui != 178 || vek[code0_in].ui != 221)) Fehler!
	if(i < sizeof(unsichtbar) && !(vek[code0_x].ui == 178 || vek[code0_x].ui == 221))
		return 0;
	//~ #ifndef TEST
		//~ if(nr == 110) // Durchfluss Elektrolyse
			//~ return 0;
	//~ #endif
	if(nr == 110) {
		if(vek[code0_x].ui != 221)
			return 0;
	}
	
	b = 1;

	switch(nr) {
		case calibr_txt:    
			b  = (vek[msr_t].ui >= cl_txt & vek[msr_t].ui <= ph_rx_txt);
			//~ b &= ~INIT_DELAY; 
		break;
		case m_rangeCL2_txt:
		case CL_pH_txt:
			b  = vek[msr_t].ui >= cl_txt & vek[msr_t].ui <= cl_ph_rx_txt; 
			if(cl_ph_comp)
				vek[set_nt].ui = ja_txt;
			else	
				vek[set_nt].ui = nein_txt;
			nmbr_txt = nr;	
		break;
		case soll_mess_txt: b = vek[msr_t].ui != no_msr_txt; break;
		case soll_Cl2_txt:
		case cal_CL2_txt: 
			b = (vek[msr_t].ui >= cl_txt & vek[msr_t].ui <= cl_ph_rx_txt);		
		break;
		case cal_pH_txt:
		case soll_pH_txt:   
			b = vek[msr_t].ui != cl_txt; 
		break;
		case soll_rx_txt:   b = vek[msr_t].ui == ph_rx_txt; break;
		
		case r3_frq_txt:    b = vek[r3_art_t].ui == puls_frq_txt; break;
		case r3_pp_txt:
		case r3_min_txt:
			b = vek[r3_art_t].ui == puls_pause_txt; 
		break;
		case r4_frq_txt: b = vek[r4_art_t].ui == puls_frq_txt; break;
		case r4_pp_txt:
		case r4_min_txt:
			b = vek[r4_art_t].ui == puls_pause_txt; 
		break;
		case ende_AER_txt:
		case ende_AER_txt + 1:
			b = vek[aer_min_ix].ui > 0; 
		break;
		case datum_txt: case aer_txt:
			b = vek[msr_t].ui == ph_rx_txt || vek[msr_t].ui == ph_txt;
			b = ~b;
		break;
		//~ case cal_rsCL_txt: case Uein_save_txt: case U0_set_txt:
			//~ nmbr_txt = nr;
		//~ break;
		case cal_rspH_txt: case ddosCl2_txt: case cal_rsCL_txt:  
		case ddosPH_txt:   case ddosRX_txt:  case aer_start_txt: case werk_rs_txt: 
			nmbr_txt = nr;
		break;
		case flow_mess_txt: case flow_rate_txt: case flow_min_txt: case flow_cmp_txt: // Durchfluss oder Schwimmerschalter
			b = vek[mw_art_t].ui == flow_txt;
		break;
		//~ case 8: // Mittelwertbildung
		case cl_konz_txt: 
		//~ case sole_dos_txt: 
			b = 0;
		break;	
		case Uein_saved_txt:
			b = uin_save_flg;
		break;
		case U0_set_txt: case (U0_set_txt-1):
			//~ nmbr_txt = nr;
			b = vek[cl_mv_ix].i < 0;
		break;
		case val_low1_txt: case val_low2_txt:
			b = 0;
		break;
		case t_kompens_txt:
			if(vek[lev_art_t].b[1] == swimm_txt) {
				vek[t_comp_t].b[1] = man_txt;
				b = 0;
			}
		break;
		case ventil_txt:
			b = ventilator_rl();
		break;
		case prodlev_ovfl_txt: case prodlev_max_txt: case prodlev_min_txt:
			if(vek[lev_art_t].b[1] == swimm_txt) {
				b = 0;
			}
		break;
		case r3_art_txt:
			b = vek[r3_strg_t].ui != ventilator_txt;
		break;
		case r4_art_txt:
			b = vek[r4_strg_t].ui != ventilator_txt;
		break;
		case notbetrieb_txt: case notlaufzeit_txt:
			b = vek[lev_art_t].ui == clm_txt;
		break;
	}
	#if 0
	{
		xdata unsigned char v;
		v = (vek[mgl_t].ui == mgl_10_txt) ? (10) : (5);
		if(vek[cl_val_ix].i < v) {
			if(nr >= 16 && nr <=  20) b = 0;
			if(v==10) {
			    if(nr == 24) b = 1;
			    if(nr == 25) b = 0;
			} else {
			    if(nr == 24) b = 0;
			    if(nr == 25) b = 1;
			}
		    if(nr == 23) b = 1;
		} else if(nr == 24 || nr==25)
			b = 0;
	}
	#endif
	return b;
}
/*
bit text_ok(unsigned int nr) {
	bit b;
	xdata unsigned char t,h;
	b = 0;
	adr_128k = nr + menu_attr - 1;
	t = ee_rdb_128k();
	h = t & 0x03; 
	if(!h || (h==1 && (vek[code0].ui==87 || vek[code0].ui == 178)) || (h==2 && vek[code0].ui==178))
		b = 1;
	//~ b |= (vek[code0].ui==22) ;
	h = (t >> 2) & 0x07;
	
	h = (t >> 5) & 0x07;
	//~ switch(h) {
		//~ case 0x01: // pH-Kalibrierung
			//~ b = (vek[ph_mess].ui == ja_txt);
		//~ break;
		//~ case 0x02: // Redox-Kalibrierung
			//~ b = (vek[ph_mess].ui == nein_txt);
		//~ break;
		//~ case 0x04:	// Kombi-, und mengeprop
			//~ b = (vek[strg_art].ui != menge_txt);
		//~ break;
		//~ default: break;
	//~ }
	switch(nr) {
		case t_comp_txt:
			b = (vek[t_comp_txt].ui != auto_txt);
		break;
		//~ case asr1_txt: case asr2_txt: case asr3_txt: case asr7_txt:
		//~ case asr4_txt: case asr5_txt: case asr6_txt: case asr8_txt:
			//~ b = asr30min > 0;
		//~ break;
		//~ case asr1_txt: case asr3_txt:  
		//~ case asr5_txt: case asr7_txt:  
			//~ b = (asr30min > 0);
		//~ break;
		//~ case asr2_txt: case asr4_txt: 
		//~ case asr6_txt: case asr8_txt: 
			//~ b = (asr30min > 0) | cal_no;
		//~ break;
		//~ case cl_dpd_txt: case pc_dpd_txt: case oz_dpd_txt: case cldx_dpd_txt:
			//~ b = asr30min == 0;
		//~ break;
		//~ case menge_reg_txt: 
			//~ b = (vek[strg_art].ui != mess_txt);
		//~ break;
		//~ case rel_txt:
			//~ b = (vek[strg_art].ui == mess_txt);
		//~ break;
		//~ case kombi_t:
			//~ b = (vek[strg_art].ui == kombi_txt);
		//~ break;
		//~ case hersteller:
			//~ if(vek[oem].ui == 0xabcd)
				//~ b = 0;
		//~ break;
		default:break;
	}
	return b;
}
void ph_calibrierung(void)
{
	if(vek[ph_cal_art].ui == punkt_2_txt)
	{	
		//~ if(vek[ph_mv].i >= tab1[ph_null].min && vek[ph_mv].i <= tab1[ph_null].max)
		if(ph_mv_corr >= tab1[ph_null].min && ph_mv_corr <= tab1[ph_null].max)
		{
			vek[ph_dpd2].i = 700;
		}
		// else if(vek[ph_mv].i >= U4_min && vek[ph_mv].i <= U4_max)
		else if(ph_mv_corr >= tab1[U4].min && ph_mv_corr <= tab1[U4].max)
		{
			vek[ph_dpd2].i = 400;
		}
		else
			vek[ph_dpd2].i = 0;
	}
}
*/

unsigned char menu_fuellen(void) {
	xdata unsigned char row;
	xdata unsigned int i, mn, text_nr;
	xdata unsigned long	menu_ptr;	
	
	memset(txt_dflt, 0, sizeof(txt_dflt));
	// richtige Menu finden
	memset(&rram, 0, sizeof(rram));
	adr_128k = menu_anf;//+ OFFSET_EEPR;
	len_128k = LEN_60; // sizeof(rram.buf_tmp);
	#define mf_buf rram.tmp_buf[3]
	ptr_128k = mf_buf;
	ee_rdseq_128k(0);
	menu_ptr = menu_anf;
	text_nr  = menu_stack.menu_nr[0];
	for(i = 0, mn = 0; mn < menu_stack.menu_nr[0]; i += 2) {
		//~ if(i == sizeof(rram.buf_tmp)) {
		if(i == LEN_60) {
			//~ memset(&rram, 0, sizeof(rram));
			memset(mf_buf, 0, LEN_60);
			//~ menu_ptr = menu_anf + sizeof(rram.buf_tmp);
			//~ menu_ptr += sizeof(rram.buf_tmp);
			menu_ptr += LEN_60;
			adr_128k = menu_ptr; // + OFFSET_EEPR;
			len_128k = LEN_60; // sizeof(rram.buf_tmp);
			ptr_128k = mf_buf; // rram.buf_tmp;
			WDT_RT();
			ee_rdseq_128k(0);
			i = 0;
		}
		//~ if(rram.buf_tmp[i]==0 && rram.buf_tmp[i + 1]==0)
		if(mf_buf[i]==0 && mf_buf[i + 1]==0)
			++mn;
	}
	menu_ptr += i;
	//---- menu_vec[] füllen
	memset(menu_vec, 0, sizeof(menu_vec));
	memset(&rram, 0, sizeof(rram));
	adr_128k = menu_ptr;// + OFFSET_EEPR;
	len_128k = MENU_LENGTH * 2;
	ptr_128k = mf_buf; // rram.buf_tmp;
	ee_rdseq_128k(0);
	for(row = 0,i = 0; row < MENU_LENGTH; ++row) {
		//~ text_nr = rram.buf_tmp[row*2];
		text_nr = mf_buf[row*2];
		text_nr <<= 8;
		//~ text_nr |= rram.buf_tmp[row*2 + 1];
		text_nr |= mf_buf[row*2 + 1];
		if(text_nr) {	
			if(text_ok(text_nr))
				menu_vec[i++] = text_nr;
		}
		else 
			break;
	}
	// 1. Zeile
	text_nr = menu_vec[0];
	zeile_fuellen(0, text_nr);
	
	//~ rram.buf_tmp[0] = menu_stack.txt_nr[0] - menu_stack.pos[0] + 1; 
	mf_buf[0] = menu_stack.txt_nr[0] - menu_stack.pos[0] + 1; 
	for(row = 1; row < 8; ++row) {
		//~ text_nr = menu_vec[rram.buf_tmp[0] + row - 1];
		text_nr = menu_vec[mf_buf[0] + row - 1];
		if(text_nr) {	
				zeile_fuellen(row, text_nr);
		}
		else
			break;
	}
	memset(&rram, 0, sizeof(rram));
	return row;
}	

void set_nmbr_txt(unsigned int nr) {
	if (nmbr_txt == cal_rsCL_txt) {
		if(nr == Uein_save_txt || nr == U0_set_txt)
			nmbr_txt = nr;
	}
}

void cursor_nach_unten(void) {	
	//~ xdata unsigned int text_nr;
	#define text_nr_c rram.tmp_buf[15][0]
	txt_dflt[menu_stack.pos[0]][11] = ' '; 
	++menu_stack.pos[0];
	++menu_stack.txt_nr[0];
	text_nr_c = menu_vec[menu_stack.txt_nr[0]];
	if(text_nr_c) {
		set_nmbr_txt(text_nr_c);	
		if(menu_stack.pos[0] > 7) {	
			//~ xdata unsigned char i;
			#define cnu_i rram.tmp_buf[15][1]
			for(cnu_i = 1; cnu_i < 8; cnu_i++) {	
				memcpy(txt_dflt[cnu_i] + 12, txt_dflt[cnu_i + 1] + 12,sizeof(txt_dflt[cnu_i]) - 11);
			}
			zeile_fuellen(7, text_nr_c);
			menu_stack.pos[0] = 7;
			cnu_i = 0;
		}
	}
	else {
		// menu_ptr -= 2;
		--menu_stack.txt_nr[0];
		--menu_stack.pos[0];
	}
	txt_dflt[menu_stack.pos[0]][11] = '>'; 
	text_nr_c = 0;
}
	
	
void cursor_nach_oben(void) {// Cursor nach oben
	//~ #define text_nro rram.tmp_buf[15][0]

	//~ xdata unsigned int text_nr;
	if(menu_stack.txt_nr[0] > 1) {	
		txt_dflt[menu_stack.pos[0]][11] = ' '; 
		--menu_stack.txt_nr[0];
		--menu_stack.pos[0];
		// menu_ptr -= 2;
		if(menu_stack.pos[0] < 1) {
			//~ xdata unsigned char i;
			//text_nr = adresse(menu_ptr, 0);
			text_nr_c = menu_vec[menu_stack.txt_nr[0]];
			set_nmbr_txt(text_nr_c);
			for(cnu_i = 7; cnu_i > 1; --cnu_i) {	
				memcpy(txt_dflt[cnu_i] + 11,txt_dflt[cnu_i - 1] + 11,sizeof(txt_dflt[cnu_i]) - 11);
			}
			zeile_fuellen(1, text_nr_c);
			menu_stack.pos[0] = 1;
		}
		txt_dflt[menu_stack.pos[0]][11] = '>'; 
		text_nr_c = 0;
	}
}

//~ unsigned char sth_n(unsigned char n) {
	//~ switch(n) {
		//~ case   cl_reset: n = cl_sth; break; 
		//~ case cldx_reset: n = cldx_sth; break; 
		//~ case   pc_reset: n = pc_sth; break; 
		//~ case   oz_reset: n = oz_sth; break; 
	//~ }
	//~ return n;
//~ }
//~ unsigned char null_n(unsigned char n) {
	//~ switch(n) {
		//~ case   cl_reset: n = cl_null_mv; break; 
		//~ case cldx_reset: n = cldx_null_mv; break; 
		//~ case   pc_reset: n = pc_null_mv; break; 
		//~ case   oz_reset: n = oz_null_mv; break; 
	//~ }
	//~ return n;
//~ }
//~ void uin_saved_fn(void) {
	//~ switch(vek[des].ui){
		//~ case pc_txt:    vek[uin_saved].i = vek[pc_mv].i;   break;
		//~ case oz_txt:    vek[uin_saved].i = vek[oz_mv].i;   break;
		//~ case cldx_txt:  vek[uin_saved].i = vek[cldx_mv].i; break;
		//~ default:        vek[uin_saved].i = vek[cl_mv].i;   break;
	//~ }
//~ }

//~ void key_press(void) {
void key_press(bit wf_upd) {
	static xdata unsigned char key_pr = 0xff;
	static xdata unsigned int res_del;// ansaugen;
	
	if(wf_upd) {
		goto AKTUALISIEREN;	
	}
	
	KEY |= 0x02;	// wegen LED_FLOW
	
	if(key_pr != KEY) {	
		KEY_PR = 1;
		min = 0;
		switch(KEY) {	
			case ESC: // Esc
				//~ if(ON_OFF || !MENU_MODE) {
					//~ if(!COLLECT_ALR)
						//~ btr_art_b0 = vek[btr_art_t].b[1];
						//~ btr_art_b0 = standby_txt;
					//~ break;
				//~ }
				if(MENU_MODE==11) {
					MENU_MODE = 1;
					blinken = 0;
				} else {
					if(menu_stack.size > 0)
						MENU_MODE = 10;
					else
						menu_ini();
				}
			break;	
			case ENTER: // Enter
				if(key_del)	{
					--key_del;
					break;
				}
				else
					key_del = 0xff;
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
						blinken = 0;
						if(vek[vr.nr[menu_stack.pos[0]]].ui != var_wert) {	
							if(vr.nr[menu_stack.pos[0]] < code0_x) {
								vek[vr.nr[menu_stack.pos[0]]].ui = var_wert;
								Wert_speichern();
							} else if(vr.nr[menu_stack.pos[0]] == wreset_en) {
								//~ wreset = var_wert;
								if(var_wert == ja_txt) {
									wifi_reset = 1;
								}
							}
						}
						MENU_MODE = 1;
						AKTUALISIEREN:
						switch(vr.nr[menu_stack.pos[0]]) {
							case code0_x:
								if(vek[code0_x].ui == 800)
									wifi_reset = 1;
									//~ vek[modbus_in].b[1] = ~vek[modbus_in].b[1];
								//~ adr_128k = modbus_in * 2;
								//~ ee_wrb_128k(vek[modbus_in].b[1]);
								//~ modbus_GW();	
							break;
							case sprache_t: 
								lingua(); 
							break;
							case std_int: case min_int: case day_int: case mon_int: case year_int:
								new_time();
							break;
							case anl_typ_t: 
							case r3_art_t: case r3_frq: case r3_pp: case r3_min:
							case r4_art_t: case r4_frq: case r4_pp: case r4_min:
								ely_ini();
								rls = 0;
							break;
							case mgl_t:
								rls = 0;
								if(vek[mgl_t].ui == mgl_10_txt) cl_sth = cl_sth_in;
								else                            cl_sth = cl_1_sth_in;
								POT_STH_ERR = test_var(cl_sth);
							break;
							case mw_art_t:
							case lev_art_t:
								rls = 0;
							break;
							case btr_art_t:
								if(vek[btr_art_t].b[1] == cellfill_txt) {
									set_sec(zelle_fill_sek);
									btr_art_b0  = cellfill_txt;
									start_vgang = 0;  
								} else if(vek[btr_art_t].b[1] == anl_fill_txt) {
									set_sec(anl_fill_sek);
									btr_art_b0  = anl_fill_txt;
									start_vgang = 1;  
								} else if(vek[btr_art_t].b[1] == run_txt || vek[btr_art_t].b[1] == solefill_txt) {
									btr_art_b0  = solefill_txt;
									start_vgang = 0;  
								} else if(vek[btr_art_t].b[1] == aus_txt)
									btr_art_b0 = aus_txt;  
								
								dly_btr = BTR_VERZ;
							case sole_dos:	
								ely_ini();
							break;	
							case r3_strg_t:
							case r4_strg_t:
								relais_ini_msr();
							break;
							case ph_dpd2_in: 
								if(ph_mv_corr >= tab1[ph_null_in].min && ph_mv_corr <= tab1[ph_null_in].max) {
									vek[U7_in].i = ph_mv_corr;
									speichern_int(U7_in);
								} 
								else if(ph_mv_corr >= tab1[U4_in].min && ph_mv_corr <= tab1[U4_in].max) {
									vek[U4_in].i = ph_mv_corr;
									speichern_int(U4_in);
								}
								SteilheitPH();
							break;
							case set_nt:
								if(var_wert == ja_txt) {
									switch(nmbr_txt) {
										case CL_pH_txt:
											cl_ph_comp = 1;
											adr_128k = cl_ph_saved * 2;
											ee_wrb_128k(SAVE_FLAG);
										break;
										case cal_rspH_txt:
											vek[U7_in].i = tab1[U7_in].dflt;
											speichern_int(U7_in);
											vek[U4_in].i = tab1[U4_in].dflt;
											speichern_int(U4_in);
											SteilheitPH();
										break;
										case aer_start_txt:
											ASR_MAN = 1;
										break;	
										case werk_rs_txt:
											Set_default();
										break;
										case ddosCl2_txt:
											ddos_fn(cl_rl,cl_man_dos,1);	
										break;	
										case ddosRX_txt:
											ddos_fn(rx_rl,rx_man_dos,1);	
										break;	
										case ddosPH_txt:
											ddos_fn(phdown_rl,ph_man_dos,1);	
											ddos_fn(phup_rl,ph_man_dos,1);	
										break;
										case Uein_save_txt:
											uin_cnt      = vek[min_int].b[1];
											uin_save_flg = 1;
											uin_set_flg  = 1;
										break;
										case U0_set_txt:
											if(vek[cl_mv_ix].i >= tab1[cl_mv0_in].min && vek[cl_mv_ix].i <= tab1[cl_mv0_in].max) {
												vek[cl_mv0_in].i = vek[cl_mv_ix].i;
												speichern_int(cl_mv0_in);
											}
										break;
										case cal_rsCL_txt:
											//~ vek[cl_sth_in].i = tab1[cl_sth_in].dflt;
											vek[cl_sth].i = tab1[cl_sth].dflt;
											speichern_int(cl_sth);
											POT_STH_ERR = 0;
											
										break;	
										default: break;
									}
									vek[set_nt].ui = nein_txt;
								} else {
									switch(nmbr_txt) {
										case CL_pH_txt:
											cl_ph_comp = 0;
											adr_128k = cl_ph_saved * 2;
											ee_wrb_128k(0xff);
										break;
										case ddosCl2_txt:
											ddos_fn(cl_rl,cl_man_dos,0);	
										break;	
										case ddosRX_txt:
											ddos_fn(rx_rl,rx_man_dos,0);	
										break;	
										case ddosPH_txt:
											ddos_fn(phdown_rl,ph_man_dos,0);	
											ddos_fn(phup_rl,ph_man_dos,0);	
										break;
										case Uein_save_txt:
											uin_save_flg = 0;
										break;
										case U0_set_txt:
											vek[cl_mv0_in].i = 0;	
											speichern_int(cl_mv0_in);
										break;
										//~ case cal_rsCL_txt:
											//~ if(menu_stack.pos[0]==4)
												//~ uin_save_flg = 0;
											//~ else if(menu_stack.pos[0]==6) {
												//~ vek[cl_mv0_in].i = 0;	
												//~ speichern_int(cl_mv0_in);
											//~ }
										//~ break;	
										default: break;
									}
								}   
							break;
							case cl_dpd_in: 
								dpd_bit = 1;
							break;
							case on_dly :
								on_delay = vek[on_dly].ui * 60;
							break;	
							case flow_komp:
								flw_cmp_fn();
							break;
							case cl_alr_dly:
							case alr_dly_rx:
								des_alr_delay = set_alr_dly(vr.nr[menu_stack.pos[0]]);
							break;
							case alr_dly_ph:
								ph_alr_delay = set_alr_dly(alr_dly_ph);
							break;
							case contr:
								displ_rst = 0;
							break;
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
				}
				else if(MENU_MODE != 11)
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
				}
				else if(MENU_MODE != 11)
						MENU_MODE = 6;
			break;	
			case DOS: // Regelung 
				//~ ON_OFF = ~ON_OFF;
				//~ adr_128k = status_ix * 2;
				//~ ee_wrb_128k((unsigned char)ON_OFF);
				if(ON_OFF) {
					P2     |= 0x7f;
					sec_cnt = 0;
					btr_art_b0 = aus_txt;
				} else {	
					//~ ee_ini();
					//~ menu_ini();
					//~ ely_ini();
					if(!show_msr) {
						if(vek[btr_art_t].b[1] != aus_txt) {
							vek[btr_art_t].b[1] = aus_txt;
							if(ventilator_rl())
								luft_cnt_nach = vek[luft_nach].ui * 60;
							else	
								luft_cnt_nach = 0;
						} else	
							vek[btr_art_t].b[1] = run_txt;
						
						speichern_int(btr_art_t);
						
						if(vek[btr_art_t].b[1]== run_txt)
							btr_art_b0 = solefill_txt;
						else	
							btr_art_b0 = vek[btr_art_t].b[1];
					} else {
						dos_aus = ~dos_aus;
						adr_128k   = (int)status_ix * 2;
						ee_wrb_128k((unsigned char)dos_aus);
					}
					//~ regelung_ini();
				}
			break;
			case MSR_ELY:
				if(vek[msr_t].ui == no_msr_txt)
					show_msr = 0;
				else {	
					show_msr = ~show_msr;
					menu_ini();
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
	} else if(KEY == ESC) {
		#define DELAY_RES 300
		++res_del;
		if(res_del > DELAY_RES) {
			xdata unsigned char i;	
			if(INIT_DELAY) {
				INIT_DELAY = 0;
				on_delay   = 0;
				res_del    = 0;
			}
			I_ALR       = 0;
			I_ALRH      = 0;
			I_ALRL      = 0;
			T_ALR       = 0;
			T_WRNH      = 0;
			T_WRNL      = 0;
			FLOW_WRN    = 0;
			FLOW_ALRH   = 0; 
			FLOW_ALRL   = 0; 
			FLOW_ALR    = 0; 
			U_ALRH      = 0; 
			U_ALRL      = 0; 
			NIVEAU_ALR  = 0;
			SOLE_ALR    = 0;
			COLLECT_ALR = 0;
			ANL_ALR     = 0;
			OVFL_ALR    = 0;
			TANK_NOT    = 0;
			swimm_cnt   = 0;
			vers_anlf   = 0;
			dly_H2      = 0;
			cnt_ini();
			for(i = 0; i < 2; ++i) {
				if (rl[i].dos_EEPR == SAVE_FLAG) {
					rl[i].dos_EEPR = 0xff;
					adr_128k = status_ix + i;
					ee_wrb_128k(0xff);
				} 
			}
		}
	} else
		res_del = 0;//, RLAN = 0;
	key_pr = KEY;
}

//~ int cal_ph_lim(unsigned char ind) {
	//~ xdata int erg;
	//~ erg = vek[ph_innen].i - vek[ind].i;
	//~ erg /= 100;
	//~ erg *= MV_PRO_PH;
	//~ return erg;
//~ }

bit txt_var_fn(unsigned int v) {
	if((vr.nr[v] >= btr_art_t && vr.nr[v] <= set_nt) || vr.nr[v] == komm_t)
		return 1;
	else 
		return 0;	
}


// MENU_MODE: 1 - Menuanfang; 2 - Untermenu oder Wert verstellen; 5 - Menu nach oben; 6 - Menu nach unten
//			 10 - Zurück  aus Untermenu, danach = 1; 11 - blinken , 12 - txt_cnt = 0;

void menu_rt(void) {
	static xdata unsigned char del_cnt;
	static xdata unsigned char km;
	unsigned char buf_d[20];
	//~ unsigned char xdata * xdata buf_d;
	//~ #define buf_d rram.tmp_buf[5]
	xdata unsigned char tmp, 
						gr;
		memset(&rram,0,sizeof(rram));
		//~ memset(buf_d, 0, LEN_60);
		switch(MENU_MODE) {
			case 1: // Menuanfang
				txt_cnt_max = menu_fuellen();
			break;
			case 2: // Untermenu oder Wert verstellen
				if(vr.pos[menu_stack.pos[0]] <= strlen(txt_dflt[menu_stack.pos[0]])) { // nur wenn Variable existiert 
					if(vr.nr[menu_stack.pos[0]] >= DIR_VAR) {// Untermenu
						xdata unsigned int nr;
						nr = vr.nr[menu_stack.pos[0]];
						gr = STACK_LAENGE - 1;
						for(; gr > 0; gr-- ) {
							menu_stack.txt_nr[gr]  = menu_stack.txt_nr [gr - 1];
							menu_stack.pos[gr]     = menu_stack.pos[gr - 1];
							menu_stack.menu_nr[gr] = menu_stack.menu_nr[gr - 1];
						}
						menu_stack.menu_nr[0] = (nr % DIR_VAR) + 1;
						menu_stack.txt_nr[0]  = 1;
						menu_stack.pos[0]     = 1;
						txt_cnt_max = menu_fuellen();
						++menu_stack.size;
						MENU_MODE = 1;
					}
					else if (vr.nr[menu_stack.pos[0]] <= code0_x || vr.nr[menu_stack.pos[0]] == ph_dpd2_in ||
							vr.nr[menu_stack.pos[0]] == cl_dpd_in || vr.nr[menu_stack.pos[0]] == t0_korr ||
							vr.nr[menu_stack.pos[0]] == komm_t ) {
						// Wert verstellen
						if((vek[code0_x].ui !=  11) && (vek[code0_x].ui != 87) &&
						   (vek[code0_x].ui != 178) && (vek[code0_x].ui != 221) &&
						    vr.nr[menu_stack.pos[0]] != code0_x) {
							MENU_MODE = 1;
						}
						else {	
							// blinken
							var_wert = vek[vr.nr[menu_stack.pos[0]]].ui;
							km = komma(vr.nr[menu_stack.pos[0]]);
							MENU_MODE = 11;
							if(vr.nr[menu_stack.pos[0]] == ph_dpd2_in) {
								if(vek[ph_dpd2_in].ui == 700 || vek[ph_dpd2_in].ui == 400)
									_nop_();
								else
									MENU_MODE = 1;
							} else if(vr.nr[menu_stack.pos[0]] == wreset_en)
								var_wert = nein_txt; 
							
						}
					}
					else
						MENU_MODE = 1;
				}		
			break;	
			case 5: // Menu nach oben
				cursor_nach_oben();
				MENU_MODE = 1;
			break;	
			case 6: // Menu nach unten
				cursor_nach_unten();
				MENU_MODE = 1;
			break;	
			case 10: // Zurück  aus Untermenu
				gr = STACK_LAENGE - 1;
				--menu_stack.size;
				for(tmp = 0; tmp < gr; tmp++ ) {
					menu_stack.txt_nr [tmp] = menu_stack.txt_nr[tmp + 1];
					menu_stack.pos[tmp]     = menu_stack.pos[tmp + 1];
					menu_stack.menu_nr[tmp] = menu_stack.menu_nr[tmp + 1];
				}
				MENU_MODE = 1;
			break;
			case 11:
				if(vr.nr[menu_stack.pos[0]] != ph_dpd2_in) {
					if(plus){
						if(!(++del_cnt % 3)) {
							++var_wert;
						}
						if(++speed_cnt > 60)
						{	
							++speed;
							speed_cnt = 0;
						}
						var_wert += speed;
						Wert_grenze(vr.nr[menu_stack.pos[0]]);
						blinken = 0;
						plus  = 0;
					}
					else if(minus) {
						if(!(++del_cnt % 3)) {
							--var_wert;
						}
						if(++speed_cnt > 60)
						{	
							--speed;
							speed_cnt = 0;
						}
						var_wert += speed;
						Wert_grenze(vr.nr[menu_stack.pos[0]]);
						minus = 0;
						blinken = 0;
					}
				}
				if(++blinken % 16 < 8) {	
					//~ memset(buf_d, 0, LEN_60);
					memset(buf_d,0,20);
					txt_dflt[menu_stack.pos[0]][vr.pos[menu_stack.pos[0]]] = 0;
					if(txt_var_fn(menu_stack.pos[0])) {
						if(vr.nr[menu_stack.pos[0]] == sprache_t)
							txt_anf(var_wert + sprachen_txt);
						else
							txt_anf(var_wert);
						read_text(buf_d);
					} else {
						if(km==3)
							sprintf(buf_d,"%6.3f", (float)var_wert/1000.0);
						//~ else if(km==4)
							//~ sprintf(buf_d,"%6.3f", (float)var_wert/1000.0);
						else if(km==2)
							sprintf(buf_d,"%5.2f", (float)var_wert/100.0);
						else if(km==1)
							sprintf(buf_d,"%5.1f", (float)var_wert/10.0);
						else	
							sprintf(buf_d,"%5d", var_wert);
					}
					add_var(menu_stack.pos[0], buf_d); 
				}
				else {
					txt_dflt[menu_stack.pos[0]][vr.pos[menu_stack.pos[0]]] = 0;
				}
			break;				
			default:
				if(show_msr) { // MSR
					if(0) { // mwm | no_verb) {	
						//~ sprintf(txt_dflt[u_zahl],"%s%s", basic[u_zahl], striche); // % Regler
						//~ sprintf(txt_dflt[t_zahl],"%s%s", basic[t_zahl], striche); // % Regler
						//~ sprintf(txt_dflt[1],"%s%s", basic[1], striche); // % Regler
						//~ sprintf(txt_dflt[8],"%s%s", basic[8], striche); // % Regler
						//~ sprintf(txt_dflt[10],"%s%bu", basic[10], 0); // % Regler
						//~ sprintf(txt_dflt[10],"%s", basic[10]); // % Regler
						//~ strcat(txt_dflt[10],"%");
					} else {
						bit add_prc(unsigned char line, unsigned char med);
						if(cl) {
							sprintf(txt_dflt[1],"%s%s", basic_msr[1], "CL2");
							add_prc(1,cl_rl);
							if(vek[mgl_t].ui == mgl_10_txt)
								sprintf(txt_dflt[2],"%s%5.2f", basic_msr[2], vek[cl_val_ix].ui / 100.0);
							else	
								sprintf(txt_dflt[2],"%s%4.3f", basic_msr[12], vek[cl_val_ix].ui / 1000.0);
						}	 
						if(ph) {
							sprintf(txt_dflt[3],"%s%s", basic_msr[3], "pH");
							if(!add_prc(3,phdown_rl))
								add_prc(3,phup_rl);
							sprintf(txt_dflt[4],"%s%5.2f", basic_msr[4], vek[ph_ix].ui / 100.0);
						}	 
						if(rx) {
							sprintf(txt_dflt[ 8],"%s%s", basic_msr[8], "Redox,mV");
							add_prc(8,rx_rl);
							sprintf(txt_dflt[10],"%s%5d", basic_msr[10], vek[rx_ix].i);
							//~ sprintf(txt_dflt[10],"%s%bu", basic_msr[10], btr_art_b0 - aus_txt);
						}
						{
							unsigned char t_komp_art;
							t_komp_art = vek[t_comp_t].ui;
							if(vek[t1_ix].i > MAX_T)
								t_komp_art = man_txt;
							//~ if(vek[mw_art_t].ui == flow_txt && vek[t_comp_t].ui == auto_txt)
							if(vek[mw_art_t].ui == flow_txt && t_komp_art == auto_txt)
								sprintf(txt_dflt[11],"%s%3.1f C  %4d L/h", basic_msr[11], (float)vek[t1_ix].i / 10.0, vek[fl_aktl_mw_ix].i);
							//~ else if(vek[mw_art_t].ui == flow_txt && vek[t_comp_t].ui == man_txt)
							else if(vek[mw_art_t].ui == flow_txt && t_komp_art == man_txt)
								sprintf(txt_dflt[11],"%s%4d L/h", basic_msr[11], vek[fl_aktl_mw_ix].i);
							//~ else if(vek[mw_art_t].ui == swimm_txt && vek[t_comp_t].ui == auto_txt)
							else if(vek[mw_art_t].ui == swimm_txt && t_komp_art == auto_txt)
								sprintf(txt_dflt[11],"%s%3.1f C", basic_msr[11], (float)vek[t1_ix].i / 10.0);
							else 
								;//sprintf(txt_dflt[11],"%s", basic_msr[11]);
								
						}
					}
					goto FLOW_MARKER;
				} else { // Elektrolyse
					sprintf(txt_dflt[u_zahl],"%s%4.1f", basic[u_zahl],vek[el_V_mv_ix].i / 1e3); 
					sprintf(txt_dflt[t_zahl],"%s%4.1f", basic[t_zahl],(float)(vek[t0_ix].i / 10.0)); 
					memset(buf_d, 0, 20);
					line_v(220, 0, "50"); 
					{
						unsigned int lev ;
						xdata float fl;
						tanksM();
						tank_lev(223, 2, min_max[plm]);
						lev = prc_fn(vek[el_A_mv_ix].ui, werte_el[i_soll][vek[anl_typ_t].b[1]-a30_txt]);
						strom_lev(155, 15, lev);
						if(vek[anl_typ_t].ui == a300_txt) {
							fl = vek[el_A_mv_ix].ui * 100.0 / 3000.0; 
						} else {
							fl = vek[el_A_mv_ix].ui * 50.0 / 3000.0; 
						}
	//~ #define STROM_DURCHFLUSS
	#ifdef STROM_DURCHFLUSS
		//~ sprintf(txt_dflt[i_prc],"%s%4.1f A, %3u", basic[i_prc], fl, lev);
		sprintf(txt_dflt[i_prc],"%sSek: %3u %3u", basic[i_prc], v_start_cnt, lev);
	#else
						sprintf(txt_dflt[i_prc],"%s%3u", basic[i_prc], lev);
	#endif
						strcat (txt_dflt[i_prc],"%");
						FLOW_MARKER:
						if(vek[flow0_ix].ui > 0) {
							if(vek[anl_typ_t].ui == a200_txt || vek[anl_typ_t].ui == a300_txt)
								fl = FL2 / vek[flow0_ix].ui; 
							else
								fl = FL1 / vek[flow0_ix].ui;
						} else fl = 0.0;		 
						lev = prc_fn(vek[flow0_soll_ix].ui, vek[flow0_ix].ui);
						//~ strom_lev(155, 39, lev);
						prc_flow = lev;
						if(!show_msr) {
							strom_lev(155, 39, lev);
						//~ if(0) {//prc_flow > 95) {
							//~ if(start_vgang == 0) {
								//~ start_vgang = 1;
								//~ sec_cnt     = 0; // in regelung_ely.abgelaufen()
								//~ FLOW_WRN    = 0;
							//~ }
						//~ }
	#ifdef STROM_DURCHFLUSS
		sprintf(txt_dflt[Q_prc],"%s%4.1f L/h, %3u", basic[Q_prc],fl,lev);
	#else
						if(vek[code0_x].ui == 178) {
							sprintf(txt_dflt[Q_prc],"%s%3u%% %u %u", basic[Q_prc], lev, vek[prv_ch0_ix].ui, vek[prv_ch1_ix].ui);
						} else 
						sprintf(txt_dflt[Q_prc],"%s%3u%%", basic[Q_prc],lev);
	#endif
						//~ strcat (txt_dflt[Q_prc],"%");
						}
					}
				}	
				{
					xdata unsigned char tmp;
					tmp = strlen(basic[sts_line]);
					txt_dflt[sts_line][tmp] = 0;
							
					if(sts_ctr && sts_ctr >= status_zeile.ctr_alr + status_zeile.ctr_warn)
						--sts_ctr;
					if(sts_ctr < status_zeile.ctr_alr)
						tmp = status_zeile.alarm[sts_ctr];
					else
						tmp = status_zeile.warn[sts_ctr - status_zeile.ctr_alr];
					
					//~ memset(buf_d,0,sizeof(buf_d));
					//~ memset(buf_d, 0, LEN_60);
					
					if(tmp) {
						//~ char buf_dd[20];
						sprintf(buf_d,"%bu+%bu ", status_zeile.ctr_alr, status_zeile.ctr_warn);
						txt_anf(tmp);
						sprintf(txt_dflt[sts_line],"%s%s", basic[sts_line], buf_d);
						read_text(txt_dflt[sts_line]);
						if(tmp == on_dly_txt && vek[msr_t].ui != no_msr_txt) {
							//~ sprintf(txt_dflt[sts_line] + strlen(txt_dflt[sts_line])," %u s", on_delay);
							sprintf(txt_dflt[sts_line] + strlen(txt_dflt[sts_line]), fmt_sek, on_delay);
						}
						if(tmp == alr_flL_txt && !FLOW_ALR) {
							//~ sprintf(txt_dflt[sts_line] + strlen(txt_dflt[sts_line])," %u s", ZEHN_MIN - sec_cnt2);
							sprintf(txt_dflt[sts_line] + strlen(txt_dflt[sts_line]),fmt_sek, DREI_MIN - sec_cnt2);
						}
						if(tmp == vent_vor_txt) {
							sprintf(txt_dflt[sts_line] + strlen(txt_dflt[sts_line]), fmt_sek, luft_cnt_vor);
						} else if(tmp == vent_nach_txt) {
							sprintf(txt_dflt[sts_line] + strlen(txt_dflt[sts_line]), fmt_sek, luft_cnt_nach);
						}
					}
				}
			break;	
		}
		//~ memset(&rram, 0, sizeof(rram));
		for(gr = 0; gr < txt_cnt_max; ++gr) {	
			display_wr(txt_dflt[gr]);
		}
	//~ free(buf_d);	
}

bit add_prc(unsigned char line, unsigned char med) {
	//~ xdata unsigned char len;
	#define len_prc rram.tmp_buf[15][1]
	bit b;
	b = 0;
	len_prc = strlen(txt_dflt[line]);
	if(rl[0].ind == med) {
		sprintf(txt_dflt[line] + len_prc, ",%bu", rl[0].prc);
		strcat(txt_dflt[line],"%");
		b = 1;
	} else if(rl[1].ind == med) {
		sprintf(txt_dflt[line] + len_prc, ",%bu", rl[1].prc);
		strcat(txt_dflt[line], "%");
		b = 1;
	}
	return b;
}


unsigned int prc_fn(unsigned int ist, unsigned int soll) {
	if(!soll)
	 return 0;
	else 
		return 100.0 * (float)ist / soll;
}

//~ int rl_grenze(unsigned int r_strg) {
	//~ switch(vek[msr_t].ui) {
		//~ case no_msr_txt:
			//~ if(vek[r_strg].ui == ph_down_txt)
				//~ vek[r_strg].ui = alarm_txt;
		//~ break;		
	    //~ case cl_txt:
			//~ if(vek[r_strg].ui == ph_down_txt)
				//~ vek[r_strg].ui = cl_txt;
			//~ if(vek[r_strg].ui == ph_up_txt)
				//~ vek[r_strg].ui = alarm_txt;
		//~ case ph_txt: case ph_rx_txt:
			//~ if(vek[r_strg].ui == cl_txt)
				//~ vek[r_strg].ui = ph_up_txt;
		//~ default: 
		//~ break;		
	//~ }		
//~ }

void Wert_grenze(unsigned int pos) {
	xdata FLOAT f;
	f.b[0] = 0;
	f.b[1] = pos;	
	if(pos == sprache_t) { // Sprache
		if(var_wert >= anz_spr) 
			var_wert = anz_spr - 1;
		else if(var_wert < 0) 
			var_wert = 0;
	} else if(pos <= code0_x || pos == cl_dpd_in || pos == komm_t){
		if(var_wert > tab1[pos].max)
			var_wert = tab1[pos].max;
		else if(var_wert < tab1[pos].min) 
			var_wert = tab1[pos].min;
	} else if (pos == t0_korr) {
		if(var_wert > tab1[t_korr].max) 
			var_wert = tab1[t_korr].max;	
		if(var_wert < tab1[t_korr].min) 
			var_wert = tab1[t_korr].min;	
	}
	switch(pos)	{
		case cl_alr_ob: 
		case ph_alr_ob: 
		case rx_soll: 
			if(var_wert < vek[pos + 1].i)
				var_wert = vek[pos + 1].i;
		break;	
		case cl_wr_ob:   case cl_soll: case cl_wr_un:
		case ph_wr_ob:   case ph_soll: case ph_wr_un:
		case rx_wr_un:
			if(var_wert < vek[pos + 1].i)
				var_wert = vek[pos + 1].i;
			else if(var_wert > vek[pos - 1].i)
				var_wert = vek[pos - 1].i;
		break;	
		case cl_alr_un: 
		case ph_alr_un: 
		case rx_alr_un: 
			if(var_wert > vek[pos - 1].i)
				var_wert = vek[pos - 1].i;
		break;
		case r3_strg_t:
		case r4_strg_t:
			switch(vek[msr_t].ui) {
				case no_msr_txt:
					if(var_wert == ph_down_txt)
						var_wert = alarm_txt;
				break;		
				case cl_txt:
					if(var_wert == ph_down_txt)
						var_wert = cl_txt;
					if(var_wert == redox_txt)
						var_wert = alarm_txt;
				break;		
				case ph_txt:
					if(var_wert == redox_txt)
						var_wert = ph_up_txt;
				break;		
				case ph_rx_txt:
					if(var_wert == cl_txt)
						var_wert = redox_txt;
				break;		
				case cl_ph_txt:
				case cl_ph_rx_txt:
					if(var_wert == redox_txt)
						(plus) ? (var_wert = cl_txt) : (var_wert = ph_up_txt);
				break;		
				default: 
				break;		
			}		
		break;	
		//~ case contr:
			//~ if(i > contr_dflt.max)      i = contr_dflt.max;
			//~ else if(i < contr_dflt.min) i = contr_dflt.min;
		//~ break;
		default: 
		break;	
	}
	//~ return i;
}
/*
void Wert_grenze(unsigned char pos) {
	xdata FLOAT f;
	
	f.b[0] = 0;
	f.b[1] = pos;
	if(pos == sprache_t) {
		if(var_wert >= anz_spr) 
			var_wert = anz_spr - 1;
		else if(var_wert < 0) 
			var_wert = 0;
	} else if(pos < alr_ob_des) {
		if     (var_wert > tab1[pos].max) var_wert = tab1[pos].max;
		else if(var_wert < tab1[pos].min) var_wert = tab1[pos].min;
	} else if(pos < code0_x) {
		f.b[0] = des_offset();
		f.b[1] = pos + f.b[0];
		//~ if     (var_wert > tab1[pos+f.b[0]].max) var_wert = tab1[pos+f.b[0]].max;
		//~ else if(var_wert < tab1[pos+f.b[0]].min) var_wert = tab1[pos+f.b[0]].min;
		if     (var_wert > tab1[f.b[1]].max) var_wert = tab1[f.b[1]].max;
		else if(var_wert < tab1[f.b[1]].min) var_wert = tab1[f.b[1]].min;
	} else if(pos >= cl_ph_T && pos <= cl_ph_c_Int) {
		f.b[0] = pos - cl_ph_T;
		if     (var_wert > tab2[f.b[0]].max) var_wert = tab2[f.b[0]].max;
		else if(var_wert < tab2[f.b[0]].min) var_wert = tab2[f.b[0]].min;
	}
	switch(pos) {
		case alr_ob_des: case ph_alr_ob_int:
			if(var_wert <  vek[pos + 1].i)
				var_wert = vek[pos + 1].i;
			//~ if(var_wert <  vek[f.b[1]].i)
				//~ var_wert = vek[f.b[1] + 1].i;
		break;	
		case wr_ob_des: case soll_des: case wr_un_des:   
		case ph_wr_ob_int:  case ph_soll_int:  case ph_wr_un_int:
			//~ if(var_wert <  vek[f.b[1] + 1].i)
				//~ var_wert = vek[f.b[1] + 1].i;
			//~ else if(var_wert > vek[f.b[1] - 1].i)
				//~ var_wert = vek[f.b[1] - 1].i;
			if(var_wert <  vek[pos + 1].i)
				var_wert = vek[pos + 1].i;
			else if(var_wert > vek[pos - 1].i)
				var_wert = vek[pos - 1].i;
		break;	
		case alr_un_des:  case ph_alr_un_int:
			//~ if(var_wert >  vek[f.b[1] - 1].i)
				//~ var_wert = vek[f.b[1] - 1].i;
			if(var_wert >  vek[pos - 1].i)
				var_wert = vek[pos - 1].i;
		break;	
		case out1_4ma_int:
		case out1_20ma_int:
			f = ma_ini(out1_t);
			ma_lim(f);
		break;
		case out2_4ma_int:
		case out2_20ma_int:
			f = ma_ini(out2_t);
			ma_lim(f);
		break;
		case out3_4ma_int:
		case out3_20ma_int:
			f = ma_ini(out3_t);
			ma_lim(f);
		break;
		case out4_4ma_int:
		case out4_20ma_int:
			f = ma_ini(out4_t);
			ma_lim(f);
		break;
		case mon1_llong: case mon2_llong: case die1_llong: case die2_llong:
		case mit1_llong: case mit2_llong: case don1_llong: case don2_llong:
		case fre1_llong: case fre2_llong: case sam1_llong: case sam2_llong:
		case son1_llong: case son2_llong:
			fzt_lim();
		break;
		case wreset_en:
			if(var_wert > nein_txt)
				var_wert = nein_txt;
			if(var_wert < ja_txt)
				var_wert = ja_txt;	
			break;
		//~ case r1_funk_t:
			//~ if(vek[filter_strg_t].ui == nein_txt) {
				//~ if(var_wert == heizen_txt)
					//~ var_wert = des_txt;
			//~ } else 
				//~ vek[rl_1_art_t].ui = an_aus_txt;
		//~ break;
		//~ case r1_funk: case r2_funk: case r3_funk:
			//~ if(vek[ph_mess].i == nein_txt)
				//~ if(i < flock_txt)
					//~ i = flock_txt;
		//~ break;
	}
}
*/


void Wert_speichern(void) {
	adr_128k = vr.nr[menu_stack.pos[0]] * 2;
	ee_wrb_128k(vek[vr.nr[menu_stack.pos[0]]].b[0]);
	++adr_128k;
	ee_wrb_128k(vek[vr.nr[menu_stack.pos[0]]].b[1]);
}

void speichern_int(unsigned int u) {
	adr_128k = u * 2;
	ee_wrb_128k(vek[u].b[0]);
	++adr_128k;
	ee_wrb_128k(vek[u].b[1]);
}

unsigned char tankMarker(unsigned char l, unsigned char overflow) {
	//~ xdata float f;
	#define tm_f rram.f_buf[7][0]
	tm_f  = 46 * (float)vek[l].ui ;
	tm_f /= (float)vek[overflow].ui;
	if(tm_f > 46.0) 
		tm_f = 46.0;
	return tm_f;
}

void tanksM(void) {
	min_max[pmin] = tankMarker(prodlev_min, prodlev_ovfl);	
	min_max[pmax] = tankMarker(prodlev_max, prodlev_ovfl);	
	min_max[plm]  = tankMarker(lev_prod_ix,    prodlev_ovfl);	
}

void tank_lev(unsigned char col, unsigned char row, unsigned char lev) {
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
	line_h(col, 46 + row - min_max[pmin],"16");
	line_h(col, 46 + row - min_max[pmax],"16");
}

void strom_lev(unsigned char col, unsigned char row, unsigned int prc) {
	xdata unsigned char x, b, str[3];
	line_v(col,    row,    "6");
	line_v(col+60, row,    "6");
	line_v(col+40, row-2,  "8");
	line_h(col,    row,    "60");
	line_h(col,    row+ 6, "61");
	if(prc > 150) prc = 150;
	b = prc * 0.4;
	for(x = 1; x < 6; ++x) {
		str[0] = 0x30 + b/10;
		str[1] = 0x30 + b%10;
		str[2] = 0;
		line_h(col, row + x, str);
	}
}

void show_update(void) {
	static char chr;
	switch (chr) {
		case '-' : chr = '\\'; break;	
		case '\\': chr = '|';  break;	
		case '|' : chr = '/';  break;
		default  : chr = '-';  break;	
	}
	memset(&status_zeile,0, sizeof(status_zeile));
	memset(&rram,0,sizeof(rram));
	sprintf(txt_dflt[0],"%s ",basic[st_zeile]);
	//~ txt_anf(good_txt);
	//~ read_text2(txt_dflt[0]);
	sprintf(txt_dflt[0] + strlen(txt_dflt[0]), " UPDATE: %c %lu", chr, adr_128k);
	display_wr(txt_dflt[0]);
}

