//~ code unsigned char id[] =    "08580OP10 VBW 2014   7515LL"; // 03.11.2016
//~ code unsigned char id[] =    "08580OP10 VBW 2014   7615LL"; // 21.07.2017 - 30 g Anlage Strom zu klein -> i_start_cnt == 150
//~ code unsigned char id[] =    "08580OP10 VBW 2014   7715LL"; // 21.07.2017 - 30 g Anlage Strom zu klein -> i_start_cnt == cnt_5a
//~ code unsigned char id[] =    "08580OP10 VBW 2014   7815LL"; // 24.07.2017 - Durchfluss und Temperaturanzeige ausblenden
//~ code unsigned char id[] =    "08580OP10 VBW 2014   7915LL"; // 24.07.2017 - Durchflussüberwachung, wenn keine Pulse vom Flügelradsensor
//~ code unsigned char id[] =    "08580OP10 VBW 2014   0316LL"; // 14.08.2017 - Produkttank < 5 cm, Alarm für Chlor 0-10 mg/l, P- ph 0 - 10,00,
															// totband pH 0-5,00, Kalibrierung unter 0.1 mg/l, Durchflußüberwachung by stand-by
															// Elektrolyse und MSR mit An-Aus abschaltbar, Display bleibt immer an.
//~ code unsigned char id[] =    "08580OP10 VBW 2014   0416LL"; // 10.10.2017 - Kalibrierung pH war nur mit code 178 möglich 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   0516LL"; // 16.10.2017 - 150 Anlage Durchflusssensor 2223 P/L 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   0616LL"; // 24.10.2017 - Chlor Hysterese 0 - 10000
//~ code unsigned char id[] =    "08580OP10 VBW 2014   1116LL"; // 13.11.2017 - Kalibrierung immer möglich, ausser ASR
//~ code unsigned char id[] =    "08580OP10 VBW 2014   1316LL"; // 15.01.2018 - Sollstrom < 50% nach 2 Minuten Alarm beim Startvorgang (ca. 10 Min)
//~ code unsigned char id[] =    "08580OP10 VBW 2014   1416LL"; // 09.04.2018 - Niveaumessung mit T1-Eingang
//~ code unsigned char id[] =    "08580OP10 VBW 201e4   1516LL"; // 02.05.2018 - Temperaturkompesation wird bei T > 90°C auf manuell umgeschaltet
//~ code unsigned char id[] =    "08580OP10 VBW 2014   1616LL"; // 13.06.2018 - vek[lev_prod_ix].i = vek[prodlev_max].i + 5;
//~ code unsigned char id[] =    "08580OP10 VBW 2014   1916LL"; // 13.06.2018 - NIVEAU_ALR  wird nicht angezeigt, code 221 für Durchflußkorrektur in % 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   2016LL"; // 13.06.2018 - Durchflußkorrektur in % Fehler in Berechnung, soll vek[flow_ely].i statt vek[flow_ely].ui 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   2116LL"; // 13.06.2018 - logische Fehler in der Auswertung code=178 und code=221 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   2216LL"; // 29.08.2018 - Fehler in der Sprachenauswahl 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   2316LL"; // 14.09.2018 - Fehler bei pH-Regelung, nicht initialisierte Variable ki_ph
//~ code unsigned char id[] =    "08580OP10 VBW 2014   2416LL"; // 25.10.2018 - Fehler bei P20 Statusabrage in ident_daten-Funktion (dev_nr_ix),
															// 31.10.2018 - Verzögerng 10 Sek. bei Schwimmschalter-Fehlfunktion 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   3316LL"; // Verdünnungsgebläse
