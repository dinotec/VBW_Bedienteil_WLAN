// Bus_LCD.c
void bus_ini(void);
void read_uart0(void);
void send_com(void);
void get_data(void);
void set_relais(void);
void relais_ini(void);
void set_update(void);
void send_com(void); 
void send1_com();


extern xdata EMPF_BUF sbuf0;
extern bit EMPF0,
		   empf_anf,
		   OK_0,
		   RD;
// extern xdata unsigned char RL_NR;
extern xdata unsigned long bwue_aktl_l;
