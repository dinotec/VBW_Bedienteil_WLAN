#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "ConstDef.h"
#include "eeprom_128drv.h"
#include "eeprom128.h"
#include "menu_lcd.h"
#include "main.h"
#include "default_werte.h"
#include "regelung.h"
#include "default_werte_ely.h"

extern code unsigned char id_strg[];
extern code unsigned char id[];


xdata EMPF_BUF1 empf1;
xdata SEND_BUF1 send1;
extern bit EXTSTOP_BIT, E_ANLALR, ENTHAERT;
 
bit EMPF_1,
	EMPF_P20, 
	ip_ini, 
	eepr_save, 
	wifi_update,
	modbus_enabled,
	mod_bus,
	alr_reset,
	send_sts;

//~ xdata unsigned int ini_cnt;
static idata unsigned char reset_cnt;
idata unsigned char changed, 
					changed2,
					changed3;
code unsigned char *finish = "FINISH";
code char * bl_rd = "BL_RD %lu\r";

xdata FLOAT ip_adr;
//~ code unsigned char wr_reg[] = {cl_alr_ob, cl_wr_ob,cl_soll,cl_wr_un,cl_alr_un,cl_p,cl_i,
							   //~ ph_alr_ob,ph_wr_ob,ph_soll,ph_wr_un,ph_alr_un,ph_p,ph_zi,
							   //~ rx_soll,rx_wr_un,rx_alr_un,rx_p};

code unsigned char ely_reg[] = {btr_art_t, lev_art_t,   flow0_ix, t0_ix, el_V_mv_ix, el_A_mv_ix, 
								lev_prod_ix,  cur_ix, prv_ch0_ix, prv_ch1_ix, stat_1, stat_ely};

code unsigned char ely_reg_len = sizeof(ely_reg);

code unsigned char ini_reg[] = {dev_nr_ix, softver_ix, soft_strg_ix, anl_typ_t};
code unsigned char ini_reg_len = sizeof(ini_reg);

unsigned int reg_fn(unsigned char c) {
	switch(c) {
		case el_A_mv_ix:
			return prc_fn(vek[el_A_mv_ix].ui, werte_el[i_soll][vek[anl_typ_t].b[1] - a30_txt]);
			break;
		case flow0_ix:
			return prc_fn(vek[flow0_soll_ix].ui, vek[flow0_ix].ui);
			break;
		case stat_1:
			return STATUS1.ui;
			break;
		case stat_ely:
			return STATUS_ELY.ui;
			break;
		case btr_art_t: {
			//~ if(EXTSTOP_BIT)
				//~ return ext_stop_txt;
			//~ else if(E_ANLALR)
				//~ return h2_alr_txt;
			//~ else if(ENTHAERT)
				//~ return enth_txt;
			//~ else		
				return btr_art_b0;
			break;		
		}
		default:
			return vek[c].ui;
			break;
	}
}


void test_update_eepr(void);

void modbus_cntr(void) {
    modbus_enabled = (vek[komm_t].ui == modbus_txt);
	(modbus_enabled) ? (P4 &= ~0x04 ) : (P4 |= 0x04); // GW_ON = 1 -> WiFi on;
}

void com1_ini(void) {
	TREN1  = 0;
	//~ GW_ON;
	//~ GW_OFF;
	EMPF_1      = 0;
	ip_ini      = 0;
    //~ ini_cnt     = 0;
    reset_cnt   = 0;
    eepr_save   = 0;
    wifi_update = 0;
    modbus_cntr();
    //~ changed  = code0_x;
    //~ changed2 = code0_x;
    //~ changed3 = code0_x;
	memset(&empf1,0,sizeof(empf1));
	memset(&send1,0,sizeof(send1));
	alr_reset = 0;
}

void send_byte(unsigned char b) {
	TREN1 = 1;
	while(!(SCON1 & 0x20)) ;
	SBUF1 = b;
}

