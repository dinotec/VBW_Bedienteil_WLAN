void WDT_RT(void);

extern bit 	INIT_DELAY,
			ON_OFF,  		   
			DES_AUS,
			PH_AUS,
			ASR,
			DOSZEIT_RL3,
			DOSZEIT_RL4,
			ASR_MAN,     		   
			POT_STH_ERR,
			COLLECT_ALR,
			I_ALR,      		   
			FLOW_ALR,   		   
			FLOW_WRN,
			SOLE_ALR,
			NIVEAU_ALR,   		   
			MB_DOS_STOP,    // Bit 0  = Abschaltung der Dosierung ( Reglerstop)
			MB_EXT_FREE,    // Bit 1  = externe Freigabe
			MB_DES_LEER,    // Bit 2  = Leermeldung Desinfektionsmittel
			MB_MWM,         // Bit 4  = Messwassermangel
			MB_WRN_AER,     // Bit 5  = Warnung Funktionstest nach AER
			MB_WRN_PLSCNT,  // Bit 6  = Warnung Plausibilitätskontrolle 
			MB_AER_ACTIV,   // Bit 7  = ASR aktiv Messwert eingefroren
			MB_EIN_VRZGRN,  // Bit 8  = Einschaltverzögerungszeit läuft 
			MB_ALR_DES_OB,  // Bit 9  = Desinfektion Alarmwert oben
			MB_WRN_DES_OB,  // Bit 10 = Desinfektion Warnwert oben
			MB_WRN_DES_UN,  // Bit 11 = Desinfektion Warnwert unten
			MB_ALR_DES_UN,  // Bit 12 = Desinfektion Alarmwert unten
			MB_AER_START,
			PH_STH_ERR,   
			PH_NULL_ERR,   
			ANL_ALR,   
			OVFL_ALR,
			TANK_VOLL,   
			no_verb,
			displ_rst,
			rls,
			T_ALR,
			T_WRNH,
			T_WRNL,
			I_ALRH,
			I_ALRL,
			FLOW_ALRH,
			FLOW_ALRL,
			U_ALRH,
			U_ALRL,
			U_WRNH,
			TANK_NOT,
			BUS_RST,
			mwm,
			dos_aus,
			soft_strg_bit;

extern bdata UINT STATUS1,
				  STATUS_ELY,
				  MB_STATUS;

//~ extern bdata char REL;				  
extern xdata unsigned int min,
						  on_delay,
						  asr_10min,
						  asr30min,
						  des_alr_delay,
						  ph_alr_delay,
						  len_strg;
					  
extern xdata unsigned char 	dly_btr, 
							swimm_cnt, 
							dly_H2, 
							upd_cnt;
							
extern xdata STATUSZEILE status_zeile;			  
void add_warn(unsigned int num);
void add_alarm(unsigned int num,bit s);
void led_rt(unsigned char state);
