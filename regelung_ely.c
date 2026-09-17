#include <stdlib.h>
#include "ConstDef.h"
#include "eeprom128.h"
#include "default_werte_ely.h"
#include "main.h"

//~ void add_alarm(unsigned int);
//~ void add_warn(unsigned int);

code float fl_cons[2][vbw_300+1] = {
	{ 11.3, 11.29,  11.52, 11.0,  10.94, 11.0},
	{-0.97,  -0.5, -0.299, -0.2, -0.139, -0.08}
};

xdata unsigned int sec_cnt,
				   sec_cnt2,
				   lev_cnt,
				   i_start_cnt,
				   soll_fluss_lim,
				   prc_flow,
				   luft_cnt_vor,
				   luft_cnt_nach,
				   not_zeit_cnt,
				   v_start_cnt;

xdata unsigned char btr_art_b0,
                    vers, // Anzahl der Versuche bei Durchflußmessung
                    vers_anlf, // Anzahl der Versuche bei Anlage füllen bei 30 g
                    i_cnt,
                    u_cnt,
                    u_cnt_min,
                    u_max_cnt,
                    t_cnt;

bit i_kleiner_5A,
	versuch_1,
	versuch_2,
	flow_alr_b,
	start_flow,
	start_vgang,
	fl_def; // defaulf-Durchfluss

void tgrad_wache(void);

bit ventilator_rl() {
	return (vek[r3_strg_t].ui == ventilator_txt || vek[r4_strg_t].ui == ventilator_txt);
}


void soll_fluss_alr(void) {
	vek[flow0_alr_un_ix].ui = vek[flow0_soll_ix].ui * 1.1;
	vek[flow0_alr_ob_ix].ui = vek[flow0_soll_ix].ui * 0.9;
}

void soll_fluss_ms(unsigned char tp) {
	//~ xdata float f;
	float xdata * xdata fp;
	fp = malloc (4);
	//~ *fp = ((float)vek[cl_konz].ui/10.0 - fl_cons[0][tp])/fl_cons[1][tp];
	//~ #ifdef TEST
		//~ *fp = vek[flow_ely].ui / 10.0;
	//~ #else
		*fp = werte_el[flow_l_h][tp] / 10.0;
		*fp *= (1.0 + vek[flow_ely].i / 100.0);
	//~ #endif
	if(tp < vbw_200)
	//~ if(tp < vbw_150)
		vek[flow0_soll_ix].ui = FL1 / *(fp);
	else
		vek[flow0_soll_ix].ui = FL2 / *(fp);

	//~ *fp = ((float)50/10.0 - fl_cons[0][tp])/fl_cons[1][tp];
	//~ if(tp < vbw_200)
		//~ soll_fluss_lim = FL1 / (*fp);
	//~ else
		//~ soll_fluss_lim = FL2 / (*fp);
	soll_fluss_lim = vek[flow0_soll_ix].ui * 0.9;
	*(fp) = vek[flow0_soll_ix].ui;
	free(fp);
	soll_fluss_alr();
}

void temperatur_fn(void) {
	if(vek[temp_lim_t].ui == ja_txt) {
		if(vek[t0_ix].i > vek[maxt].i * 10) {
			if(vek[flow0_soll_ix].ui > soll_fluss_lim) {
				--vek[flow0_soll_ix].ui;
				soll_fluss_alr();
			}
			tgrad_wache();
			T_WRNH = 1 & T_ALR;
		} else { // if(vek[t0_ix].i < vek[maxt].i * 10)
			soll_fluss_ms(vek[anl_typ_t].b[1] - a30_txt);
			t_cnt = 0;
		}
	} else {
		soll_fluss_ms(vek[anl_typ_t].b[1] - a30_txt);
		if(vek[t0_ix].i > vek[maxt].i * 10) {
			tgrad_wache();
		} else t_cnt = 0;
	}
	#define T_MIN 250 // 25°C
	if(vek[t0_ix].i < T_MIN) {
		add_warn(wrn_tL_txt);
	}
}

bit abgelaufen(void) {
	if(sec_cnt > 0)
		--sec_cnt;
	return 	(bit)sec_cnt;
}

void cnt_ini(void) {
	sec_cnt2    = 0;
	vers        = 0;
	i_cnt       = 0;
	u_cnt       = 0;
	u_cnt_min   = 0;
	u_max_cnt   = 0;
	t_cnt       = 0;
	i_start_cnt = 0;
	lev_cnt     = 0;
	i_kleiner_5A= 0;
	//~ versuch_1   = 0;
	//~ versuch_2   = 0;
	prc_flow    = 0;
	v_start_cnt = 0;
}