void UART1_ISR (void) interrupt 16 {
	if(SCON1 & 0x01) {
		SCON1 &= ~0x01;
		if(SCON1 & 0x80) {	// Receive FIFO Overrun Flag
			memset(&empf1,0,sizeof(empf1));
			SCON1 &= ~0x80;
			empf1.cnt = 0;
			return;
		}
		empf1.buf[empf1.cnt] = SBUF1;
		++empf1.cnt;
		if(empf1.cnt == strlen(finish)) {
			if(!strncmp(finish, empf1.buf, strlen(finish))) {
				EMPF_1 = 1;
				eepr_save = 0;
			}
		}
		
		if(eepr_save) {
			if(empf1.cnt == BLOCK_LEN) 
				EMPF_1 = 1;
		} else {		
			//~ if(empf1.buf[empf1.cnt] == 0x0d && !modbus_enabled) {
			if(empf1.buf[empf1.cnt-1] == 0x0d) {
				if(!modbus_enabled) 
					EMPF_1 = 1;
			} 
			//~ else
				//~ ++empf1.cnt;
		}
	} else {
		SCON1 &= ~0x02;
		if(modbus_enabled){
			if(++send1.cnt < empf1.cnt) {
				WDT_RT();
				goto SEND_2;
				//~ while(!(SCON1 & 0x20)) ;
				//~ SBUF1 = send1.buf[send1.cnt];
			} else {	
				TREN1 = 0;
				memset(&send1,0,sizeof(send1));
				memset(&empf1,0,sizeof(empf1));
				mod_bus = 0;
			}
		} else {
			if(send1.buf[++send1.cnt] > 0) {	
				SEND_2:
				while(!(SCON1 & 0x20)) ;
				SBUF1 = send1.buf[send1.cnt];
			} else {	
				TREN1 = 0;
				memset(&send1,0,sizeof(send1));
			}
		}
	}
}	

void wifiReset(void) {
    wifi_reset = 0;
    strncpy(send1.buf,"WIFI_RST",8);
    ip_adr.b[0] = ip_adr.b[1] = ip_adr.b[2] = ip_adr.b[3] = 0;
    //~ ini_cnt = 0;
    send1.buf[strlen(send1.buf)] = 0x0d;
    send_byte(send1.buf[0]);
}

void send_sos(void) {
	memset(&empf1,0,sizeof(empf1));
	sprintf(send1.buf, bl_rd, adr_128k);
	send_byte(send1.buf[0]);
}

//~ 11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
//---------------

