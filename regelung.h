void regelung_ini(void);
void pH_rechnen(void);
char pH_Regelung(void);
void cl_rechnen(void);
char cl_Regelung(void);
char rx_Regelung(void);


//~ bit rel_fun(unsigned char r_funk, char prc);
void t_compensation(void);
//~ void rel_assign(unsigned char n, unsigned int val);
//~ char rx_Regelung(void);

void i_anteil(void);
void des_doszeit(void);
extern bit cl_ph_comp;
extern xdata unsigned char 	doszeit_Des_eepr, btr_art_b0;
extern xdata int ph_mv_corr;							
extern xdata unsigned int	doszeit_Des,	
							doszeit,
							ddos_des,
							v_start_cnt;


