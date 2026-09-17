#include <algorithm>
#include <iterator>
#include <vector>
#include <set>
#include <list>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

//~ #define PFAD "H:\\INTERNER DATENAUSTAUSCH\\"

using namespace std;

int main(int argcs , char* pArgs[]) {
	ofstream aus;
    ifstream in;
	string str = "vbw_update.bin";
	aus.open (str.c_str(), ios::out | ios::binary);

	for(int i = 1; i < argcs; i++) {
		string f_name(pArgs[i]);
		in.open(pArgs[i], ios::in | ios::binary);
		if(in) {
			ostringstream os;
			os << in.rdbuf(); 
			string bin = os.str(); 
			bool ok = true;
			if(f_name.find("OP10") != string::npos || f_name.find("vbw_strg") != string::npos ) {
				//~ cout << "gefunden: " << pArgs[i] << endl;
				unsigned int pos=0;
				while((pos = bin.find("FINISH", pos)) != string::npos) {
					if(pos % 128 == 0) {
						cout << "\"FINISH\": falsche Position: " << pos <<" % 128 == 0" << endl;
						ok = false;
					}
					//~ cout << "pos = " << pos << endl;
					++pos;
				}
			}
			aus << bin;// in.rdbuf();
			in.close();
			if(ok == false) {
				cout << "Datei: " << pArgs[i] << " ist fehlerhaft" << endl;
			}
		} else {
			cout << "Datei " << pArgs[i] << " nicht gefunden???..." << endl;
			char c;
			cin >> c;
		}
	}
	aus.close();
	cout << endl << "Beliebige Taste + Enter druecken..." << endl;
	char c;
	cin >> c;
 }
