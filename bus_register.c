#include "ConstDef.h"

//~ code unsigned char reg[224] =
//~ {
	  //~ softver_ix,       cl,        ph,       rx,      t0, flow_aktl,   status,   softver_ix,  cl_soll,  cl_wr_ob,  	// 0  -    9
	//~ cl_alr_ob, cl_wr_un, cl_alr_un,     cl_p,    cl_i,  softver_ix,   softver_ix,   softver_ix,  softver_ix,   softver_ix,  	// 10 -   19
	  //~ softver_ix,  softver_ix,     rx_mv,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	// 20 -   29
	  //~ softver_ix,       pc,        ph,  softver_ix,      t0, flow_aktl,   status,   softver_ix,  pc_soll,  pc_wr_ob, 	// 30 -   39
	//~ pc_alr_ob, pc_wr_un, pc_alr_un,     pc_p,    pc_i,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	// 40 -   49
	  //~ softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	// 50 -   59
	  //~ softver_ix,       oz,        ph,    rx_mv,      t0, flow_aktl,   status,   softver_ix,  oz_soll,  oz_wr_ob, 	// 60 -   69
	//~ oz_alr_ob, oz_wr_un, oz_alr_un,     oz_p,    oz_i,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	// 70 -   79
	  //~ softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	// 80 -   89
	  //~ softver_ix,     cldx,        ph,       rx,      t0, flow_aktl,   status,   softver_ix,cldx_soll,cldx_wr_ob, 	// 90 -   99
  //~ cldx_alr_ob,cldx_wr_un,cldx_alr_un, cldx_p,  cldx_i,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//100 -  109
	  //~ softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//110 -  119
	  //~ softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//120 -  129
	  //~ softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//130 -  139
	  //~ softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//140 -  149
	  //~ softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//150 -  159
	  //~ softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//160 -  169
	  //~ softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//170 -  179
	  //~ softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix,	    //180 -  189
	    //~ contr,  softver_ix,         0,        1,       2,         3,        4,         5,        6,         7, 	//190 -  199
		    //~ 8,        9,        10,       11,      12,        13,       14,        15,  softver_ix,   wrteptr,		//200 -  209
	    //~ mdate,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix, lng_adr0,  lng_adr1,		//210 -  219
     //~ lng_adr2,  softver_ix,   softver_ix,  wrteptr                                                					//220 -  223
//~ };                                  
code unsigned char reg[224] =
{
	    cl_mv_ix,   cl_val_ix,        ph_ix,       rx_ix,      t1_ix,fl_aktl_mw_ix,   status_ix,  softver_ix,      cl_soll,     cl_wr_ob, 	//0 -  9
	   cl_alr_ob,    cl_wr_un,    cl_alr_un,        cl_p,       cl_i,      ph_soll,    ph_wr_ob,    ph_alr_ob,    ph_wr_un,    ph_alr_un, 	//10 -  19
	        ph_p,       ph_zi,      rx_soll,  	rx_wr_un,  rx_alr_un,         rx_p,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//20 -  29
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//30 -  39
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//40 -  49
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//50 -  59
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//60 -  69
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//70 -  79
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//80 -  89
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//90 -  99
	  anl_typ_t,   el_A_mv_ix,   el_V_mv_ix,    flow0_ix, btr_art_t,        t0_ix,     cl_konz,  lev_prod_ix,      stat_1,     stat_ely, 	//100 -  109
   prodlev_ovfl,  prodlev_max,  prodlev_min,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//110 -  119
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//120 -  129
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//130 -  139
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//140 -  149
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//150 -  159
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//160 -  169
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix, 	//170 -  179
	  softver_ix,  softver_ix,   softver_ix,  softver_ix, softver_ix,   softver_ix,  softver_ix,   softver_ix,  softver_ix,   softver_ix,	//180 -  189
		   contr,  softver_ix,            0,           1,          2,            3,           4,            5,           6,            7, 	//190 -  199
		       8,           9,           10,          11,         12,           13,          14,           15,  softver_ix,     wrteptr_ix,	//200 -  209
	    mdate_in,     std_int,      min_int,     day_int,    mon_int,     year_int,  softver_ix,   softver_ix,    dev_nr_ix,     dev_nr_ix,	//210 -  219
	    dev_nr_ix,  softver_ix,   softver_ix,     wrteptr_ix                                            					                    //220 -  223
};                                  
