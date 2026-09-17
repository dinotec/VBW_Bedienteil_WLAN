copy "L:\M51\SRC\workspace\vbw_prrm0116\Keil 8051 v9.53 - Default\vbw_prrm0116.hex" vbw_st16.hex

rem aus absolute OMF Datei wird HEX- Datei PRRM.hex erzeugt
rem oh51 vbw_st16.o51
rem aus vbw_st16.HEX-Datei  wird vbw_st16.BIN-Datei  erzeugt
::~ hex2bin vbw_st16.hex
srec_cat vbw_st16.hex -Intel -o vbw_st16.bin -binary
rem wi_prrm.hex löschen
del vbw_st16.hex
rem aus wi_prrm.bin wird wi_prrm.dlf für EEPROM erzeugt
bin_2_dlf_pcd.exe vbw_st16.bin 
rem aus vbw_st16.dlf wird vbw_st16.hex für Flash erzeugt
::~ bin2hex vbw_st16.dlf vbw_st16.hex
srec_cat vbw_st16.dlf -binary -o vbw_st16.hex -Intel
copy vbw_st16.hex vbw_strg(Flash)_3916.hex
copy vbw_st16.dlf vbw_strg(EEPROM)_3916.dlf
copy vbw_st16.dlf vbw_update_strg.dlf
del vbw_st16.hex
del vbw_st16.dlf
bootloader_cat.cpp.exe vbw_strg(Flash)_3916.hex LOADER_PRRM.HEX
::~ copy ..\Firmware\vbw_st16(Flash).hex ..\Firmware\vbw_st16(Flash)_alt.hex
::~ copy ..\Firmware\vbw_st16(EEPROM).dlf ..\Firmware\vbw_st16(EEPROM)_alt.dlf
::~ copy vbw_st16(Flash).hex ..\Firmware\vbw_st16(Flash).hex
::~ copy vbw_st16(EEPROM).dlf ..\Firmware\vbw_st16(EEPROM).dlf
::~ copy ..\Firmware\vbw_st16(Flash)$LOADER_PRRM.hex ..\Firmware\vbw_st16(Flash)$LOADER_PRRM_alt.hex
::~ copy vbw_st16(Flash)$LOADER_PRRM.hex ..\Firmware\vbw_st16(Flash)$LOADER_PRRM.hex
