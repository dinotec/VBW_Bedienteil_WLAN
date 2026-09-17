#include <stdio.h>
#include <algorithm>
#include <vector>
#include <deque>
#include <set>
#include <list>
#include <map>
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iterator>
#include <iomanip>
// #include "enumer.h"

using namespace std;
typedef union UINT 
{                   // byte-addressable int
  char b[4];
  int i;
  unsigned int ui;
} UINT;

bool sp;
void zeichen(int& ws)
{
    switch(ws)
	{
		case 0x00c4: ws = 0x0081;break; 	// 'Ä'
		case 0x00e4: ws = 0x0082;break;		// 'ä'
		case 0x00dc: ws = 0x0083;break;		// 'Ü'
		case 0x00fc: ws = 0x0084;break;		// 'ü'
		case 0x00d6: ws = 0x0085;break;		// 'Ö'
		case 0x00f6: ws = 0x0086;break;		// 'ö'
		case 0x00df: ws = 0x0087;break;		// 'ß'
		case 0x00b0: ws = 0x007f;break;		// '°'
		//--------- französische && spanische Zeichen -------------
		case 0x00e1: ws = 0x0088;break;		// 'á'
		case 0x00e0: ws = 0x0089;break;		// 'à'
		case 0x00e2: ws = 0x008a;break;		// 'â'
		case 0x00e7: ws = 0x008b;break;		// 'ç'
		case 0x00e9: ws = 0x008c;break;		// 'é'
		case 0x00e8: ws = 0x008d;break;		// 'è'
		case 0x00ea: ws = 0x008e;break;		// 'ê'
		case 0x00f3: ws = 0x008f;break;		// 'ó'
		case 0x00f2: ws = 0x0090;break;		// 'ò'
		case 0x00f4: ws = 0x0091;break;		// 'ô'
		case 0x00ec: ws = 0x0092;break;		// 'ì' mit Gravis
		case 0x00f1: ws = 0x0093;break;		// 'ñ'
		case 0x00fa: ws = 0x0094;break;		// 'ú'
		//--------- slowenische Zeichen -------------
		case 0x0161: ws = 0x0095;break;		// 'š'
		case 0x017e: ws = 0x0096;break;		// 'ž'
		case 0x010d: ws = 0x0097;break;		// 'c' mit Caron
		//--------- polnische Zeichen -------------
		case 0x0142: ws = 0x0098;break;		// 98 'l' mit Qwerstrich 0x0142
		case 0x0105: ws = 0x0099;break;		// 99 'a' mit Ogonek     0x0105
		case 0x017a: ws = 0x009a;break;		// 9a 'z' mit Akut       0x017a
		case 0x015b: ws = 0x009b;break;		// 9b 's' mit Akut       0x015b
		case 0x0107: ws = 0x009c;break;		// 9c 'c' mit Akut       0x0107
		case 0x0119: ws = 0x009d;break;		// 9d 'e' mit Ogonek     0x0119
		case 0x017c: ws = 0x009e;break;		// 9e 'z' mit Punkt oben 0x017c
		case 0x0144: ws = 0x009f;break;		// 9f 'n' mit Akut		 0x0144
		// ---- rumänische Zeichen ----------                                   
		case 0x0103: ws = 0x00a0;break;		// a0 'a' mit Brevis	 0x0103
		case 0x015f: ws = 0x00a1;break;		// a1 's' mit Cedile	 0x015f
		case 0x0163: ws = 0x00a2;break;		// a2 't' mit Cedile	 0x0163
		case 0x021b: ws = 0x00a2;break;		// a2 't' mit Cedile	 0x0163
		case 0x0111: ws = 0x00a3;break;		// a2 'd' mit Querstrich 0x0111
		// tschechische Zeichen
		case 0x00ed: ws = 0x0092;break;		// 'í' 
		case 0x00f8: ws = 0x00a4;break;		// 'ø' mit Querstrich
		case 0x00fd: ws = 0x00a5;break;		// 'ý' mit Akut
		case 0x011b: ws = 0x00a6;break;		// 'e' mit Caron
		case 0x0159: ws = 0x00a7;break;		// 'r' mit Caron   
		case 0x016f: ws = 0x00a8;break;		// 'u' mit Ring
		case 0x0151: ws = 0x00a9;break;		// 'o' mit Doppelakut
		case 0x0171: ws = 0x00aa;break;		// 'u' mit Doppelakut
	// }
    // if(ws >= 0x0410 && ws <= 0x044f)
    // {
		// switch(static_cast<unsigned int>(ws))
		// {
			case 0x410: ws = static_cast<int>('A'); break; // A
			case 0x411: ws = 0x81; break; // B
			case 0x412: ws = static_cast<int>('B'); break;
			case 0x413: ws = 0x82; break;
			case 0x414: ws = 0x83; break;
			case 0x415: ws = static_cast<int>('E'); break;
			case 0x416: ws = 0x84; break;
			case 0x417: ws = 0x85; break;
			case 0x418: ws = 0x86; break;
			case 0x419: ws = 0x87; break;
			case 0x41a: ws = static_cast<int>('K'); break;
			case 0x41b: ws = 0x88; break;
			case 0x41c: ws = static_cast<int>('M'); break;
			case 0x41d: ws = static_cast<int>('H'); break;
			case 0x41e: ws = static_cast<int>('O'); break;
			case 0x41f: ws = 0x89; break;
			case 0x420: ws = static_cast<int>('P'); break;
			case 0x421: ws = static_cast<int>('C'); break;
			case 0x422: ws = static_cast<int>('T'); break;
			case 0x423: ws = 0x8a; break;
			case 0x424: ws = 0x8b; break;
			case 0x425: ws = static_cast<int>('X'); break;
			case 0x426: ws = 0x8c; break;
			case 0x427: ws = 0x8d; break;
			case 0x428: ws = 0x8e; break;
			case 0x429: ws = 0x8f; break;
			case 0x42a: ws = 0x90; break;
			case 0x42b: ws = 0x91; break;
			case 0x42c: ws = 0x92; break;
			case 0x42d: ws = 0x93; break;
			case 0x42e: ws = 0x94; break;
			case 0x42f: ws = 0x95; break;
			case 0x430: ws = static_cast<int>('a'); break;
			case 0x431: ws = 0x96; break;
			case 0x432: ws = 0x97; break;
			case 0x433: ws = 0x98; break;
			case 0x434: ws = 0x99; break;
			case 0x435: ws = static_cast<int>('e'); break;
			case 0x436: ws = 0x9a; break;
			case 0x437: ws = 0x9b; break;
			case 0x438: ws = 0x9c; break;
			case 0x439: ws = 0x9d; break;
			case 0x43a: ws = 0x9e; break;
			case 0x43b: ws = 0x9f; break;
			case 0x43c: ws = 0xa0; break;
			case 0x43d: ws = 0xa1; break;
			case 0x43e: ws = static_cast<int>('o'); break;
			case 0x43f: ws = 0xa2; break;
			case 0x440: ws = static_cast<int>('p'); break;
			case 0x441: ws = static_cast<int>('c'); break;
			case 0x442: ws = 0xa3; break;
			case 0x443: ws = static_cast<int>('y'); break;
			case 0x444: ws = 0xa4; break;
			case 0x445: ws = static_cast<int>('x'); break;
			case 0x446: ws = 0xa5; break;
			case 0x447: ws = 0xa6; break;
			case 0x448: ws = 0xa7; break;
			case 0x449: ws = 0xa8; break;
			case 0x44a: ws = 0xa9; break;
			case 0x44b: ws = 0xaa; break;
			case 0x44c: ws = 0xab; break;
			case 0x44d: ws = 0xac; break;
			case 0x44e: ws = 0xad; break;
			case 0x44f: ws = 0xae; break;
			case 0x451: ws = 0xaf; break;
			case 0x401: ws = 0xb0; break;
			case 0x454: ws = 0xb1; break;
			case 0x456: ws = 0x69; break;
			case 0x457: ws = 0xb2; break;
        // }
		default: break;	
    }
}

