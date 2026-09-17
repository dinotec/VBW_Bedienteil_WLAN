copy "L:\M51\SRC\workspace\VBW_14_OP10_WLAN\Keil 8051 v9.60.0 - Default\VBW_14_OP10_WLAN.hex" OP10.hex
srec_cat OP10.hex -Intel -o OP10.bin -binary
del OP10.hex
bin_2_dlf_pcd.exe OP10.bin 
del OP10.bin
srec_cat OP10.dlf -binary -o OP10.hex -Intel
REM ~ copy OP10.hex OP10(Flash)_7371_VBW.hex
copy OP10.dlf vbw_update_OP10.dlf
del OP10.dlf
REM ~ bootloader_cat.cpp.exe OP10.hex Loader_OP10_WLAN.hex
bootloader_cat.cpp.exe OP10.hex Loader_vbw_UC_1608.hex
REM ~ bootloader_cat.cpp.exe OP10.hex Loader_OP10.hex
