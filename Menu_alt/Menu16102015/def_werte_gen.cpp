#include <stdio.h>
#include <algorithm>
#include <vector>
#include <deque>
#include <set>
#include <list>
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iterator>
#include <iomanip>

using namespace std;

typedef vector<string>::iterator it_str;

int main (int argcs , char* pArgs[]) {
	ofstream def_werte;
    ifstream inm, inen;
	if(argcs == 2)
		inm.open (pArgs[1]);
	else 
		inm.open("menu2.txt");
	inen.open("enum_def.h");
	
	if(inm && inen) {
		def_werte.open("default_werte.c");
		def_werte << "#include \"ConstDef.h\""  << endl;
		def_werte << "code WERTE tab1[] = {"  << endl;
		vector<string> raw_text;
		string str;
		//~ copy(istream_iterator<string>(in), istream_iterator<string>(), back_inserter(raw_text));
		while(getline(inm,str)) 	{
			raw_text.push_back(str);
		}
		unsigned int pos, pk1, pk2;		
		while(getline(inen, str)) 	{
			string s0, s1, s2;
			istringstream in(str);
			in >> s0 >> s1 >> s2;
			for(it_str i = raw_text.begin(); i != raw_text.end(); ++i) {
				pos = (*i).find(s1);
				if(pos != string::npos) {
					pk1 = (*i).find('{');
					pk2 = (*i).find('}');
					if(pk1 != string::npos) {
						if(pk2 != string::npos) {
							s0 = s1 + " " + s2;
							while(s0.size() < 15) s0 += ' ';
							string s = (*i).substr(pk1,pk2 - pk1 + 1);
							def_werte << "/* " << s0 <<  " */" << "\t" << s << ',' << endl;
						}
					}
					break;
				}
			}	
			
		}
		//~ copy(raw_text.begin(), raw_text.end(), ostream_iterator<string>(cout,",\n"));
		//~ return 0;
		def_werte << "};" << endl;
		def_werte.close();	
	}	
}		
