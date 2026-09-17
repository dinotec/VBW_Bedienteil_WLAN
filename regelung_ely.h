void cnt_ini(void);
bit ventilator_rl(void);

code bit (*fptr_ely[])(void);
extern xdata unsigned int sec_cnt;
void ely_ini(void);
void set_sec(unsigned char art);

extern xdata unsigned int sec_cnt, sec_cnt2, prc_flow, luft_cnt_vor, luft_cnt_nach, not_zeit_cnt;
extern xdata unsigned char btr_art_b0, vers_anlf;
extern bit 	i_kleiner_5A,
			versuch_1,
			versuch_2,
			start_vgang;


