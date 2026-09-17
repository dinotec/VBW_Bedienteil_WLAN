code char (*fptr_rel[])(void);
//~ code char (*fptr_rel4[])(void);
void relais_ini_msr(void);

extern xdata MSR_RL rl[2];
void rl_doszeit(unsigned char i);
void ddos_fn(unsigned char, unsigned char,bit);
unsigned int set_alr_dly(unsigned char in);
						   

//~ extern bit  DOS_ENABLE; 
//~ extern xdata unsigned int pcare,
						  //~ Dos_Zeit_Des,
						  //~ Dos_Zeit_PH_senken,
						  //~ Dos_Zeit_PH_heben;
