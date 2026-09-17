#include <stdlib.h>
#include <stdio.h>
#include <intrins.h>
#include <string.h>
#include "ConstDef.h"                 // SFR declarations
#include "eeprom128.h"
#include "eeprom_128drv.h"
#include "regelung.h"
#include "regelung_ely.h"
//~ #include "menu_lcd.h"
#include "main.h"

xdata MSR_RL rl[2];

unsigned int set_alr_dly(unsigned char in) {
	return vek[in].ui * 60; 
}

void dos_watch_ini(unsigned char ind, unsigned char * p) {
	xdata UINT ui;
	switch(ind) {
		case 3:
			//~ frl.vui[0].ui = vek[ph_dos_watch].ui;
			//~ frl.vui[1].ui = dos_phs_ovfl_txt;
			ui.ui = vek[ph_dos_watch].ui;
			*(p + 2) = dos_phs_ovfl_txt;
			*(p + 3) = ddos_ph_txt;
			ph_alr_delay = set_alr_dly(alr_dly_ph);
		break;	
		case 4:
			ui.ui = vek[ph_dos_watch].ui;
			*(p + 2) = dos_phh_ovfl_txt;
			*(p + 3) = ddos_ph_txt;
			ph_alr_delay = set_alr_dly(alr_dly_ph);
		break;	
		case 5:
			ui.ui = vek[rx_dos_watch].ui;
			*(p + 2) = dos_des_ovfl_txt;
			*(p + 3) = ddos_des_txt;
			des_alr_delay = set_alr_dly(alr_dly_rx); 
		break;	
		case 6:
			ui.ui = vek[cl_dos_watch].ui;
			*(p + 2) = dos_des_ovfl_txt;
			*(p + 3) = ddos_des_txt;
			des_alr_delay = set_alr_dly(cl_alr_dly);
		break;	
		default:
			ui.ui = 0;
			*(p + 2) = 0;
			*(p + 3) = 0;
			//~ des_alr_delay = 0;
			//~ ph_alr_delay  = 0;
		break;	
	}
	ui.ui *= 60;
	*(p+0) = ui.b[0];
	*(p+1) = ui.b[1];
}

void relais_ini_msr(void) {
	xdata char i;
	//~ xdata FLOAT frl;
	unsigned char xdata * xdata fp;
	fp = malloc(4); 

	for(i = 0; i < 2; ++i) {
		rl[i].ind = vek[r3_strg_t + 2*i].ui - frei_txt;
		dos_watch_ini(rl[i].ind, fp);
		rl[i].dos_zeit_max   = *(fp + 0);
		rl[i].dos_zeit_max <<= 8;
		rl[i].dos_zeit_max  |= *(fp + 1);
		rl[i].dos_ovfl_txt   = *(fp + 2);
		rl[i].ddos_txt       = *(fp + 3);
		rl[i].ddos           = 0;
		rl[i].dos_zeit       = 0;
		rl[i].prc            = 0;
		adr_128k = status_ix + i;
		rl[i].dos_EEPR = ee_rdb_128k();
		if(ventilator_rl()) {
			vek[r3_art_t + 2*i].ui = an_aus_txt;	
		}
	}
	free(fp);
}

void ddos_fn(unsigned char ind, unsigned char vek_ind, bit enbl) {
	xdata unsigned int ti;
	ti = vek[vek_ind].ui * 60 * (char)enbl;
	if(rl[0].ind == ind)
		rl[0].ddos = ti;
	if(rl[1].ind == ind)
		rl[1].ddos = ti;
}

char frei_fn(void) { return 0;}

char vent_fn(void) {
	if(luft_cnt_vor > 0) {
		--luft_cnt_vor;
		return 100;
	}	
	if(luft_cnt_nach > 0) {
		--luft_cnt_nach;
		add_warn(vent_nach_txt);
		return 100;
	}	
	if(btr_art_b0 == run_txt)
		return 100;
	else 
		return 0;	
}
char aimb_fn(void) {
	//~ #define AIMB_BIT 0x8000 // später festlegen 
	return 100 * (unsigned char)(btr_art_b0 == run_txt);
}

char alarm_fn(void) {
	return 100 * (unsigned char)COLLECT_ALR;
}


void rl_doszeit(unsigned char i) {
	if(rl[i].ind > 2) {
		if(vek[r3_art_t + 2*i].ui == an_aus_txt && rl[i].prc > 0)
			rl[i].prc = 100;
		if(rl[i].prc > 30) 
			++rl[i].dos_zeit;
		else
		    rl[i].dos_zeit = 0;
		
		if(rl[i].dos_zeit_max > 0) {
			if(rl[i].dos_zeit > rl[i].dos_zeit_max) {
				rl[i].dos_zeit = 0;
				//~ (i > 0) ? (DOSZEIT_RL3 = 1) : (DOSZEIT_RL4 = 1);
				if(rl[i].dos_EEPR != SAVE_FLAG) {
					rl[i].dos_EEPR = SAVE_FLAG;
					adr_128k = status_ix + i;
					ee_wrb_128k(SAVE_FLAG);
				}
			}
		}
	}
}

char phdown_fn(void) {
	xdata char val;
	val = pH_Regelung();
	if (val >= 0) {
		return val;
	} else
		return 0;
}

char phup_fn(void) {
	xdata char val;
	val = pH_Regelung();
	if(val >= 0)
		return 0;
	else
		return abs(val);
}

code char (*fptr_rel[])(void) = {frei_fn, vent_fn, aimb_fn, alarm_fn, phdown_fn, phup_fn, rx_Regelung, cl_Regelung};
//~ code char (*fptr_rel4[])(void) = {frei_fn, aimb_fn, alarm_fn, phdown_fn, phup_fn, rx_fn, cl_fn};

