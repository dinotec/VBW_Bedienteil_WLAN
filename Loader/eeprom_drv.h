extern xdata unsigned long adr_128k;
extern xdata unsigned long adr_wr;
extern xdata unsigned char *ptr_128k;
extern xdata unsigned int len_128k;

void ee_wrseq_128k(void);
bit ee_rdseq_128k(unsigned char);

