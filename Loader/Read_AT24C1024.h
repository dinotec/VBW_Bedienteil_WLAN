extern xdata RW_BUF buf;
extern xdata unsigned int crc;
extern xdata ULONG	len;
extern bit eepr_64k;
void Calc_CRC(unsigned char a);
void message(unsigned char txt, unsigned char upd);