void sole_dosierung() {
	float xdata * xdata fp;
	fp = malloc (4);
	//~ typ = vek[anl_typ_t].b[1] - a30_txt;
	*(fp)  = 1.0 + (float)vek[sole_dos].i/100.0;
	*(fp) *= werte_el[sole_ms][vek[anl_typ_t].b[1] - a30_txt];
	//~ *(fp) *= werte_el[sole_ms][typ];
	vek[sole_puls_ix].ui = *(fp);
	free(fp);
}

void ely_ini(void) { // sole_puls, flow_soll, zeit, strom in rel_ini
	//~ xdata unsigned char typ;
	unsigned char typ;

	//~ float xdata * xdata fp;
	//~ fp = malloc (4);
	typ = vek[anl_typ_t].b[1] - a30_txt;
	//~ *(fp)  = 1.0 + (float)vek[sole_dos].i/100.0;
	//~ *(fp) *= werte_el[sole_ms][typ];
	//~ vek[sole_puls_ix].ui = *(fp);
	//~ free(fp);
	sole_dosierung();
	if(vek[btr_art_t].b[1] == run_txt) {
		soll_fluss_ms(typ);
		if(vek[code0_x].ui == TEST_MODE) {
			#define MIN10 600
			#define MIN3  3
			sec_cnt = MIN10;
			vek[sole_nspw].ui = MIN3;
		} else
			sec_cnt = werte_el[sole_nachsp][typ];
	} else {
		vek[sole_puls_ix].ui  = CELLEFILL_SOLE;
		vek[flow0_soll_ix].ui = werte_el[zelle_fill_flow][typ];
		if(btr_art_b0 == cellfill_txt)
			sec_cnt = werte_el[zelle_fill_sek][typ];
		if(btr_art_b0 == anl_fill_txt)
			sec_cnt = werte_el[anl_fill_sek][typ];
	}
	U_ALRL      = 0;
	U_WRNH      = 0;
	flow_alr_b  = 0;
	start_flow  = 0;
	fl_def      = 0;
	cnt_ini();
}

void strom_wache(unsigned char di) {
	#define MINUTE_2 120
	#define MINUTE_3 180
	i_cnt += di;
	if(i_cnt > MINUTE_3) {
		I_ALR = 1;
		i_cnt = 0;
		i_start_cnt = 0;
	}
}

#define I_VERZ 600
void tgrad_wache(void) {
	if(i_start_cnt >= I_VERZ) {
		if(++t_cnt > MINUTE_2) {
			T_ALR = 1;
			t_cnt = 0;
		}
	}
}

void fluss_wache(void) {
	#define FUENF_MIN 300
	#define PRC_150   150
	#define PRC_50    50
	static xdata unsigned char delta;
	unsigned char ix;
	if(prc_flow >= PRC_150) {
		ix = 5; // 60 Sek
		if(!start_flow)
			ix = 1;
		FLOW_ALRH = 1;
		//~ ix = 3; // 15.11.2016: 120 Sek 100
	} else if(prc_flow <= PRC_50) {
		ix = 10; // 30 Sek
		FLOW_ALRL = 1;
	} else {
		ix = 1, start_flow = 1;
	}
	if(delta != ix) {
		sec_cnt2 = 0;
		delta = ix;
	}
	sec_cnt2 += delta;
	if(sec_cnt2 > FUENF_MIN) {
		FLOW_WRN  = 1;
		sec_cnt2  = 0;
		FLOW_ALR  = vers >= 6;
	}
}