bit wlan_cmd() {
    xdata unsigned char i;
	bit ret;
	ret = 0;
	memset(&send1,0,sizeof(send1));

	if(!strncmp(finish, empf1.buf, strlen(finish))) {
		eepr_save   = 0;
		wifi_update = 0;
		ret = 1;
		memset(&empf1,0,sizeof(empf1));
	} else if(eepr_save) {
		EIE2 = 0;
		eepr_save = ee_wrseq_128k(1); // cnt=0,128,256....
		EIE2 = 0x02;
		if(eepr_save) {
			sprintf(send1.buf, bl_rd, adr_128k);
		} else
			sprintf(send1.buf,"NO EEPROM\r");
		memset(&empf1,0,sizeof(empf1));
		send_byte(send1.buf[0]);
	} 
	else if(!strncmp("BLOCK ", empf1.buf, 6)) {
		adr_128k = 0;//atoi(empf1.buf+6);
		ptr_128k = empf1.buf;
		len_128k = BLOCK_LEN;
		strncpy(send1.buf,"BL_RD 0\r", 8);
		eepr_save = 1;
		send_byte(send1.buf[0]);
		wifi_update = 1;
		ES0 = 0;
	} else if(!strncmp("END", empf1.buf, 3)) {
		memset(&empf1,0,sizeof(empf1));
		wifi_update = 0;
		eepr_save   = 0;
		ES0 = 1;
		ret = 1;
	} else if(!strncmp("ALR_RST", empf1.buf, 7)) {
		memset(&empf1,0,sizeof(empf1));
		alr_reset = 1;
		menu_ini();
		key_press(0);
	} else if(!strncmp("LADR", empf1.buf, 4)) {
		if(rls) {
			//~ xdata UINT menu_ver;
			xdata unsigned int strg_ver;
			//~ adr_128k = MENU_ATTR_ADR+1;
			//~ menu_ver.b[0] = ee_rdb_128k();
			//~ menu_ver.b[1] = ee_rdb_128k();
			(soft_strg_bit) ? (strg_ver = 9999) : (strg_ver = vek[soft_strg_ix].ui);
			sprintf(send1.buf,"LA: %u,%u,%u,%u,%u\r", vek[dev_nr_ix].ui, menu_ver.ui, vek[softver_ix].ui, strg_ver, vek[anl_typ_t].ui);
			soft_strg_bit = 0;
			send_byte(send1.buf[0]);
		}
	} else if(!strncmp("IP:", empf1.buf, 3)) {
		xdata char pos, * str;
		if(wifi_reset)
			goto WIFI_RESET;


		str = (char*)(empf1.buf + 3);
		for(i = 0; i < 4; ++i) {
			ip_adr.b[i] = atoi(str);
			pos = strpos(str, '.');
			if(pos != -1)
				str += (pos+1);
		}
		if(!ip_adr.b[0])
			ip_ini = 0;
		else
			ip_ini = 1;

		str = (char*)(empf1.buf + 4);
		pos = strpos(str, ' '); 
		i = strpos(str, 0x0d);
		str[i] = 0;
		i = strlen(str + pos + 1); 
		#define SSID_LEN 14
		if(i > SSID_LEN)
			i = SSID_LEN;
		memset(ssid, 0, SSID_LEN+1);
		strncpy(ssid, str + pos + 1, i);
		i = 0;		
	} else if(!strncmp("MESS", empf1.buf, 4)) {
        if(wifi_reset) {
			WIFI_RESET:
            strncpy(send1.buf,"WIFI_RST",8);
            wifi_reset = 0;
            ip_adr.b[0] = ip_adr.b[1] = ip_adr.b[2] = ip_adr.b[3] = 0; 
            //~ ini_cnt = 0;
            ip_ini  = 0;
        } 
        else {
			xdata unsigned char y;
			wlan_sig = atoi(empf1.buf + 5);
			i = strpos(empf1.buf + 5, ' ');
			connected = atoi(empf1.buf + 6 + i);
            y = strpos(empf1.buf + 6 + i, ' ');
			serv_cnt = atoi(empf1.buf + 6 + i + y);
            y = strncmp(ssid, "VBW", 3);
            if((connected && !y) || (!connected && y))
				ip_ini = 0;
            strncpy(send1.buf, "MD ",3);
            for(i = 0; i < ely_reg_len; ++i) {
                sprintf(send1.buf + strlen(send1.buf),"%d,", reg_fn(ely_reg[i]));
            }
            //~ if(des_man)
				//~ stat3_app |= DES_DOS_MAN;
            //~ if(ph_man)
				//~ stat3_app |= PH_DOS_MAN;
            //~ if(fp_man)
				//~ stat3_app |= FPUMP_MAN;
            //~ if(des_man_on)
				//~ stat4_app |= DESMANON;
            //~ if(ph_man_on)
				//~ stat4_app |= PHMANON;
				
            //~ sprintf(send1.buf + strlen(send1.buf),"%d,%d,%u,%u,%d,", t0_ix, t1_ix, stat2_app, stat3_app, rx_mv);
            //~ sprintf(send1.buf + strlen(send1.buf),"%d,\r", stat4_app);
        }
        send_byte(send1.buf[0]);
	} else if(!strncmp("INIT", empf1.buf, 4)) {
        if(wifi_reset) {
            wifiReset();
        }
        else {
			//~ xdata int v_min, v_max;
            xdata unsigned char n;
            //~ ini_cnt = 0;// atoi(empf1.buf + 5);
            n = 0;
            #define MAX_CHAR_CNT 100
                strncpy(send1.buf,"INI ",4);
                while(strlen(send1.buf) < MAX_CHAR_CNT && n < ini_reg_len) {
                    //~ if(ini_cnt < code0_x)
						//~ v_min = tab1[ini_cnt].min, v_max = tab1[ini_cnt].max;
                    //~ else
						//~ v_min = 0, v_max = 0; // nur Lesewerte
					sprintf(send1.buf + strlen(send1.buf),"%d,", vek[ini_reg[n]].i);
                    ++n;
                }
                i = strlen(send1.buf);
                send1.buf[i - 1] = 0x0d; // letzte Komma überschreiben
                send_byte(send1.buf[0]);
        }    
    } else if(!strncmp("Update ", empf1.buf,7)) {
        xdata unsigned int val, reg;
        xdata unsigned char anz;
        anz = sscanf(empf1.buf+7, "%u %d", &reg, &val);
        if(anz==2) {
			if(reg < code0_x ) {
				//Grenzen testen
				var_wert = val;
				Wert_grenze(reg);
				vek[reg].ui = var_wert;
				speichern_int(reg);
				vr.nr[menu_stack.pos[0]] = reg;
				key_press(1);
				changed = reg;
			}  
					
        }
    } 
	memset(&empf1,0,sizeof(empf1));
	EMPF_1   = 0;
    return ret;
}

