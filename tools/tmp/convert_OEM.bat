rem aus absolute OMF Datei wird HEX- Datei Pcd.hex erzeugt
oh51 OP10.o51
rem aus OP10.HEX-Datei  wird OP10.BIN-Datei  erzeugt
hex2bin OP10.hex
rem OP10.hex löschen
del OP10.hex
rem aus bedienteil.bin wird bedienteil.dlf für EEPROM erzeugt
bin_2_dlf_pcd.exe OP10.bin 
rem aus bedienteil.dlf wird bedienteil.hex für Flash erzeugt
bin2hex OP10.dlf OP10.hex
copy OP10.hex OP10(Flash)_OEM.hex
copy OP10.dlf OP10(EEPROM)_OEM.dlf
copy OP10(Flash)_OEM.hex ..\Firmware\OP10(Flash)_OEM.hex
copy OP10(EEPROM)_OEM.dlf ..\Firmware\OP10(EEPROM).dlf
bootloader_cat.cpp.exe OP10(Flash)_OEM.hex Loader_OP10.hex
copy OP10(Flash)_OEM$Loader_OP10.hex ..\Firmware\OP10(Flash)_OEM$Loader_OP10.hex
del OP10.hex
del OP10.dlf
