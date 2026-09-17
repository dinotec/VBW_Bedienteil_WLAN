#include <stdio.h>
#include <algorithm>
#include <vector>
#include <set>
#include <list>
#include <iostream>
#include <cstring>
#include <fstream>
#include <iterator>
#include <iomanip>
#include "pc_dynamic_3.h"
#include "Sprachen.h"

using namespace std;

#define ZEICHEN 60
#define ZEILEN 20
#define STACK_LAENGE 20

#define KENNUNG 'P'
//~ #define KENNUNG '~'
const int version = 1; 


vector<unsigned char> vc;
typedef struct menustack
{
	unsigned int  menu_nr[STACK_LAENGE];
	unsigned char  txt_nr[STACK_LAENGE];
	unsigned char     pos[STACK_LAENGE];
	unsigned char size;
} menustack;
menustack menu_stack;	


int adresse(int adr,int pos)
{
	int x;
	x = vc[adr];
	x <<= 8;
	x |= vc[adr+1];
	if(pos)
	{	
		x <<= 8;
		x |= vc[adr + 2];
	}
	return x;
}

unsigned char txt_dflt[ZEILEN][ZEICHEN];
int aktSprache,
	anz_spr,
	anz_txt,
	offset,
	menu_ptr,
	menu_anf;
	//text_nr,
	//txt_cnt_max;
	//var[8];


void zeile_fuellen(int zeile, int text_nr)
{
	int adr0,cnt;
	
	adr0 = adresse(15 + ((text_nr - 1) * 3) + offset*aktSprache, 1);
	// cout << "adr0="<<adr0<<endl; 
	memset(txt_dflt[zeile],0,sizeof(txt_dflt[0]));
	for(cnt = 0; vc[adr0] > 0; ++adr0, ++cnt)
	{	
		txt_dflt[zeile][cnt] = vc[adr0];
	}
	cout << "Zeile "<< zeile << ": " << txt_dflt[zeile] << " ,attr: " << static_cast<int>(menu_attr[text_nr-1]) << endl;
}

	
void menu_fuellen3(void) // 
{
	unsigned int i, 
		mn,
		row,
		//~ cnt,
		text_nr;
	
	for(i = 0; i < 8; ++i)
		memset(txt_dflt[i],0,sizeof(txt_dflt[0]));
	
	// richtige Menu finden
	for(i = 0, mn = 0; mn < menu_stack.menu_nr[0]; i += 2) 
	{
		if(vc[menu_anf + i]==0 && vc[menu_anf + i + 1]==0)
			++mn;
	}
	// cout << "i="<<i<<", menu_stack.menu_nr[0]="<<static_cast<int>(menu_stack.menu_nr[0])<<endl;
	menu_ptr = menu_anf + i;
	// 1. Zeile
	text_nr = adresse(menu_ptr, 0);
	cout << "text_nr=" << text_nr << endl;
	zeile_fuellen(0,text_nr);
	menu_ptr += (menu_stack.txt_nr[0] - menu_stack.pos[0] + 1) * 2; 
	
	// row = 1;
	// text_nr = adresse(menu_ptr, 0);
	// while(text_nr && row < 8)
	// {
		// zeile_fuellen(row);
		// ++row;
		// menu_ptr += 2;
		// text_nr = adresse(menu_ptr, 0);
	// }
	
	text_nr = adresse(menu_ptr, 0);
	cout << "text_nr=" << text_nr << endl;
	for(row = 1; text_nr && row < 8; ++row)
	{
		text_nr = adresse(menu_ptr, 0);
		cout << "text_nr=" << text_nr << endl;
		if(1)// !menu_attr[text_nr-1])
			zeile_fuellen(row,text_nr);
		else 
			--row;
		menu_ptr += 2;
	}
	menu_ptr = menu_anf + i + menu_stack.txt_nr[0]*2;
	text_nr = adresse(menu_ptr, 0);
	// if(menu_stack.cursor[0] == 0) menu_stack.cursor[0] = row;
	// cout << "Anfang menu_ptr=" << menu_ptr << endl;
	// cout << "text_nr=" << text_nr << endl;
	// return row;
}	

