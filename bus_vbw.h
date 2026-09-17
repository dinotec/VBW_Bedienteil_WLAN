void com1_ini(void);
void send_status(void);
void send_byte(unsigned char b);
//~ unsigned char offst(void);
unsigned int reg_fn(unsigned char c);


extern xdata unsigned char bus_free, 
						   ret_adr;
extern bit send_sts,
           mod_bus;
extern xdata EMPF_BUF1 mbbuf, 
                       sbuf1;