bit run_fn(void) {
	if(!start_vgang) {
		add_warn(start_vgang_txt);
		return 1;
	}
	if(luft_cnt_vor) {
		add_warn(vent_vor_txt);
		return 0;
	}
	add_warn(work_txt);
	if(TANK_NOT)
		add_warn(notbtr_txt);

	led_rt(RUN_LED);
	if(i_start_cnt < I_VERZ) {
		xdata unsigned char cnt_5a;
		++i_start_cnt;
		if(versuch_1 & versuch_2)
			cnt_5a = 100;
		else
			cnt_5a = 60; //15;
		if(i_start_cnt == cnt_5a) {
			if(vek[el_A_mv_ix].ui < werte_el[i_anf][vek[anl_typ_t].b[1] - a30_txt])
				i_kleiner_5A = 1;
		}
		if(vek[el_A_mv_ix].ui > werte_el[i_ob_alr][vek[anl_typ_t].b[1] - a30_txt]) {
			//~ strom_wache(4); // 30 Sek
			strom_wache(2); // 90 (60) Sek
			I_ALRH = I_ALR;
		} else if(vek[el_A_mv_ix].ui < (werte_el[i_soll][vek[anl_typ_t].b[1] - a30_txt] / 2)) { // 50 % von Sollstrom
			strom_wache(1);
			I_ALRL = I_ALR;
		} else
			i_cnt = 0;
	} else {
		if(vek[el_A_mv_ix].ui < werte_el[i_un_alr][vek[anl_typ_t].b[1] - a30_txt]) {
			strom_wache(1);
			I_ALRL = I_ALR;
		} else if(vek[el_A_mv_ix].ui > werte_el[i_ob_alr][vek[anl_typ_t].b[1] - a30_txt]) {
			strom_wache(2);
			I_ALRH = I_ALR;
		} else
			i_cnt = 0;
	}

	if(vek[flow0_ix].ui > 0) {
		if(vek[flow0_ix].ui < vek[flow0_alr_ob_ix].ui) {
			flow_alr_b = 1;
			fluss_wache();
		} else if(vek[flow0_ix].ui > vek[flow0_alr_un_ix].ui) {
			fluss_wache();
			flow_alr_b = 0;
		} else {
			sec_cnt2 = vers = 0;
			flow_alr_b = 0;
			FLOW_ALRL = FLOW_ALRH = 0;

		}
	} else
		fluss_wache();

	if(vek[not_btr_t].ui == nein_txt) {
		if(vek[lev_prod_ix].ui < 1) {
			#define MIN30 1800 // 30 Min
			if(++lev_cnt > MIN30) {
				NIVEAU_ALR = 1;
			}
		} else
			lev_cnt = 0;
	}

	temperatur_fn();

/*
	if(vek[el_V_mv_ix].ui > werte_el[u_max][vek[anl_typ_t].b[1] - a30_txt]) {
		if(++u_max_cnt > 60) { // 60 Sek. Verzögerung
			u_max_cnt = 0;
			if(vek[sole_dos].ui < 25) {
				++vek[sole_dos].i;
				sole_dosierung();
			}
		}
	} else if(vek[el_V_mv_ix].ui < (werte_el[u_max][vek[anl_typ_t].b[1] - a30_txt] - 2000) ) {
		if(++u_max_cnt > 60) { // 60 Sek. Verzögerung
			u_max_cnt = 0;
			if(vek[sole_dos].i > 0) {
				--vek[sole_dos].i;
				sole_dosierung();
			}
		}
	} else
		u_max_cnt = 0;
*/

	if(v_start_cnt < ZEHN_MIN)
		++v_start_cnt;
	if(vek[el_V_mv_ix].ui > werte_el[u_alr][vek[anl_typ_t].b[1] - a30_txt]) {
		U_WRNH = 1;
	 	if(++u_cnt > 30) {	// 30 Sek.
			if(vek[v_monitus_t].ui == ja_txt && v_start_cnt >= ZEHN_MIN) {
				U_ALRH = 1;
				U_WRNH = 0;
				u_cnt  = 0;
			}
		}
	} else {
		U_WRNH = 0;
		u_cnt = 0;
	}

	if(vek[el_V_mv_ix].i < 1000) {
		#define U_LOW_DELAY 20 // Sek.
		if(++u_cnt_min > U_LOW_DELAY) {	// 20 Sek.
			U_ALRL     = 1;
			u_cnt_min  = 0;
		}
	} else
		u_cnt_min = 0;

	if(!TANK_NOT)
		TANK_VOLL = vek[lev_prod_ix].ui > vek[prodlev_max].ui;
	//~ else {

	//~ }
	if(U_WRNH)
		add_alarm(alr_UH_txt, 0);

	return (TANK_VOLL | ~abgelaufen() | FLOW_WRN | I_ALR | T_ALR | NIVEAU_ALR | ANL_ALR | U_ALRL | U_ALRH  | OVFL_ALR | i_kleiner_5A) ;
}


bit stanby_fn(void) {
	static xdata cnt_fl = 0;
	bit b;
	b = 0;
	if(FLOW_WRN) {
		if(FLOW_ALR) {
			cnt_ini();
			if(flow_alr_b) {
				add_alarm(alr_flH_txt,1);
			} else {
				add_alarm(alr_flL_txt,1);
			}
		} else if(++sec_cnt2 > DREI_MIN) {
			unsigned char v;
			v = vers;
			FLOW_WRN   = 0;
			flow_alr_b = 0;
			cnt_ini();
			vers = v;
			++vers;
			b = 1;
		} else {
			if(flow_alr_b)
				add_warn(alr_flH_txt);
			else
				add_warn(alr_flL_txt);
			b = 0;
		}
	} else if(I_ALR) {
		i_cnt = 0;
		versuch_1 = versuch_2 = 0;
		if(I_ALRH) {
			add_alarm(alr_iH_txt,1);
		}
		if(I_ALRL) {
			add_alarm(alr_iL_txt,1);
		}
	} else if(T_ALR) {
		t_cnt = 0;
		add_alarm(alr_t_txt,1);
	} else if(SOLE_ALR) {
		add_alarm(alr_sole_txt,1);
	} else if(NIVEAU_ALR) {
		add_alarm(alr_prod_txt,1);
	} else if(ANL_ALR) {
		add_alarm(h2_alr_txt,1);
	} else if(OVFL_ALR) {
		add_alarm(ovfl_txt,1);
	} else if(U_ALRH) {
		add_alarm(alr_UH_txt,1);
	} else if(U_ALRL) {
		add_alarm(alr_UL_txt,1);
	} else if(prc_flow) {
		if(cnt_fl < 10)
			++cnt_fl;
		else {
			add_alarm(alr_flH_txt,0);
		}
	} else {
		cnt_ini();
		add_warn(standby_txt);
		if(TANK_NOT) {
			add_warn(notbtr_txt);
			b = (not_zeit_cnt == 0);
		} else {
			b = vek[lev_prod_ix].ui < vek[prodlev_min].ui;
		}
		cnt_fl = 0;
	}
	led_rt(LED_AUS);

	return b;
}

