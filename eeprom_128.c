#include <math.h>
#include <intrins.h>
#include <stdlib.h>
#include <string.h>
#include <absacc.h>
#include "local_var.h"
#include "eeprom_128drv.h"
#include "main.h"
#include "regelung.h"
#include "display.h"
#include "Bus_LCD.h"
#include "menu_lcd.h"



extern code unsigned char id_strg[];
extern code unsigned char id[];
extern code WERTE tab1[];
extern code WERTE tab2[];
xdata unsigned long adr_128k;
xdata unsigned char *ptr_128k;
xdata unsigned int len_128k;
xdata UINT vek[ssid_en];
xdata unsigned char asr2, cl_sth;


void speichern_int(unsigned int u);

void Set_default(void) {
	memset(&rram, 0xff, sizeof(rram));
	adr_128k = 0;
	len_128k = code0_x * 2;	//
	ptr_128k = rram.tmp_buf;
	WDT_RT();
	ee_wrseq_128k(0);
	/*
	//adr_128k = U7;
	memset(&rram,0x7f,sizeof(rram));
	adr_128k = (int)ph_mv_eich1 * 2;
	len_128k = 10;
	ptr_128k = &rram.buf_tmp;
	WDT_RT();
	ee_wrseq_128k();
	adr_128k = (int)rx_corr * 2;
	ee_wrb_128k(0);
	++adr_128k;
	ee_wrb_128k(0);
	*/ 
	RSTSRC |= 0x10; // Softwarereset
}

#if 0
void dev_num(void) {
	//~ xdata FLOAT f;
	/*
	unsigned long xdata * xdata fp;
	fp = malloc (4); 
	
	#define L2 50176 // 224*224	
	//~ f.ul  = (vek[lng_adr2].b[1] - 0x20) * L2;
	//~ f.ul += (vek[lng_adr1].b[1] - 0x20) * 224;
	//~ f.ul += (vek[lng_adr0].b[1] - 0x20);
	*fp  = (vek[lng_adr2].b[1] - 0x20) * L2;
	*fp += (vek[lng_adr1].b[1] - 0x20) * 224;
	*fp += (vek[lng_adr0].b[1] - 0x20);
	vek[dev_nr_ix].ui = (*fp) & 0x00ffff;
	free(fp);
	*/
}
#endif

void des_alarm_fn(void) {
	des_alr_delay  = vek[cl_alr_dly].i;
	des_alr_delay *= 60;
}

#if 1
void SteilheitPH(void) {
	unsigned long xdata * xdata fp;
	fp = malloc (4); 
	*fp = (vek[U4_in].i - vek[U7_in].i) / 3.0;
	vek[ph_sth_in].i = *fp;
	vek[ph_null_in].i = vek[U7_in].i;
	if(*fp - vek[ph_sth_in].i >= 0.5) 
		++vek[ph_sth_in].i;
	if(vek[ph_sth_in].i < tab1[ph_sth_in].min) {	
		PH_STH_ERR = 1;
	} else
		PH_STH_ERR = 0;
	//~ f = (float) vek[ph_null_in].i / (float) vek[ph_sth_in].i;
	if(vek[ph_null_in].i < tab1[ph_null_in].min || vek[ph_null_in].i > tab1[ph_null_in].max) {	
		PH_NULL_ERR = 1;
	} else
		PH_NULL_ERR = 0;
	free(fp);	
}
#endif

#if 0
void SteilheitPH(void) {
	#define LONG_0 rram.l_buf[6][0]

	LONG_0 = (vek[U4_in].i - vek[U7_in].i) / 3.0;
	vek[ph_sth_in].i = LONG_0;
	vek[ph_null_in].i = vek[U7_in].i;
	if(LONG_0 - vek[ph_sth_in].i >= 0.5) 
		++vek[ph_sth_in].i;
	if(vek[ph_sth_in].i < tab1[ph_sth_in].min) {	
		PH_STH_ERR = 1;
	} else
		PH_STH_ERR = 0;
	if(vek[ph_null_in].i < tab1[ph_null_in].min || vek[ph_null_in].i > tab1[ph_null_in].max) {	
		PH_NULL_ERR = 1;
	} else
		PH_NULL_ERR = 0;
}
#endif
//~ unsigned int date_conv(unsigned int dat) {
	//~ return ((dat%100)*100 + dat/100);
//~ }

bit test_var(unsigned char var_num) {
	//~ if(var_num==mdate_in) {
		//~ if(date_conv(vek[var_num].i) >= date_conv(tab1[var_num].min) && date_conv(vek[var_num].i) <= date_conv(tab1[var_num].max))	
			//~ return 0;
		//~ else return 1;
	//~ } else {
		if(vek[var_num].i >= tab1[var_num].min && vek[var_num].i <= tab1[var_num].max)	
			return 0;
		else return 1;
	//~ }	
}

void modbus_GW(void) {
	if(vek[modbus_in].b[1] > 0) 
		P4 |= 0x04; // GW_ON   = 1;
	else
		P4 &= ~0x04;
}

