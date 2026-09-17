#include <math.h>
#include "ConstDef.h"
#include "eeprom128.h"
#include "eeprom_128drv.h"
#include "main.h"
#include "menu_lcd.h"

//~ #define PH_DELAY 10

//~ #define MAXZEIT 180 // 3 Min.

extern code WERTE tab1[];

static bit des_hs;
static xdata char ph_dir;
				  //~ prc_save;

static unsigned int t_gradC;
bit cl_ph_comp;
xdata int ph_mv_corr;
static xdata float ki_cl,
				   isum_cl,
				   ki_ph,
				   isum_ph, 
				   f_tmp; 
void cl_rechnen(void);				
void t_compensation(void);
//~ unsigned char rel_assign(unsigned char n);
				
// Aufruf in menu_ini()
//~ void i_anteil(void) {	
	//~ if(vek[cl_i].ui)
		//~ ki_cl = 100.0 / (vek[cl_i].ui * vek[cl_p].ui);
	//~ else
		//~ ki_cl = 0.0;
//~ }

float ki(unsigned int i, unsigned int p) {
	if(vek[i].ui)
		return 100.0 / (vek[i].ui * vek[p].ui);
	else
		return 0.0;
}

void i_anteil(void) {	
	//~ ki_ph = ki(ph_i_int, ph_p_int);
	//~ ki_des = ki(i_des,p_des);
	ki_ph = ki(ph_zi, ph_p);
	ki_cl = ki(cl_i,cl_p);
}

void t_compensation(void) {
	if(vek[t_comp_t].ui == auto_txt && vek[t1_ix].i < MAX_T)
		t_gradC = vek[t1_ix].ui / 10;
	else
		t_gradC = vek[t_man_comp].ui;
}

void compensation_1_2(void) {
	xdata int flow_lim;
	
	f_tmp = vek[cl_mv_ix].i;
	//~ 1. Kompensation
	if(vek[fl_aktl_mw_ix].i > 0) {
		flow_lim = vek[fl_aktl_mw_ix].i;
		#define MAX_FLUSS 120 // l/h
		#define MIN_FLUSS 30 // l/h
		if(flow_lim > MAX_FLUSS) flow_lim = MAX_FLUSS;
		if(flow_lim < MIN_FLUSS) flow_lim = MIN_FLUSS;
		#define REF_FLOW 80 // l/h
		f_tmp  = flow_lim - REF_FLOW;
		f_tmp *= flw_cmp;
		f_tmp += 1;
		f_tmp  = vek[cl_mv_ix].i / f_tmp;
	} 
	//~ 2. Kompensation
	f_tmp *= 0.96872;
	f_tmp /= (0.018736 * t_gradC + 0.50032);
	//--------------------###### 
	//-- pH-Kompensation bei Chlormessung ----
	// y = 50% * ((2/3.14159)*atan(pH - 7.5) + 1)
	#define CL_PH_CONST 0.15 / 0.57
	#define PH_EXP -0.71
	if(cl_ph_comp) { 
		xdata float f;
		f = vek[ph_ix].i / 100.0;
		//~ f = pow(f, PH_EXP);
		//~ f = CL_PH_CONST / f;
		if(vek[ph_ix].i < 870) {
			f *= 0.162;
		} else {
			xdata float v;
			v  = -57.0 * f;
			v += 635.0;
			f  = 192.0 / v;
		}	
		f = f_tmp * f;
		f_tmp = f;
		//~ vek[mv_comp_ix].ui = f;
	}
	//-- Ende --------------------------------
	//--------------------###### 
	//~ vek[mv_comp_ix].i  = vek[cl_mv_ix].i;
	vek[cl_mv_ix].i = f_tmp;
	if(uin_set_flg) {
		vek[uin_saved_ix].i = vek[cl_mv_ix].i;
		uin_set_flg = 0;
	} 
}

void cl_rechnen(void) {
	xdata float f;
	if(asr_10min) {
		//~ vek[mv_comp_ix].i = vek[cl_mv_ix].i;
		return;
	}
	
	compensation_1_2();
	//~ f_tmp = vek[cl_mv_ix].i;
	if(vek[cl_mv_ix].i > 0) {	
		f = f_tmp;
		if(dpd_bit)	{
			xdata float cl_corr; 
			dpd_bit = 0;
			if(uin_save_flg) {
				f = vek[uin_saved_ix].i;
				uin_save_flg = 0;
			}
			cl_corr = f / (float)vek[cl_dpd_in].i;
			cl_corr *= 100.0;
			vek[cl_sth].i = cl_corr;
			speichern_int(cl_sth);
			POT_STH_ERR = test_var(cl_sth);
		}
		f /= (float)vek[cl_sth].i;
		if(vek[mgl_t].ui==mgl_10_txt) {
			if(f > 50.0) f = 50.0;
			vek[cl_val_ix].i = f * 100.0;
		} else {
			if(f > 5.0) f = 5.0;
			vek[cl_val_ix].i = f * 1000.0;
		}	
	}
	else
		vek[cl_val_ix].i = 0;
}
	