bit solefill_fn(void) {
	if(vek[status_ix].b[1] & SOLE_KONT) { // SOLE_KONT=0 - Soletank ist voll
		led_rt(SOLE_LED);
		if(vek[status_ix].b[1] & SOLE_FIL)
			add_warn(solefill_txt);
		if(++sec_cnt2 > (vek[sole_nspw].ui * 60)) {
			SOLE_ALR = 1;
			return 1;
		}
		return 0;
	} else {
		add_warn(solevoll_txt);
		start_vgang = 0;
		return  1; // SOLE_KONT=0 - Soletank ist voll
	}
}


void flow_100prc(void) {
	if(fl_def || !start_vgang) {
		if(prc_flow > 95 && prc_flow < 105) {
			sec_cnt = 1;
			start_vgang = 1;
		}
		if(vek[lev_prod_ix].ui > vek[prodlev_max].ui) {
			sec_cnt = 1;
			start_vgang = 1;
			btr_art_b0 = standby_txt;
		}
	}
}


void set_sec(unsigned char art){
	sec_cnt = werte_el[art][vek[anl_typ_t].b[1] - a30_txt];
}

bit cellfill_fn(void) {
	flow_100prc();
	if(abgelaufen()) {
		if(vek[status_ix].b[1] & CELLE_FIL) {
			add_warn(cellfill_txt);
		}
		led_rt(LED_AUS);
		//~ fl_def = 0;
		if(prc_flow < 20) {
			set_sec(zelle_fill_sek);
		}
		return 0;
	} else {
		if(!fl_def){
			fl_def = 1;
			soll_fluss_ms(vek[anl_typ_t].b[1] - a30_txt);
			sec_cnt = 120;
			return 0;
		}
		add_warn(cellvoll_txt);
		i_kleiner_5A = 0;
		i_start_cnt  = 0;
		fl_def = 0;
		return 1;
	}
}


bit anlfill_fn(void){
	flow_100prc();
	if(abgelaufen()) {
		if(vek[status_ix].b[1] & ANL_FIL) {
			if(start_vgang)
				add_warn(anl_fill_txt);
			else {
				add_warn(start_vgang_txt);
			}
		}
		if(prc_flow < 20) {
			set_sec(anl_fill_sek);
		}
		//~ fl_def = 0;
		return 0;
	} else {
		if(!fl_def){
			fl_def  = 1;
			sec_cnt = 120;
			soll_fluss_ms(vek[anl_typ_t].b[1] - a30_txt);
			return 0;
		}
		if(start_vgang == 1)
			add_warn(anl_voll_txt);
		if(!start_vgang) {
			if(vers_anlf < 2)
				++vers_anlf;
			else {
				FLOW_ALR = 1;
				FLOW_WRN = 1;
				start_vgang = 1;
			}
		}
		fl_def = 0;
		return 1;
	}
}

bit defaultfn(void) { // aus_txt
	add_warn(ely_aus_txt);
	led_rt(LED_AUS);
	return 0;
}

#if 0
float prc_T_48() {
    // reg_r[lev_mm], vek[prod_aus] -> 50%, vek[prod_ein] -> 70%, vek[prod_min] -> 100%
    float ret, b;
    b   = 0.3 / (vek[prodlev_max] - vek[prodlev_min]);
    ret = b * (vek[prodlev_max].i - reg_r[lev_prod_ix].i) + 0.5;
    if(ret > 1.0)
        ret = 1.0;
    else if(ret < 0.5)
        ret = 0.5;
    return ret;
}
#endif

code bit (*fptr_ely[])(void) = {defaultfn, run_fn, solefill_fn, cellfill_fn, anlfill_fn, stanby_fn};