void cursor_nach_unten(void)
{	
	int text_nr;
	cout << "cursor_nach_unten()" << endl;
	
	do
	{	
		menu_ptr += 2;
		text_nr = adresse(menu_ptr, 0);
	} while(menu_attr[text_nr - 1] && text_nr);
	
	if(!text_nr)
	{	
		menu_ptr -= 2;
		return;
	}
	
	++menu_stack.pos[0];
	++menu_stack.txt_nr[0];
	if(text_nr)
	{
		if(menu_stack.pos[0] > 7)
		{	
			cout << "text_nr=" << text_nr << endl;
			// ++menu_stack.txt_nr[0];
			for(int i = 1; i < 8; i++)
			{	
				memset(txt_dflt[i],0,sizeof(txt_dflt[i]));
				copy(txt_dflt[i+1],txt_dflt[i+1] + sizeof(txt_dflt[i]),txt_dflt[i]);
			}
			menu_stack.pos[0] = 7;
			zeile_fuellen(7,text_nr);
		}
	}
	else
	{	
		--menu_stack.pos[0];
		--menu_stack.txt_nr[0];
		menu_ptr -= 2;
	}
	
	cout << "menu_stack.txt_nr[0]=" <<static_cast<int>(menu_stack.txt_nr[0]) << endl;
	cout << "menu_stack.pos[0]=" << static_cast<int>(menu_stack.pos[0]) << endl;
	cout << "menu_ptr=" << menu_ptr << endl;
	text_nr = adresse(menu_ptr, 0);
	cout << "text_nr=" << text_nr << endl;
}
	
void cursor_nach_oben(void) // Cursor nach oben
{
	int text_nr;
	cout << "cursor_nach_oben()" << endl;
	if(menu_stack.txt_nr[0] > 1)
	{	
		menu_ptr -= 2;
		--menu_stack.txt_nr[0];
		--menu_stack.pos[0];
		
		if(menu_stack.pos[0] < 1)
		{
			text_nr = adresse(menu_ptr, 0);
			while(menu_attr[text_nr-1])
			{
				menu_ptr -= 2;
				text_nr = adresse(menu_ptr, 0);
			}
			for(int i = 7; i > 1; --i)
			{	
				memset(txt_dflt[i],0,sizeof(txt_dflt[i]));
				copy(txt_dflt[i-1],txt_dflt[i-1] + sizeof(txt_dflt[i]),txt_dflt[i]);
			}
			zeile_fuellen(1,text_nr);
			menu_stack.pos[0] = 1;
		}
	}
	cout << "menu_stack.txt_nr[0]=" <<static_cast<int>(menu_stack.txt_nr[0]) << endl;
	cout << "menu_stack.pos[0]=" << static_cast<int>(menu_stack.pos[0]) << endl;
	cout << "menu_ptr=" << menu_ptr << endl;
	text_nr = adresse(menu_ptr, 0);
	cout << "text_nr=" << text_nr << endl;
}
	

int find_var(unsigned char ptr)
{
	unsigned int p;
	cout << "find_var(ptr)="<< static_cast<int>(ptr) << endl;
	for(p = 0; p < sizeof(txt_dflt[0]); p++)
	{
		// cout<<static_cast<int>(txt_dflt[ptr][p])<<',';
		if(static_cast<int>(txt_dflt[ptr][p]) == 0x01)
			break;
	}
	cout << endl;
	if(p < sizeof(txt_dflt[0]))
	{
		string s;
		//~ int cur;
		// if(menu_stack.cursor[0] < 7)
		// {	
			// cur = menu_stack.cursor[0];
		// }
		// else
			// cur = 7;
		s += static_cast<char>(txt_dflt[menu_stack.pos[0]][p+1]);
		s += static_cast<char>(txt_dflt[menu_stack.pos[0]][p+2]);
		s += static_cast<char>(txt_dflt[menu_stack.pos[0]][p+3]);
		p = atoi(s.c_str());
	}
	else p = -1;
	return p;
}
void menu_anzeigen(void)
{
	for(int i=0; i<8; i++)
	{	
		if(i==menu_stack.pos[0])
			cout << "> ";
		else
			cout << "  ";	
		for(int y = 0; txt_dflt[i][y] > 0; y++)
		{	
			if(static_cast<int>(txt_dflt[i][y]) == 0x01)
			{	
				string s;
				s += static_cast<char>(txt_dflt[i][y+1]);
				s += static_cast<char>(txt_dflt[i][y+2]);
				s += static_cast<char>(txt_dflt[i][y+3]);
				int p = atoi(s.c_str());
				if(i)
				{	
					if(p >= 300)
					{
						cout << " >";
					}
					else
					{
						cout << p;
					}
				}
				y += 3;
				continue;
			}
			cout << txt_dflt[i][y];
		}
		cout << endl;
	}
}

