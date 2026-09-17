#define RESET_REG 145

//status reg. 145 nur Schreibregister, um bestimmte Betriebsarten ein-, oder auszuschalten 
#define DOS_WATCH_RST     0x0001	// "Dosierzeit Desinfektion überschritt.", um zurücksetzen, von App auf 1 setzen	
#define FPUMP_CONTROL_MAN 0x0002	// Filterpumpe auf manuell schalten
#define FPUMP_CONTROL_AUT 0x0004	// Filterpumpe auf automatik schalten
#define FPUMP_MAN_EIN     0x0008	// wenn Filterpumpe manuell: 1 - einschalten
#define FPUMP_MAN_AUS     0x0010	// wenn Filterpumpe manuell: 1 - ausschalten
#define DES_CONTROL_MAN   0x0020	// 1 auf manuell umschalten
#define DES_CONTROL_AUT   0x0040	// 1 auf automatik umschalten
#define DES_CNTRL_MAN_AN  0x0080	// 1 manuell einschalten
#define DES_CNTRL_MAN_AUS 0x0100	// 1 manuell ausschalten
#define PH_CONTROL_MAN    0x0200	// 1 auf manuell umschalten
#define PH_CONTROL_AUT    0x0400	// 1 auf automatik umschalten
#define PH_CNTRL_MAN_AN   0x0800	// 1 manuell einschalten
#define PH_CNTRL_MAN_AUS  0x1000	// 1 manuell ausschalten
#define ON_DLY_RST        0x2000	// "Einschaltverzögerung aktiv", um abzubrechen, von App auf 1 setzen

// status2 reg. 142
#define ALARM_OB_DES   0x0001	// "Alarm Desinfektion oben",
#define WARN_OB_DES    0x0002	// "Warnung Desinfektion oben",
#define WARN_UN_DES    0x0004	// "Warnung Desinfektion unten",
#define ALARM_UN_DES   0x0008	// "Alarm Desinfektion unten",
#define WARN_OB_PH     0x0010	// "Warnung pH oben",
#define WARN_UN_PH     0x0020	// "Warnung pH unten",
#define ALARM_OB_PH    0x0040	// "Alarm pH oben",
#define ALARM_UN_PH    0x0080	// "Alarm pH unten",
#define WASS_MANGL     0x0100	// "Messwassermangel",	
#define EXTERN_STOP    0x0200	// "Externer Dosierstopp",
#define DES_LEER       0x0400	// "Desinfektion Kanister ist leer",
#define DES_DOS_WATCH  0x0800	// "Dosierzeit Desinfektion überschritt.",	
#define PHUPDOSWATCH   0x1000	// "Dosierzeit pH-heben überschritten",
#define PHDOWNDOSWATCH 0x2000	// "Dosierzeit pH-senken überschritten",
#define ON_DLY         0x4000	// "Einschaltverzögerung aktiv",
#define NO_CONNECT     0x8000	// "Keine Verbindung zu Unterteil",

//status3 reg. 143
#define STH_ERR     0x0001	// "Steilheitsfehler",
#define NULLERR     0x0002	// "Nullpunktfehler",
#define MAN_DOS_DES 0x0004	// "Dauerdosierung Desinfektion läuft",
#define MAN_DOS_PH  0x0008	// "Dauerdosierung pH läuft",
#define PH_LIM_UP   0x0010	// "Regelgrenze pH überschritten",	
#define PH_LIM_DW   0x0020	// "Regelgrenze pH unterschritten",
#define WARN_OB_T   0x0040	// "Warnung Wassertemperatur oben",
#define WARN_UN_T   0x0080	// "Warnung Wassertemperatur unten",
#define ALR_OB_T    0x0100  // "Alarm Wassertemperatur oben",
#define ALR_UN_T    0x0200	// "Alarm Wassertemperatur unten",
#define PH_LEER     0x0400	// "pH Kanister ist leer",
#define DES_DOS_MAN 0x0800	// Desinfektion-Dosierung manuell,
#define PH_DOS_MAN  0x1000	// pH-Dosierung manuell,
#define FPUMP_MAN   0x2000	// wenn Filterpumpe manuell geschaltet,
#define FPUMP_RUN   0x4000	// wenn Filterpumpe läuft: 0 - aus, 1 - ein, nur Anzeige,
#define HEIZUNG_AN  0x8000	// wenn 1, Heizungpumpe läuft, nur Anzeige,

//status4 reg 145
#define DESMANON 0x0001  // 1 wenn eingeschaltet
#define PHMANON  0x0002  // 1 wenn eingeschaltet

