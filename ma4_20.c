#include "ConstDef.h"
#include "eeprom128.h"
#include "regelung.h"

#define RX_MIN 0
#define RX_MAX 1000
#define PH_MIN 0
#define PH_MAX 1000
#define T_MIN 0
#define T_MAX 100
#define MA_TEST 2244

extern code WERTE tab1[];

FLOAT ma_ini(unsigned char enm)
{
	static xdata FLOAT fl;
	//static xdata int x,y;
	switch(vek[enm].b[1]) {
		default:
			switch(des)	{
				default:
					fl.vui[0].i = tab1[cl_soll].min;
					fl.vui[1].i = tab1[cl_soll].max;
				break;
				case pc_txt:
					fl.vui[0].i = tab1[pc_soll].min;
					fl.vui[1].i = tab1[pc_soll].max;
				break;
				case oz_txt:
					fl.vui[0].i = tab1[oz_soll].min;
					fl.vui[1].i = tab1[oz_soll].max;
				break;
				case cldx_txt:
					fl.vui[0].i = tab1[cldx_soll].min;
					fl.vui[1].i = tab1[cldx_soll].max;
				break;
			}
		break;
		case rx_analog_txt:
			fl.vui[0].i = RX_MIN;
			fl.vui[1].i = RX_MAX;
		break;
		case ph_txt:
			fl.vui[0].i = PH_MIN;
			fl.vui[1].i = PH_MAX;
		break;
		case grad_txt:
			fl.vui[0].i = T_MIN;
			fl.vui[1].i = T_MAX;
		break;
	}
	return fl;
}	

int ma_lim(int i, FLOAT f)
{	
	if(i < f.vui[0].i) 
		i = f.vui[0].i;
	else if(i > f.vui[1].i)
		i = f.vui[1].i;
	return i;	
}

void ma_out(unsigned char n) // 0..3
{
/* out1_art,
   out1,
   out1_4ma,
   out1_20ma,
   out1_ma,
*/	
	xdata float a;
	xdata int c,
			  val;
    xdata unsigned char b;
	  				    //~ ind;
	
	
	
	//~ n *= 2;
	//~ b = out1 + n*2;
	//~ switch(vek[b].ui)
	switch(vek[out1 + n*2].ui) {
		case rx_analog_txt:
			val = vek[rx_mv].i;
		break;
		case ph_txt:
			val = vek[ph].i;
		break;
		case grad_txt:
			val = vek[t0].i / 10;
		break;
		default:
			//~ val = vek[des_med].i;
			val = vek[des_mwert].i;
		break;
	}
	//~ val = vek[ind].i;
	//~ ind = n / 4;
	n *= 2;
	c = vek[out1_20ma + n].i - vek[out1_4ma + n].i;

	if(c > 0) {
		if     (val < vek[out1_4ma  + n].i ) val = vek[out1_4ma  + n].i;
		else if(val > vek[out1_20ma + n].i)  val = vek[out1_20ma + n].i;
	}
	else if(c < 0) {
		if     (val > vek[out1_4ma  + n].i ) val = vek[out1_4ma  + n].i;
		else if(val < vek[out1_20ma + n].i)  val = vek[out1_20ma + n].i;
	}
	else {
		vek[out1_ma + n/2].i = 0;
		return;
	}

	if(vek[out1_art + n].ui == mA_4_20txt) {
		a = 16.0 / c;
		(c > 0) ? (b = 4, c = vek[out1_4ma  + n].i) : (b = 20, c = vek[out1_20ma + n].i);
	}
	else {
		a = 20.0 / c;
		(c > 0) ? (b = 0, c = vek[out1_4ma  + n].i) : (b = 20, c = vek[out1_20ma + n].i);
	}
	
	a = a * (val - c) + b;
	vek[out1_ma + n/2].i = a * 100; // 2000 ca. 20 mA
}
