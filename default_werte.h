#ifndef DEFAULT_WERTE_H
#define DEFAULT_WERTE_H

#include "ConstDef.h"
// Bits 0..1 - Code A,B,C,D, Bits 2..4 - Betriebsart, Bits 5,6 - Nachkommastellen
// Betriebsart: 0 - immer anzeigen, 1 - PH - RX, 2 - PCZG - PH, 3 - PCZG, 4 - Chlor - RX - PH, 5 - Chlor - PH, 6 - PC - PH, 7 - Chlor 

extern code WERTE tab1[];

#if 0
code WERTE tab1[] =
{
// dflt,  min,  max 
	{  138,   135,   138}, //"Desinfektion: $", 				//  C,0,0 ?des Freies Chlor-0,	Poolcare-1, Poolcare (zeitgesteuert)-2,	Redox-3,...., -15
	{  139,   139,   140}, //"Relais 2 $:",					   	//  C,0,0 ?rel2 pH senken, Flockung		
	{  141,   141,   142}, //"Redoxmessung $:",					//  C,1,0 ?rx_mess ja, nein		
	{    5,     0,    20}, //"Einschaltverzögerung: $",			//  C,1,0 ?on_dly		
	{   15,     0,    60}, //"Alarmverzögerung:     $",			//  C,1,0 ?alr_dly		
	{  144,   143,   144}, //"Kalibrierungsart  $",				//  B,0,0 ?ph_cal_art Ein-Punkt, Zwei-Punkt
	{  100,     0,   400}, //"Alarmwert oben  $ mg/L",			//  A,1,2 ?cl_alr_ob
	{   70,     0,   400}, //"Warnwert oben   $ mg/L", 			//  A,1,2 ?cl_wr_ob
	{   40,     0,   400}, //"Sollwert        $ mg/L",			//  A,1,2 ?cl_soll
	{   20,     0,   400}, //"Warnwert unten  $ mg/L",			//  A,1,2 ?cl_wr_un
	{   10,     0,   400}, //"Alarmwert unten $ mg/L",			//  A,1,2 ?cl_alr_un
	{   10,    10,   100}, //"P-Bereich              $ mg/L",	//  B,1,2 ?cl_p
	{    0,     0,  1800}, //"I-Anteil/Nachstellzeit $ Sek",	//  B,1,0 ?cl_i
	{    5,     0,    60}, //"man. Dauerdosierung   $ Min",		//  B,1,0 ?cl_man_dos
	{  148,   147,   148}, //"Dauerdosierung        $",			//  B,1,0 ?cl_ddos_on einschalten, ausschalten
	{   60,     0,   180}, //"Dosierzeitüberwachung $ Min",		//  B,1,0 ?cl_dos_watch
	{  650,   100,   800}, //"Sollwert        $ mV",			//  A,4,0 ?rx_soll
	{  600,   100,   800}, //"Warnwert unten  $ mV",			//  A,4,0 ?rx_wr_un
	{  550,   100,   800}, //"Alarmwert unten $ mV",			//  A,4,0 ?rx_alr_un
	{   10,     0,   100}, //"P-Bereich  $ mV",					//  B,4,0 ?rx_p
	{    5,     0,   500}, //"man. Dauerdosierung   $ Min",		//  B,4,0 ?rx_man_dos
	{  148,   147,   148}, //"Dauerdosierung        $",			//  B,4,0 ?rx_ddos_on einschalten, ausschalten
	{   60,     0,   180}, //"Dosierzeitüberwachung $ Min",		//  B,4,0 ?rx_dos_watch
	{   45,     0,   100}, //"Alarmwert oben   $ mg/L",			//  A,2,2 ?pc_alr_ob
	{   40,     0,   100}, //"Warnwert oben    $ mg/L",			//  A,2,2 ?pc_wr_ob
	{   30,     0,   100}, //"Sollwert         $ mg/L",	    	//  A,2,2 ?pc_soll
	{   15,     0,   100}, //"Warnwert unten   $ mg/L",			//  A,2,2 ?pc_wr_un
	{    5,     0,   100}, //"Alarmwert unten  $ mg/L",			//  A,2,2 ?pc_alr_un
	{    5,     0,    20}, //"P-Bereich              $",		//  B,2,2 ?pc_p
	{    5,     1,    15}, //"Hysterese              $",		//  B,2,2 ?pc_hys
	{    0,     0,  1800}, //"I-Anteil/Nachstellzeit $ Sek",	//  B,2,0 ?pc_i
	{    5,     0,    60}, //"man. Dauerdosierung   $ Min",		//  B,2,0 ?pc_man_dos
	{  148,   147,   148}, //"Dauerdosierung        $",			//  B,2,0 ?pc_ddos_on einschalten, ausschalten
	{   60,     0,   180}, //"Dosierzeitüberwachung $ Min",		//  B,2,0 ?pc_dos_watch
	{   50,     1,   150}, //"Beckenvolumen            $ m3",	//  A,3,0 ?pczg_beck
	{   12,     0,    24}, //"Umwälzzeit               $ h",	//  A,3,0 ?pczg_uzeit
	{    0,   -20,    20}, //"Feindosierung            $ %",	//  A,3,0 ?pczg_fein
	{    1,     0,     3}, //"Startdosierung           $ L/10m3",//  A,3,0 ?pczg_start
	{  800,   100,  1300}, //"Alarmwert oben  $",				//  A,0,2 ?ph_alr_ob
	{  760,   100,  1300}, //"Warnwert oben   $",				//  A,0,2 ?ph_wr_ob
	{  720,   100,  1300}, //"Sollwert        $",				//  A,0,2 ?ph_soll
	{  680,   100,  1300}, //"Warnwert unten  $",				//  A,0,2 ?ph_wr_un
	{  640,   100,  1300}, //"Alarmwert unten $",				//  A,0,2 ?ph_alr_un
	{  100,    50,   200}, //"P-Bereich              $",		//  B,0,2 ?ph_p
	{   10,     5,   200}, //"Hysterese              $",		//  B,0,2 ?ph_hys
	{    0,     0,   600}, //"I-Anteil/Nachstellzeit $ Sek",	//  B,0,0 ?ph_i
	{    0,     0,   100}, //"Totband                $ pH",		//  B,0,0 ?ph_totband
	{    5,     0,   500}, //"man. Dauerdosierung   $ Min",		//  B,0,0 ?ph_man_dos
	{  148,   147,   148}, //"Dauerdosierung        $",			//  B,0,0 ?ph_ddos_on einschalten, ausschalten
	{   60,     0,   180}, //"Dosierzeitüberwachung $ Min",		//  B,0,0 ?ph_dos_watch
	{  143,   142,   143}, //"Kompensation            $", //   	//  B,0,0 ?t_comp Man./autom.
	{   25,     0,    50}, //"Manuelle T-Kompensation $",	    //  B,0,0 ?t_man_comp
	{  129,   129,   131}, // "Reglerart             $", 		//  C,0,0 ?rl_0_art Reglerart: 0 - Aus, 1 - Ein-Aus, 2 - Pulsfrequenz, 3 - Puls-Pause
	{ 7200,     0,  7200}, //"Pulsfrequenz          $ P/h",		//  C,0,0 ?rl_0_frq
	{   10,    10,    60}, //"Puls-Pause Verhältnis $ Sek",		//  C,0,0 ?rl_0_pp
	{   10,     5,   100}, //"Mindestimpuls         $ Sek",		//  C,0,0 ?rl_0_min
	{  129,   129,   131}, //"Reglerart             $", 		//  C,0,0 ?rl_1_art Reglerart: 0 - Aus, 1 - Ein-Aus, 2 - Pulsfrequenz, 3 - Puls-Pause
	{ 7200,     0,  7200}, //"Pulsfrequenz          $ P/h",		//  C,0,0 ?rl_1_frq
	{   10,    10,    60}, //"Puls-Pause Verhältnis $ Sek",		//  C,0,0 ?rl_1_pp
	{   10,     5,   100}, //"Mindestimpuls         $ Sek",		//  C,0,0 ?rl_1_min
	{  129,   129,   131}, //"Reglerart             $", 		//  C,0,0 ?rl_2_art Reglerart: 0 - Aus, 1 - Ein-Aus, 2 - Pulsfrequenz, 3 - Puls-Pause
	{ 7200,     0,  7200}, //"Pulsfrequenz          $ P/h",		//  C,0,0 ?rl_2_frq
	{   10,    10,    60}, //"Puls-Pause Verhältnis $ Sek",		//  C,0,0 ?rl_2_pp
	{   10,     5,   100}, //"Mindestimpuls         $ Sek",		//  C,0,0 ?rl_2_min
	{  129,   129,   131}, //"Reglerart             $", 		//  C,0,0 ?rl_3_art Reglerart: 0 - Aus, 1 - Ein-Aus, 2 - Pulsfrequenz, 3 - Puls-Pause
	{ 7200,     0,  7200}, //"Pulsfrequenz          $ P/h",		//  C,0,0 ?rl_3_frq
	{   10,    10,    60}, //"Puls-Pause Verhältnis $ Sek",		//  C,0,0 ?rl_3_pp
	{   10,     5,   100}, //"Mindestimpuls         $ Sek",		//  C,0,0 ?rl_3_min
	{    0,     0, 65535}, //"Gerätenummer  $",					//  A,0,0 ?dev_nr
	{  910,   910, 65535}, //"Softwarestand $",					//  A,0,0 ?softver
	{  910,   910,  1299}, //"Herstelldatum $",					//  A,0,0 ?mdate
	{  151,   151,   156}, //"Sprache:   $",  					//  A,0,0 ?sprache	
};
#endif
#endif