void test_update_eepr() {
	EIE2 = 0;
	adr_128k = 0;
	ptr_128k = empf1.buf;
	len_128k = BLOCK_LEN;
	ee_rdseq_128k(1);
	if(!strncmp(empf1.buf + 0x40, id_strg, 21)) { // Update für Steuerung
		len_strg   = empf1.buf[25 + 0x40];
		len_strg <<= 8;
		len_strg  += empf1.buf[26 + 0x40];
		len_strg  += 2;
		ptr_128k   = send1.buf;
		upd_cnt    = 1;
		ES0        = 1;
	} else { 
		RSTSRC |= 0x10;		
	} 
}


#if 0
        else if(changed != code0_x) {
            sprintf(send1.buf,"CHANGE %bu,%d\r", changed, vek[changed].i);
			changed = code0_x;
        } else if(changed2 != code0_x) {
            sprintf(send1.buf,"CHANGE %bu,%d\r", changed2, vek[changed2].i);
			changed2 = code0_x;
        } 
        else if(changed3 != code0_x) {
			changed3 = 0;
			if(vek[min_int].ui == 0) {
				++changed3;
				if(vek[std_int].ui == 0) {
					++changed3;
					if(vek[day_int].ui == 1) {
						++changed3;
						if(vek[mon_int].ui == 1) {
							++changed3;
						}
					}
				}
			}		
			switch(changed3) {
				case 1:
					sprintf(send1.buf,"CHANGE %bu,%d,%d\r", std_int, vek[std_int].i, vek[min_int].i);
					break;
				case 2:
					sprintf(send1.buf,"CHANGE %bu,%d,%d,%d\r", std_int, vek[std_int].i, vek[min_int].i, vek[day_int].i);
					break;	
				case 3:
					sprintf(send1.buf,"CHANGE %bu,%d,%d,%d,%d\r", std_int, vek[std_int].i, vek[min_int].i, vek[day_int].i, vek[mon_int].i);
					break;	
				case 4:
					sprintf(send1.buf,"CHANGE %bu,%d,%d,%d,%d,%d\r", std_int, vek[std_int].i, vek[min_int].i, vek[day_int].i, vek[mon_int].i, vek[year_int].i);
					break;	
				default:
					sprintf(send1.buf,"CHANGE %bu,%d\r", min_int, vek[min_int].i);
					break;	
			}
			changed3 = code0_x;
        } 
#endif
#if 0		
			else if(reg == ph_dpd2_x) { // pH-Kalibrierung
				vr.nr[menu_stack.pos[0]] = reg;
				key_press(1);
			} else if(reg == RESET_REG) { // Reset-Register
				if(val & DOS_WATCH_RST) {
					dos_zeit_alarm_reset();
				}
				if(val & DES_CONTROL_MAN) {
					des_man    = 1;
					des_man_on = 0;
					goto DESONOFF;
				}
				if(val & DES_CNTRL_MAN_AN) {
					des_man_on = 1 & des_man;
					//~ ddos_des = vek[des_man_dos_int].ui * 60 * (int)des_man_on;
					goto DESONOFF;
				}
				if(val & DES_CNTRL_MAN_AUS) {
					des_man_on = 0;
					goto DESONOFF;
				}
				if(val & DES_CONTROL_AUT) {
					des_man    = 0;
					des_man_on = 0;
					DESONOFF:
					ddos_des = vek[des_man_dos_int].ui * 60 * (int)des_man_on;
				}
				if(val & PH_CONTROL_MAN) {
					ph_man    = 1;
					ph_man_on = 0;
					goto PHONOFF;
				}
				if(val & PH_CNTRL_MAN_AN) {
					ph_man_on = 1 & ph_man;
					goto PHONOFF;
				}
				if(val & PH_CNTRL_MAN_AUS) {
					ph_man_on = 0;
					goto PHONOFF;
				}
				if(val & PH_CONTROL_AUT) { 
					ph_man    = 0;
					ph_man_on = 0;
					PHONOFF:
					ddos_ph = vek[ph_man_dos_int].ui * 60 *(int)ph_man_on;
					
				}
				if(val & ON_DLY_RST) { // Einschaltverzögerung
					INIT_DELAY = 0;
					on_delay   = 0;
				}
			} 
#endif				
