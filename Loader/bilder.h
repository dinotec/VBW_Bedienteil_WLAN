enum{boot_txt, flash_txt, false_txt, menu_txt, no_txt, ok_txt, program_txt, start_txt, update_txt, reset_txt};

// extern code unsigned char rst[];
// extern code unsigned char updt[];
// extern code unsigned char start[];
// extern code unsigned char program[];
// extern code unsigned char ok[];
// extern code unsigned char no[];
// extern code unsigned char menu[];
// extern code unsigned char false[];
// extern code unsigned char flsh[];
// extern code unsigned char boot[];
extern code unsigned char *bmp_vec[];// = {boot, flsh, false, menu, no, ok, program, start, updt, rst};
extern code unsigned char bmp_len[];//  = {sizeof(boot), sizeof(flsh), sizeof(false), sizeof(menu), sizeof(no), sizeof(ok), sizeof(program),