unsigned long Calc_CRC(unsigned char a, unsigned long crc)
{
	unsigned long l;
	unsigned char n;
	
	l = a;
	l <<= 8;
	crc = crc ^ l;
	for(n = 0; n < 8; n++)
	{
		crc = crc << 1;
		if(crc & 0x10000)
		{
			crc = crc & 65535;
			crc = crc ^ 0x1021;
		}
	}
	return crc;
}

class crc_sum
{
		unsigned int crc;
	public:
		crc_sum() : crc(0){};
		void operator()(unsigned char val)
		{
			unsigned int l, 
						 crc_cpy,
						 n;
			
			crc_cpy = crc;
			l = val;
			l <<= 8;
			crc_cpy = crc_cpy ^ l;
			for(n = 0; n < 8; n++)
			{
				crc_cpy = crc_cpy << 1;
				if(crc_cpy & 0x10000)
				{
					crc_cpy = crc_cpy & 0x0000ffff;
					crc_cpy = crc_cpy ^ 0x1021;
				}
			}
			crc = crc_cpy;
		}
		operator unsigned int() { return crc;}
};

class sonder_zeichen
{
		vector<vector<unsigned char> > sz;
	public:
		sonder_zeichen(unsigned char v[][5], int len)
		{
			vector<unsigned char> tmp;
			int anz_z = len / sizeof(v[0]);
			for(int i = 0; i < anz_z; ++i)
			{
				copy(v[i], v[i] + sizeof(v[i]), back_inserter(tmp));
				sz.push_back(tmp);
				tmp.clear();
			}
		}
		operator vector<vector<unsigned char> >()
		{
			return sz;
		}
};


