set OP10="vbw_update_OP10.dlf"
REM ~ set OP10="OP10(EEPROM)_0004.dlf"
set PRRM="vbw_update_prrm.dlf"
REM ~ eeprom_P.bin ver. 9
set MENU="..\menu\eeprom.bin"
vbw_update.exe %OP10% %PRRM% %MENU%  
REM ~ vbw_update.exe %MENU%  
