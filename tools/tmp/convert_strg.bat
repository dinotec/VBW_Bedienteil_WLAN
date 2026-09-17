copy "E:\Users\tsybulini\Projekte\workspace_v4\vbw_prrm0116\Keil 8051 v9.60.0 - Default\vbw_prrm0116.hex" vbw_prrm.hex
srec_cat vbw_prrm.hex -Intel -o vbw_prrm.bin -binary
del vbw_prrm.hex
bin_2_dlf_pcd.exe vbw_prrm.bin 
del vbw_prrm.bin
srec_cat vbw_prrm.dlf -binary -o vbw_prrm.hex -Intel
REM ~ copy OP10.hex OP10(Flash)_7071_VBW.hex
copy vbw_prrm.dlf vbw_update_prrm.dlf
del vbw_prrm.dlf
bootloader_cat.cpp.exe vbw_prrm.hex LOADER_PRRM.hex