int main (int argcs , char* pArgs[]) {
	ofstream aus_h;
    ifstream in;
	string spr;
	if(argcs == 2)
		spr = pArgs[1];
	else {	
		spr = "de_uni.txt";
	}
	in.open(spr.c_str(),ios::binary);
	if(in) {
		unsigned int pos = spr.find("uni");
		if(pos != string::npos) {
			spr.replace(pos,3,"pcd");
		}
		else
			spr ="de_.txt";
		pos = spr.find("txt");
		if(pos != string::npos) {
			spr.replace(pos,3,"h");
		}
		pos = spr.find("sp");
		(pos != string::npos) ? (sp = true) : (sp = false);
			
		aus_h.open(spr.c_str(),ios::binary);
		in.clear();
		UINT w;
		while(1)
		{
			w.i = 0;
			in.read(&w.b[0],1); 
			in.read(&w.b[1],1); 
			if(!in.eof())
			{	
				if(w.i < 0x8000)
				{
					zeichen(w.i); 
					char c = static_cast<char>(w.i); 
					aus_h.write(&c,1);
				}
			}
			else
				break;
		}
		cout << "In Datei " << spr << " geschrieben." << endl;
	}
	else
		cout << "Datei " << spr << " kann nicht geoefnet werden..." << endl;
}

