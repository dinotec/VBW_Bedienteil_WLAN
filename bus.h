void com1_ini(void);
void send_status(void);
void send_byte(unsigned char b);
bit wlan_cmd(void);
void test_update_eepr(void);
unsigned char offst(void);
void modbus_cntr(void);
void send_sos(void);
unsigned int reg_fn(unsigned char c);


extern bit EMPF_1, 
		   ip_ini, 
		   fzt_changed,
		   mod_bus, 
		   modbus_enabled, 
		   wifi_update,
		   alr_reset,
		   send_sts;

extern xdata EMPF_BUF1 empf1;
extern xdata SEND_BUF1 send1;
//~ extern xdata unsigned int ini_cnt;
extern xdata unsigned char changed, 
						   changed2, 
						   changed3;
						   
extern code unsigned char *finish;

//~ extern code unsigned char wr_reg[];
extern code unsigned char ely_reg[];
extern code unsigned char ely_reg_len;
extern xdata FLOAT ip_adr;
