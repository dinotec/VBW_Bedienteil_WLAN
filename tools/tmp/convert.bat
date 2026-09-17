copy "E:\Users\tsybulini\Projekte\workspace_v4\VBW_14_OP10_WLAN\Keil 8051 v9.60.0 - Default\VBW_14_OP10_WLAN.hex" OP10.hex
srec_cat OP10.hex -Intel -o OP10.bin -binary
del OP10.hex
bin_2_dlf_pcd.exe OP10.bin 
del OP10.bin
srec_cat OP10.dlf -binary -o OP10.hex -Intel
REM ~ copy OP10.hex OP10(Flash)_7071_VBW.hex
copy OP10.dlf vbw_update_OP10.dlf
del OP10.dlf
bootloader_cat.cpp.exe OP10.hex Loader_VBW_OP10.hex
