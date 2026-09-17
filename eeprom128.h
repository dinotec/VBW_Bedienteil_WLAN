void SteilheitPH(void);
//~ void SteilheitFl(bit);

void pczg_pumpenzeit(void);
void ee_ini(void);
void Set_default(void);
void speichern_int(unsigned int u);
void ohne_ph(void);
//~ void dev_num(void);
void des_alarm_fn(void);
bit test_var(unsigned char var_num);
void modbus_GW(void);

extern xdata unsigned long adr_128k;
extern xdata unsigned char *ptr_128k, asr2, cl_sth;
extern xdata unsigned int len_128k;
extern xdata FLOAT EEPPROM_ADR_SZ;
extern xdata UINT vek[];
//~ extern xdata LOC_VAR lvar;

//~ extern xdata int phf0[2];
//~ extern xdata float	fl_0[2],
			        //~ fl_sth[2];