// vector<unsigned char> vc;
int main (int argcs , char* pArgs[])
{
	ofstream aus, aus_txt;
    ifstream in;
	int i = 0;
	int //width, 
		//height,
		anz_Txt,
		anz_Spr;
	unsigned int crc;
	
	
	vc.clear();
	vc.push_back(0x00); // [0] Länge reservieren
	vc.push_back(0x00); // [1] Länge reservieren
	vc.push_back(0x00); // [2] Länge reservieren
	
	vc.push_back(0x00); // [3] Anfangadresse Sonderzeichen reservieren
	vc.push_back(0x00); // [4] Anfangadresse Sonderzeichen reservieren
	vc.push_back(0x00); // [5] Anfangadresse Sonderzeichen reservieren
	
	// vc.push_back(0x00); // [6] gewählte Sprache
	anz_Spr = sizeof(text)/sizeof(text[0]); // 12 Sprachen
	vc.push_back(anz_Spr); //[6]  6 Sprachen
	cout <<"anz_Spr=" << anz_Spr << endl;
	anz_Txt = sizeof(text_de) / sizeof(text_de[0]);
	cout <<"anz_Txt=" << anz_Txt << endl;
	vc.push_back(anz_Txt >> 8); // [7] Texten pro Sprache 2 Byte: MSByte
	vc.push_back(anz_Txt); 		// [8] Texten pro Sprache 2 Byte: LSByte
	vc.push_back(0x00); // [ 9] Menu Startadr. - 3 Byte,
	vc.push_back(0x00); // [10] 
	vc.push_back(0x00); // [11]
	vc.push_back(0x00); // [12] Menu_attr Startadr. - 3 Byte,
	vc.push_back(0x00); // [13] 
	vc.push_back(0x00); // [14]
	offset = vc[7];
	offset <<= 8;
	offset |= vc[8]; // Anzahl der Texten
	cout <<"anz_Txt=" << offset << ". vc.size()=" << vc.size() << endl;
	// Textadressen reservieren
	for(int y = 0; y < anz_Spr; y++)	
		for(int i = 0; i < anz_Txt; i++)	
			for(int x = 0; x < 3; x++) // 3 Bytes pro Textadresse
				vc.push_back(0x00); // [15+x],[17+x],[18+x] ...
	
	offset *= 3;
	cout << "offset=" << offset << endl;
	cout << "sizeof(txt_dflt[0])=" << sizeof(txt_dflt[0]) << endl;
	cout <<" vc.size()=" << vc.size() << endl;
	// 0..2 ->length, 3..5 Sonderzeichen, 6 - Anzahl der Sprachen,7..8-Anzahl der Texten,9..11 - Menu Startadr,12..14 Menu_attr
	for(int x = 0; x < anz_Spr; ++x)
	{	
		for(int i = 0; i < anz_Txt; ++i)
		{
			int adr = vc.size();
			vc[15 + i*3 + x*offset] = static_cast<unsigned char>(adr >> 16);
			vc[16 + i*3 + x*offset] = static_cast<unsigned char>(adr >> 8); 
			vc[17 + i*3 + x*offset] = static_cast<unsigned char>(adr); 
			vector<char> v_tmp;
			v_tmp.clear();
			copy(text[x][i], text[x][i] + strlen(text[x][i]), back_inserter(v_tmp));
			// copy(v_tmp.begin(),v_tmp.end(),ostream_iterator<char>(cout));
			// cout << endl;
			
			replace(v_tmp.begin(),v_tmp.end(),'#',static_cast<char>(0x01));
			copy(v_tmp.begin(),v_tmp.end(), back_inserter(vc));
			vc.push_back(0);
		}
		
		cout <<" vc.size()=" << vc.size() <<", x = " << x << endl;
		//~ char c;
		//~ cin >> c;
	}
	i = vc.size();
	cout <<"----------------" << endl;
	cout <<" vc.size()=" << vc.size() << endl;

	vc[ 9] = static_cast<unsigned char>(i >> 16);
	vc[10] = static_cast<unsigned char>(i >> 8); 
	vc[11] = static_cast<unsigned char>(i); 
	
	// Menus
	for(unsigned int x = 0; x < sizeof(menu)/sizeof(menu[0]); ++x)
	{
		for(int y = 0; y < len[x]; ++y)
		{	
			vc.push_back(*(menu[x]+y) >> 8);
			vc.push_back(*(menu[x]+y));
		}
		vc.push_back(0);
		vc.push_back(0);
	}
	crc = 0;
	// Anfangsadresse Menu_attr
	i = vc.size();
	//~ vc[12] = static_cast<unsigned char>(i >> 16);
	//~ vc[13] = static_cast<unsigned char>(i >> 8); 
	//~ vc[14] = static_cast<unsigned char>(i); 
	vc[12] = static_cast<unsigned char>(KENNUNG);
	vc[13] = static_cast<unsigned char>(version >> 8); 
	vc[14] = static_cast<unsigned char>(version); 
	copy(menu_attr, menu_attr + sizeof(menu_attr), back_inserter(vc));
	
	// Anfangsadresse Sonderzeichen
	i = vc.size();
	int anf_sz = i;
	cout << "anf_sz=" << anf_sz << endl;
	
	vc[3] = static_cast<unsigned char>(i >> 16);
	vc[4] = static_cast<unsigned char>(i >> 8); 
	vc[5] = static_cast<unsigned char>(i);
	for(int i = 0; i < anz_Spr*3; ++i)
		vc.push_back('$');
	
		
	vector<vector<unsigned char> > de = sonder_zeichen(de_sz, sizeof(de_sz));
	vector<vector<unsigned char> > kyr = sonder_zeichen(kyr_sz, sizeof(kyr_sz));
	vector<vector<vector<unsigned char> > > S_Zeichen;
	S_Zeichen.push_back(de);
	S_Zeichen.push_back(kyr);
	vector<int> sz_adr;
	for(unsigned int i = 0; i < S_Zeichen.size(); ++i)
	{
		if(S_Zeichen[i].size() > 0)
		{	
			sz_adr.push_back(vc.size());
			for(unsigned int y = 0; y < S_Zeichen[i].size(); ++y)
			{	
				copy(S_Zeichen[i][y].begin(), S_Zeichen[i][y].end(), back_inserter(vc));
			}
		}
	}
	// de, en, fr, sp, sl, ua, ru, pl, nl, it, ro, sr
	for(int i = 0; i < anz_Spr; ++i)
	{	
		switch(i)
		{
			//~ case 0: case 1: case  2: case  3: case  4: case 7:
			//~ case 8: case 9: case 10: case 11: case 12: 
			default:
				vc[anf_sz++] = static_cast<unsigned char>(sz_adr[0] >> 16); // de, en, fr, sp, sl,, pl
				vc[anf_sz++] = static_cast<unsigned char>(sz_adr[0] >> 8); 
				vc[anf_sz++] = static_cast<unsigned char>(sz_adr[0]);
			break;
			case 5: case 6:
				vc[anf_sz++] = static_cast<unsigned char>(sz_adr[1] >> 16); // ua, ru
				vc[anf_sz++] = static_cast<unsigned char>(sz_adr[1] >> 8); 
				vc[anf_sz++] = static_cast<unsigned char>(sz_adr[1]);
			break;
			//~ default:
			//~ break;
		}
	}
/* 	
	copy(font_sz,font_sz+sizeof(font_sz),back_inserter(vc));
	for(int y = 0; y < sizeof(de_sz)/sizeof(de_sz[0]); ++y)
		copy(de_sz[y],de_sz[y] + sizeof(de_sz[y]),back_inserter(vc));
	for(int y = 0; y < sizeof(kyr_sz)/sizeof(kyr_sz[0]); ++y)
		copy(kyr_sz[y],kyr_sz[y] + sizeof(kyr_sz[y]),back_inserter(vc));
 */		
	i = vc.size();
	cout << "vc.size() = " << hex << i << endl;
	vc[0] = static_cast<unsigned char>(i >> 16);
	vc[1] = static_cast<unsigned char>(i >> 8);
	vc[2] = static_cast<unsigned char>(i);
	cout.width(2);
	cout.fill('0');
	cout << "vc[0..2] = " << hex <<static_cast<int>(vc[0]) << static_cast<int>(vc[1]) << static_cast<int>(vc[2]) << endl;
	cout << "Erste 6 Adressen: " << hex << endl;
	copy(vc.begin(),vc.begin()+6,ostream_iterator<unsigned int>(cout,","));
	cout << endl << "Erste 6 Adressen. " << endl;
	
	crc = for_each(vc.begin(),vc.end(), crc_sum());
	cout << "crc = " << crc << endl;
	vc.push_back(static_cast<unsigned char>(crc >> 8));
	vc.push_back(static_cast<unsigned char>(crc));
	crc = for_each(vc.begin(),vc.end(), crc_sum());
	cout << "crc = " << crc << endl;
	
	aus.open ("eeprom.bin",ios::binary);	
	// aus_txt.open ("eeprom.h");	
	// copy(vc.begin(),vc.end(),ostream_iterator<unsigned int>(cout,","));
	if(aus)
	{	
		copy(vc.begin(),vc.end(),ostream_iterator<unsigned char>(aus));
		aus.close();
	}
	if(0)//aus_txt)
	{	
		aus_txt << "unsigned char eeprom[] = {";
		
		for(unsigned int i = 0; i < vc.size(); i++)
		{	
			if(i% 20==0)
			{
				aus_txt << '\n';
			}
			aus_txt << static_cast<unsigned int>(vc[i]) <<", ";
		}
		aus_txt << "};" << endl;
		
		aus_txt.close();
	}
	//exit(0);
	// Eeprom bearbeiten
	{
		int	//adr0,
			//text_nr_r,
			//~ ix,
			//txt_cnt_max,
			mn_attr;
		//int unter_menu = 0;
	// 0..2 ->length, 3..5 Sonderzeichen, 6-aktSprache,7 - Anzahl der Sprachen,8..9-Anzahl der Texten,10..12 - Menu Startadr,13..15 Menu_attr
	// 0..2 ->length, 3..5 Sonderzeichen, 6 - Anzahl der Sprachen,7..8-Anzahl der Texten,9..11 - Menu Startadr,12..14 Menu_attr
		aktSprache = 0;// vc[6];
		anz_spr    = vc[6];
		anz_txt    = vc[7]; anz_txt <<= 8;
		anz_txt   |= vc[8];
		menu_anf = adresse(9,1);
		cout << "aktSprache="<<aktSprache<<", anz_spr="<<anz_spr<<", anz_txt="<<anz_txt<<", menu_anf="<< menu_anf<<endl;	
		//~ ix = 0;
		mn_attr = adresse(12,1);
		cout << "menu_attr="<<mn_attr<<endl;	
		//vector<string> mnu;
		char c = '1';
		// string s;
		aktSprache=0;
		memset(menu_stack.menu_nr,0,sizeof(menu_stack.menu_nr));
		memset(menu_stack.txt_nr,0,sizeof(menu_stack.txt_nr));
		memset(menu_stack.pos,0,sizeof(menu_stack.pos));
		menu_stack.menu_nr[0] = 0;
		menu_stack.txt_nr[0]  = 1;
		menu_stack.pos[0]     = 1;
		menu_stack.size       = 0;
		//txt_cnt_max = 
		menu_fuellen3();
		menu_anzeigen();
		while(c)
		{	
			cin >> c;
			switch(c)
			{
				case 'a': // nach unten
					cursor_nach_unten();
					menu_anzeigen();
					continue;
				break;	
				case 'b': // nach oben
					cursor_nach_oben();
					menu_anzeigen();
				break;
				case 'u': // nach Untermenu
					int p;
					
					p = find_var(menu_stack.pos[0]);
					cout << "p = " << p << endl;
					if(p >= 300)
					{	
						int gr = sizeof(menu_stack.txt_nr) - 1;
	cout << "menu_stack.txt_nr[0]=" <<static_cast<int>(menu_stack.txt_nr[0]) << endl;
	cout << "menu_stack.pos[0]=" << static_cast<int>(menu_stack.pos[0]) << endl;
						for(int i = gr; i > 0; i-- )
						{
							menu_stack.txt_nr [i] = menu_stack.txt_nr [i - 1];
							menu_stack.pos    [i] = menu_stack.pos    [i - 1];
							menu_stack.menu_nr[i] = menu_stack.menu_nr[i - 1];
						}
						menu_stack.menu_nr[0] = (p % 100) + 1;
						menu_stack.txt_nr[0]  = 1;
						menu_stack.pos[0]     = 1;
						//txt_cnt_max = 
						menu_fuellen3();
						menu_anzeigen();
						menu_stack.size++;
					}
					else
					{
						;//blinken
					}
				break;
				case 'r': // nach Obermenu
					cout << "r ist gedrueckt..."<<endl;
				
					if(menu_stack.size)
					{
						//~ int tmp_cursor, tmp_curs;
						int gr = sizeof(menu_stack.txt_nr) - 1;
						--menu_stack.size;
	cout << "menu_stack.txt_nr[1]=" <<static_cast<int>(menu_stack.txt_nr[1]) << endl;
	cout << "menu_stack.pos[1]=" << static_cast<int>(menu_stack.pos[1]) << endl;
						for(int i = 0; i < gr; i++ )
						{
							menu_stack.txt_nr [i] = menu_stack.txt_nr [i + 1];
							menu_stack.pos    [i] = menu_stack.pos    [i + 1];
							menu_stack.menu_nr[i] = menu_stack.menu_nr[i + 1];
						}
						cout << "menu_stack.txt_nr [0]="<<static_cast<int>(menu_stack.txt_nr [0])<<endl; 
						cout << "menu_stack.pos [0]="<<static_cast<int>(menu_stack.pos [0])<<endl; 
						//txt_cnt_max = 
						menu_fuellen3();
						menu_anzeigen();
					}
				break;
				
				default:
					c = 0;
				break;	
			}
		}
		//copy(mnu.begin(),mnu.end(), ostream_iterator<string>(cout,"\n"));
	}
}

