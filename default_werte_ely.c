#include "ConstDef.h"
#define IC1 (3000/30)
#define IC3 (3000/50)
#define IC2 (3000/100)
#define prc_5  (0.05)
#define prc_10 (0.1)

#define i_5A   5.0
#define i_30  22.0
#define i_150 29.0
//~ #define i_150 31.5
#define i_200 42.0
//~ #define i_300 53.0
//~ #define i_300 68.0
#define i_300 67.0

#define i_anf_5A     (i_5A  * IC1)	// 
#define i_anf_5A_300 (i_5A  * IC2)	// 
#define i_soll_30    (i_30  * IC1)	// 50A - 3000 mV 
#define i_soll_150   (i_150 * IC3)
#define i_soll_200   (i_200 * IC3)
#define i_soll_300   (i_300 * IC2)	// 100 A - 3000 mV

#define i_ob_alr_30  (i_soll_30  * (1 + prc_10))	// 50A - 3000 mV 
#define i_ob_alr_150 (i_soll_150 * (1 + prc_10))
#define i_ob_alr_200 (i_soll_200 * (1 + prc_10))
#define i_ob_alr_300 (i_soll_300 * (1 + prc_10))	// 100 A - 3000 mV

#define i_un_alr_30  (i_soll_30  * (1 - prc_10))	// 50A - 3000 mV 
#define i_un_alr_150 (i_soll_150 * (1 - prc_10))
#define i_un_alr_200 (i_soll_200 * (1 - prc_10))
#define i_un_alr_300 (i_soll_300 * (1 - prc_10))	// 100 A - 3000 mV

#define i_ob_wrn_30  (i_soll_30  * (1 + prc_5))	// 50A - 3000 mV 
#define i_ob_wrn_150 (i_soll_150 * (1 + prc_5))
#define i_ob_wrn_200 (i_soll_200 * (1 + prc_5))
#define i_ob_wrn_300 (i_soll_300 * (1 + prc_5))	// 100 A - 3000 mV

#define i_un_wrn_30  (i_soll_30  * (1 - prc_5))	// 50A - 3000 mV 
#define i_un_wrn_150 (i_soll_150 * (1 - prc_5))
#define i_un_wrn_200 (i_soll_200 * (1 - prc_5))
#define i_un_wrn_300 (i_soll_300 * (1 - prc_5))	// 100 A - 3000 mV

#define zellefill_1  (FL1 / 20.0)
#define zellefill_2  (FL2 / 20.0)

code unsigned int werte_el[flow_l_h+1][vbw_300+1] = {	
//					   vbw_30        vbw_60  	   vbw_90        vbw_150        vbw_200        vbw_300
/* i_ob_alr        */ {i_ob_alr_30,  i_ob_alr_30,  i_ob_alr_30,  i_ob_alr_150,  i_ob_alr_200,  i_ob_alr_300},
/* i_soll          */ {  i_soll_30,    i_soll_30,    i_soll_30,    i_soll_150,    i_soll_200,    i_soll_300}, 
/* i_un_arm  	   */ {i_un_alr_30,  i_un_alr_30,  i_un_alr_30,  i_un_alr_150,  i_un_alr_200,  i_un_alr_300},
/* u_alr           */ {       9200,        13800,        23000,         23000,         23000,         27600},
/* u_max           */ {       8400,        12600,        21000,         21000,         21000,/*25200*/21000},
/* sole_ms         */ {        320,          540,         1180/*1000*/,  1654/*1560*/,  2200,          3184},	// 13.01.2020, 25.11.2020
/* zelle_fill_sek  */ {         60,          120,          120,           180,           180,           240},
/* anl_fill_sek    */ {        240,          240,          240,           300,           300,           400},
/* zelle_fill_flow */ {zellefill_1,  zellefill_1,  zellefill_1,   zellefill_1,   zellefill_2,   zellefill_2},
/* sole_nachsp     */ {      52956,        29988,        16380,         10584,          8172,          5292},
/* i_anf           */ {   i_anf_5A,     i_anf_5A,     i_anf_5A,      i_anf_5A,      i_anf_5A,  i_anf_5A_300},
/* flow_l_h        */ {         48,           90,          160,           230,           300,           460}  // * 10
};

//~ Zellenspannung, V			10			15				24			24				24				30			