char cl_Regelung(void) {
	xdata float dif;
	xdata int d;
	d = vek[cl_val_ix].i - vek[cl_soll].i; 
	dif = d;
	isum_cl += dif;
	dif /= vek[cl_p].ui;
	dif *= 100;
	dif += isum_cl * ki_cl;
	if(dif > 0) isum_cl = 0;
	if(dif > 100.0) {
		dif = 100.0;
		isum_cl -= d;
	} else if(dif < -100.0) {
		dif = -100.0;
		isum_cl -= d;
	}
	if(dif >= 0)
		des_hs = 0;
	else {
		if(abs(d) > vek[cl_hys].i)
			des_hs = 1;
		if(!des_hs)
			dif = 0;
	}
	//~ return (char) dif;
	if (dif >= 0)
		return 0;
	return (char)fabs(dif);
}

void regelung_ini(void) {
	isum_cl = 0;
}

void pH_rechnen(void) {
	xdata float mv_corr;
	mv_corr = vek[ph_mv_ix].i ;//* 298;
	mv_corr /= (273 + t_gradC);
	mv_corr *= 298;
	ph_mv_corr = mv_corr;
	{	
		xdata float dif;
		dif = vek[U7_in].i - mv_corr;
		dif /= vek[ph_sth_in].i;
		dif = 7.0 + dif;
		if(dif - (int)dif >= 0.005)
			dif += 0.01;
		vek[ph_ix].i = dif * 100.0;
	}
	if(vek[ph_ix].i < 0)         vek[ph_ix].i = 0;
	else if(vek[ph_ix].i > 1400) vek[ph_ix].i = 1400;
	
	if(ph_mv_corr >= tab1[U7_in].min && ph_mv_corr <= tab1[U7_in].max) {
		vek[ph_dpd2_in].i = 700;
	}
	else if(ph_mv_corr >= tab1[U4_in].min && ph_mv_corr <= tab1[U4_in].max) {
		vek[ph_dpd2_in].i = 400;
	}
	else
		vek[ph_dpd2_in].i = 0;
}

char pH_Regelung(void) {
	xdata float dif;
	static xdata float f;
	xdata char s;
	
	//~ vek[ph_ix].i = 900;
	
	dif = vek[ph_ix].i - vek[ph_soll].i; // pH Sollwert 
	f = dif;
	if(dif >= 0) s =  1; 
	else         s = -1;
	
	dif = fabs(dif);
	if(vek[totband_ph].ui) {
		if(dif <= vek[totband_ph].ui) 
			ph_dir = 0;
	} else {	
		if(ph_dir + s == 0)
			ph_dir = 0;
	}
	
	// dif = fabs(dif);
	if(dif > vek[ph_hys].ui) // Hysterese
		ph_dir = s;
	
	if(ph_dir) {	
		dif /= vek[ph_p].ui;
		dif *= 100;
		dif *= s;
		//----------------------------
		if(ki_ph > 0.0) {	
			dif += isum_ph * ki_ph;
		}
		//----------------------------
	} else {	
		dif     = 0;
		isum_ph = 0;
		ph_dir  = 0;
	}
	
	isum_ph += f;
	if(dif > 100.0) {
		dif = 100.0;
		isum_ph -= f;
	} else if(dif < -100.0) {
		dif = -100.0;
		isum_ph -= f;
	}
	return (char)dif;
}

char rx_Regelung(void) {
	xdata float dif;
	dif = vek[rx_ix].i - vek[rx_soll].i; 
	if(dif >= 0) { // rx senken
		dif    = 0.0;
		des_hs = 0;
		return 0;
	} else  {// rx heben
		dif = fabs(dif);
		dif /= vek[rx_p].ui;
		dif *= 100;
		if(dif > 100.0) dif = 100.0;
		if(dif > vek[rx_hys].i)
			des_hs = 1;
		
		if(des_hs)
			return dif;
		else	
			return 0;
	}
}




