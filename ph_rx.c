#include <math.h>
#include <string.h>
#include "ConstDef.h"
//#include "Extern.h"
#include "eeprom128.h"
#include "main.h"

static xdata float isum_ph; 
static xdata float isum_des; 
static xdata float kp_rx; 
static xdata float kp_ph; 
static xdata float kp_des; 
static xdata float ki_ph; 
static xdata float ki_des; 
					
xdata unsigned int  doszeit_Des,	
					doszeit_PH_senken,	
					doszeit_PH_heben;

bit PH_START_CLBR;
xdata unsigned char PH_CLBR;

// void SteilheitPH(void)
// {
	// vek[sth_ph].ui = (vek[pH7].ui - vek[pH4].ui) / abs(vek[U7].ui - vek[U4].ui); // pH/mV
// }


void ph_rx_init(void)
{
	vek[rx_mv].ui = 0;
	vek[ph].ui = 0;
	PH_START_CLBR = 0;
	isum_ph = isum_des = 0.0;
	kp_rx = 100.0 / vek[rx_p].ui; // vek[5] - P-Bereich Redox
	kp_ph = 10000.0 / vek[ph_p].ui;	
	if(vek[ph_i].ui)
		ki_ph = kp_ph / vek[46].ui;
	else
		ki_ph = 0;
}

// void kalibrieren(void)
// {
	// xdata float f7,f4;
	// if(PH_START_CLBR)
	// {
		// PH_CLBR = 0;
		// f7 = (7.0 - (float)vek[pH7].ui)*580.0;
		// f4 = (7.0 - (float)vek[pH4].ui)*580.0;
		// if(fabs(vek[ph_mv].ui*10 - f7) < 600.0) 
		// {
			// vek[U7].ui = vek[ph_mv].ui;
			// // ee_wrt_reqi(42,U7.b[0]); //6186
			// // ee_wrt_reqi(43,U7.b[1]);
			// // ee_wrt_reqi(44,U7.b[2]);
			// // ee_wrt_reqi(45,U7.b[3]);
			// // vek[87].ui = vek[84].ui;// / 10.0; Nullpunktabweichung pH in mV
			// //if(vek[84].ui > 0) vek[11].ui = vek[84].ui / 10.0; // nur für Display.c  Zeile 368
			// // PH_CLBR = 7;
			// SteilheitPH();
		// }
		// else if(fabs(vek[ph_mv].ui - f4) < 600.0) 
		// {
			// vek[U4].ui = vek[ph_mv].ui;
			// // ee_wrt_reqi(38,U7.b[0]);// 6182
			// // ee_wrt_reqi(39,U7.b[1]);
			// // ee_wrt_reqi(40,U7.b[2]);
			// // ee_wrt_reqi(41,U7.b[3]);
			// // PH_CLBR = 4;
			// SteilheitPH();
		// }
		// PH_START_CLBR = 0;
	// }
// }

void kalibrieren(void)
{
	xdata float f7,f4;
	if(PH_START_CLBR)
	{
		if(vek[ph_cal_art].i == 0) // 1 Punkt Kalibrierung
		{	
			U7 = 0;
		}
		else // 2 Punkt Kalibrierung
		{
			
		}
		PH_CLBR = 0;
		f7 = (7.0 - (float)vek[pH7].ui)*580.0;
		f4 = (7.0 - (float)vek[pH4].ui)*580.0;
		if(fabs(vek[ph_mv].ui*10 - f7) < 600.0) 
		{
			vek[U7].ui = vek[ph_mv].ui;
			// ee_wrt_reqi(42,U7.b[0]); //6186
			// ee_wrt_reqi(43,U7.b[1]);
			// ee_wrt_reqi(44,U7.b[2]);
			// ee_wrt_reqi(45,U7.b[3]);
			// vek[87].ui = vek[84].ui;// / 10.0; Nullpunktabweichung pH in mV
			//if(vek[84].ui > 0) vek[11].ui = vek[84].ui / 10.0; // nur für Display.c  Zeile 368
			// PH_CLBR = 7;
			SteilheitPH();
		}
		else if(fabs(vek[ph_mv].ui - f4) < 600.0) 
		{
			vek[U4].ui = vek[ph_mv].ui;
			// ee_wrt_reqi(38,U7.b[0]);// 6182
			// ee_wrt_reqi(39,U7.b[1]);
			// ee_wrt_reqi(40,U7.b[2]);
			// ee_wrt_reqi(41,U7.b[3]);
			// PH_CLBR = 4;
			SteilheitPH();
		}
		PH_START_CLBR = 0;
	}
}



void ph_ausrechnen(void)
{
	xdata float f;
	
	// vek[sth_ph].ui = 58;
	// U4 = 174;
	// U7 = 0;
	// pH4 = 4;
	
	f = vek[U4].ui - vek[ph_mv].ui;
	f = f / vek[sth_ph].ui;
	f += vek[pH4].ui;
	if(f > 14.0) f = 14.0;
	if(f < 0)    f = 0;
	kalibrieren();
	// Grenzen: 	Redox: 	50 - 950mV
			// pH:		3 - 12
	if(f < 3.0 || f > 12.0)  PH_FAIL = 1;	
	else PH_FAIL = 0;
	vek[ph].ui = f * 100.0;
}

/*
void rx_ausrechnen(void)
{
	REDOX_FAIL = 1;
	// Ausrechnen
		// Grenzen: 	Redox: 	50 - 950mV
					// pH:		3 - 12    // Mod. Redox
		if((vek[rx_mv].ui < 50  || vek[rx_mv].ui > 950) && vek[des].ui==3)   REDOX_FAIL = 1;	
		else REDOX_FAIL = 0;	
	
}
*/
char pH_Regelung(void)
{
	xdata float dif;
	xdata char s;
	
	dif = vek[ph].ui - (vek[41].ui / 100.0); // pH Sollwert 
	isum_ph += dif;
	if(dif >= 0) s = 1; else s = -1;
	dif = fabs(dif);
	if(dif > (vek[45].ui / 100.0)) // Hysterese
	{
		dif *= s;
		dif *= kp_ph;// 1e4 / vek[2].ui;
		dif += ki_ph * isum_ph;
		if(dif > 100.0) dif = 100.0;
		if(dif <-100.0) dif =-100.0;
		return dif;
	}
	else return 0.0;
}

char rx_Regelung(void)
{
	xdata float dif;
	dif = vek[83].ui - vek[28].ui; //vek[4]  -->> Sollwert Redox
	dif = dif * kp_rx;
	if(dif > 0) // rx senken
	{
		dif = 0.0;
	}
	else // rx heben
	{
		dif = fabs(dif);
		if(dif > 100.0) dif = 100.0;
		if(dif > 30.0) doszeit_Des++;
		else           doszeit_Des = 0;
		
//		if(doszeit_Des>Dos_ZeitRL0 && Dos_ZeitRL0>0) DOSZEIT_RL0 = 1;	
		
		return dif;
	}
}
