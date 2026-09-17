#include <math.h>
//~ #include "ConstDef.h"
#include "local_var.h"
#include "eeprom128.h"
#include "eeprom_128drv.h"
#include "main.h"
#include "menu_lcd.h"

void frei_fn(unsigned char);
void rx_rechnen(unsigned char);
void ph_rechnen(unsigned char);
void fl_rechnen(unsigned char);

code void (*fptr_m12[])(unsigned char) = {frei_fn, rx_rechnen, ph_rechnen, ph_rechnen, fl_rechnen};

//~ xdata int ph_mv_corr;

void frei_fn(unsigned char in) {
	;
}
void rx_rechnen(unsigned char in) {
	vek[in].i = vek[in + 1].i + vek[in + 3].i;
}

float tcomp_fn(unsigned char ind) {
	xdata float mv_crr;
	xdata unsigned char t;
	mv_crr = vek[ind].i ;	// mV
	//~ mv_corr /= (273 + t_gradC);
	if(vek[t_comp_txt].ui == auto_txt)
		t = t0;
	else
		t = t_man_comp;	
	mv_crr /= (273 + vek[t].ui);
	mv_crr *= 298;
	return mv_crr;
	
}


void fl_rechnen(unsigned char in) {
	bit b;
	xdata float mv_corr;
	mv_corr = tcomp_fn(in+1);	
	b = (bit)(in % m1_val);
	if(b)
		mv_corr -= vek[m1_ph_null_mv].i;
	else
		mv_corr -= vek[m2_ph_null_mv].i;
	//~ f.f = (float)vek[in + 1].i / fl_sth[b];
	f.f = mv_corr / fl_sth[b];
	f.f = fl_0[b] - f.f;
	f.f *= log(10);
	f.f = exp(f.f);
	vek[in].ui = f.f * 100; 
}


void pH_rechnen(unsigned char in) {
	
	xdata float f;
	xdata float mv_corr;
	mv_corr = tcomp_fn(in+1);	
	f  = 1e3 / vek[in + 2].i; 	// ph_sth 1 Nachkommastelle, ph-Wert 2 Nachkommastellen
	f *= mv_corr;
	if(in == m1_val)
		vek[in].i = phf0[0] - f;
	else	
		vek[in].i = phf0[1] - f;
	if(vek[in].i < 0)         vek[in].i = 0;
	else if(vek[in].i > 1400) vek[in].i = 1400;
}

