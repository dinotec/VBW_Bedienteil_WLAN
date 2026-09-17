void menu_ini(void);
void key_press(bit);
void menu_rt(void);
void tank_level(void);
void tanksM(void);
unsigned char tankMarker(unsigned char l, unsigned char overflow);
void balken(unsigned char pos, unsigned char prc);
unsigned int prc_fn(unsigned int ist, unsigned int soll);
void Wert_grenze(unsigned char pos);
void show_update(void);
reset_fn(void);

extern bit plus, 
		   minus,
		   KEY_PR,
    	   uin_save_flg,
		   uin_set_flg,
		   dpd_bit,
		   wifi_reset,
		   connected;

extern xdata unsigned char ph_rel,
						   mot_pos,
						   buf_tmp[],
						   txt_cnt_max,
						   txt_dflt[ZEILEN][ZEICHEN],
						   anz_spr,
						   MENU_MODE,
						   KEY,
						   key_del,
						   status_cnt,
						   ssid[],
						   serv_cnt;
extern char wlan_sig;
//~ extern idata  unsigned char MENU_MODE;
				
extern xdata var_act vr;
extern xdata menustack menu_stack;	


extern xdata int menu_vec[];
extern char uin_cnt;
extern xdata int var_wert;

extern xdata unsigned long	
							//menu_ptr,
							menu_anf;
							//~ menu_attr;
							
extern xdata unsigned int	offset,
							r0art,
							r1art, 
							r1funk;
extern xdata float flw_cmp;
extern xdata RRAM rram;
extern xdata UINT menu_ver;