void ee_ini(void) {	
	//~ xdata unsigned char i, tmp0[5]; 
	//~ xdata unsigned int ix;

	#define ip (*p)
	unsigned char xdata * xdata p;
	
	p = malloc (1);
	memset(vek, 0, sizeof(vek));	
	for(adr_128k = 0, ip = 0; ip <= komm_t; ++ip) {
		vek[ip].b[0] = ee_rdb_128k();
		vek[ip].b[1] = ee_rdb_128k();
		//~ if(vek[ip].i < tab1[ip].min || vek[ip].i > tab1[ip].max)
			//~ vek[ip].i = tab1[ip].dflt;
		//~ if(i_0 == v_monitus_t)
			//~ i_0 = i_0;
		if(test_var(ip))
			vek[ip].i = tab1[ip].dflt;
			
		WDT_RT();
	}
	//~ free(p);
	//~ n = vek[contr].b[1];
	adr_128k   = (int)status_ix * 2;
	dos_aus    = (bit)ee_rdb_128k();
	ON_OFF     = 0;
	ON_OFF_LED = ON_OFF;
	SteilheitPH();
 	WDT_RT();

	adr_128k   = (int)modbus_in * 2;
	vek[modbus_in].b[1] = ee_rdb_128k();
	modbus_GW();
	
		
	//~ DES_AUS = (bit)ee_rdb_128k();
	//~ DES_AUS_LED = ~DES_AUS;
	//~ PH_AUS = (bit)ee_rdb_128k();
	//~ PH_AUS_LED = ~PH_AUS;
	//~ DES_AUS     = 1;
	//~ DES_AUS_LED = 1;
	//~ PH_AUS      = 1;
	//~ PH_AUS_LED  = 1;

	//~ on_delay = vek[on_dly].ui * 60;
	des_alarm_fn();
	
	vek[code0_x].ui = 0;
	vek[t0_ix].ui = 0;
	//~ for(adr_128k = mdate*2, n = sh_adr; n <= lng_adr2; ++n) {
	adr_128k = mdate_in*2;
	vek[mdate_in].b[0] = ee_rdb_128k();
	vek[mdate_in].b[1] = ee_rdb_128k();
	//~ if(vek[mdate_in].i < tab1[mdate_in].min || vek[mdate_in].i > tab1[mdate_in].max)
	if(test_var(mdate_in))
		vek[mdate_in].i = tab1[mdate_in].dflt;

	vek[cl_konz].i  = tab1[cl_konz].dflt;
	//~ vek[sole_dos].i = tab1[sole_dos].dflt;

	//~ if(vek[sh_adr].ui < 0x21 || vek[sh_adr].ui > 0xff)
	if(test_var(sh_adr))
		vek[sh_adr].ui = 94;
	
	//~ if(vek[lng_adr2].b[1] < 0x20) vek[lng_adr2].b[1] = 0xff;
	//~ if(vek[lng_adr1].b[1] < 0x20) vek[lng_adr1].b[1] = 0xff;
	//~ if(vek[lng_adr0].b[1] < 0x20) vek[lng_adr0].b[1] = 0xff;
	//~ dev_num();
	adr_128k = dev_nr_ix * 2;
	vek[dev_nr_ix].b[0] = ee_rdb_128k();
	vek[dev_nr_ix].b[1] = ee_rdb_128k();
	if(vek[dev_nr_ix].i <= 0)
		vek[dev_nr_ix].i = 0x7fff;
		
	strncpy(rram.tmp_buf, id + 21, 4);
	vek[softver_ix].i = atoi(rram.tmp_buf);
	asr2 = vek[asr_std].b[1] + 12;
	if(asr2 > 23) asr2 -= 24;
	if(vek[mgl_t].ui == mgl_1_txt)
		cl_sth = cl_1_sth_in;
	else 	
		cl_sth = cl_sth_in;
	adr_128k = cl_sth * 2;
	vek[cl_sth].b[0] = ee_rdb_128k();
	vek[cl_sth].b[1] = ee_rdb_128k();
	POT_STH_ERR = test_var(cl_sth);
	adr_128k = cl_ph_saved * 2;
	if(ee_rdb_128k() == SAVE_FLAG)
		cl_ph_comp = 1;
	else
		cl_ph_comp = 0;

	adr_128k = t0_korr * 2;
	vek[t0_korr].b[0] = ee_rdb_128k();
	vek[t0_korr].b[1] = ee_rdb_128k();
	if(vek[t0_korr].i < tab1[t_korr].min || vek[t0_korr].i > tab1[t_korr].max)
		vek[t0_korr].i = tab1[t_korr].dflt;	
	if(vek[lev_art_t].b[1] == swimm_txt)
		vek[t_comp_t].b[1] = man_txt;	
	
	//~ vek[sprache_t].ui -= sprachen_txt; // Sprache speichern.
	//~ ix = vek[sprache_t].ui;
	//~ ix = 0;
	vek[mdate_in].ui = 418;
#ifdef TEST
	vek[code0_x].ui = 178;
#endif
	adr_128k = 0;
	free(p);
}