//~ code unsigned char id[] =    "08580OP10 VBW 2014   3416LL"; // Abschaltung der Spannunnsüberwachung
//~ code unsigned char id[] =    "08580OP10 VBW 2014   3616LL"; // Zusätzliche BIN-Eingänge für Schwimmerschalter, Startvorgang für 30gr. Anlage
//~ code unsigned char id[] =    "08580OP10 VBW 2014   3716LL"; // Zusätzliche BIN-Eingänge für Schwimmerschalter, Startvorgang für 30gr. Anlage
//~ code unsigned char id[] =    "08580OP10 VBW 2014   3816LL"; // Luftpumpe abschaltbar bei Niveaumessung mit Schwimmerschaltern
//~ code unsigned char id[] =    "08580OP10 VBW 2014   3916LL"; // Anlagefüllen: Zeit läuft ab Durchfluß > 20 %
//~ code unsigned char id[] =    "08580OP10 VBW 2014   4116LL"; // Fehler im Notbetrieb nicht abschaltbar
//~ code unsigned char id[] =    "08580OP10 VBW 2014   4216LL"; // Startvorgang für alle(!) Anlage
//~ code unsigned char id[] =    "08580OP10 VBW 2014   4316LL"; // Startvorgang für Spannungsüberwachung 10 Min.
//~ code unsigned char id[] =    "08580OP10 VBW 2014   5116LL"; // Startvorgang für Spannungsüberwachung 10 Min.
//~ code unsigned char id[] =    "08580OP10 VBW 2014   5216LL"; // Startvorgang für Spannungsüberwachung 10 Min, immer bei Produktionstart . 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   5616LL"; // Startvorgang für Spannungsüberwachung 10 Min, immer bei Produktionstart, auch Startvorgang . 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   57716LL"; // In Menu - "Spannungsüberwachung Ja, Nein" war nicht richtig abgespeichert. 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   58716LL"; // Soledosierung VBW90: 1,3 l/h statt 1,1 l/h - Puls -> 1180 ms, statt 1000 ms
//~ code unsigned char id[] =    "08580OP10 VBW 2014   5971LL"; // Soledosierung VBW90: 1,3 l/h statt 1,1 l/h - Puls -> 1180 ms, statt 1000 ms
//~ code unsigned char id[] =    "08580OP10 VBW 2014   6071LL"; // Fehler im Startvorgang
//~ code unsigned char id[] =    "08580OP10 VBW 2014   6271LL"; // flow_ely -50 ... 100%, Sollstrom für 150 mg 31,5 A
//~ code unsigned char id[] =    "08580OP10 VBW 2014   6471LL"; // Elektrolyse startet nicht, wenn Display auf "MSR" umgeschaltet, 
															//~ // aber Wasser fließt durch -> kann zum Überlauf kommen, ESQ-Taste keine Funktion bei Elektrolyse
//~ code unsigned char id[] =    "08580OP10 VBW 2014   6571LL"; // Testmode mit Code 567: Produktion - 3 Min, Solenachspeiseüberwachung 3 Min 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   6671LL"; // 150 g - i_soll=29 A, sole 6% mehr 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   6771LL"; // Alarm Anlagebetrieb-Modus korrigiert, "Dosierung abgeschaltet" in Statuszeile bei Wassermangel 
//~ code unsigned char id[] =    "08580OP10 VBW 2014   6871LL"; // Alarm I zu hoch beim Anlauf auf 60 Sek erhöht
//~ code unsigned char id[] =    "08580OP10 VBW 2014   6971LL"; // GW für Modbus mit Code 300 umschaltbar
//~ code unsigned char id[] =    "08580OP10 VBW 2014   7071LL"; // Zusätzliche Status-Register per P20 lesen
//~ code unsigned char id[] =    "08580OP10 VBW 2014   7171LL"; // Verzögerung für Alarm Strom  zu hoch auf 3 Min erhöht.
//~ code unsigned char id[] =    "08580OP10 VBW 2014   7271LL"; // AER für MSR
//~ code unsigned char id[]   =    "08580OP10 VBW 2014   7371LL"; // Pr.Ventil Digitalansteuerung im Display
//~ code unsigned char id[]   =    "08580OP10 VBW 2014   7471LL"; // Test Update
//~ code unsigned char id[]   =    "08580OP10 VBW 2014   2502LL"; // Test Update
code unsigned char id[]   =    "08580OP10 VBW 2014   2503LL"; // Neue Version
code unsigned char id_strg[] = "01369VBW2015 CONTROL 3416LL"; // Steuerungkennung
