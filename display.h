void display_wr(unsigned char *ptr);
void display_flush(bit num, bit pos, bit invt); // 1 oder 2 Display normal oder umdrehen Bild invertieren
void display_flush_pcd5(bit invt); // 1 oder 2 Display normal oder umdrehen Bild invertieren
void bilder2(void);
void bilder3(void);
void display_reset(void);
void bmp_clear(void);
void wasser_bild(void);
void hand_bild(void);
void line_h(unsigned char, unsigned char, unsigned char *);
void line_v(unsigned char, unsigned char, unsigned char *);
 void lcdInit(void);
 
extern xdata RRAM rram;


