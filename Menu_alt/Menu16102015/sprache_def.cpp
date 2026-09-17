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

#define npos 0xffffffff

using namespace std;

typedef vector<string>::iterator it_str;

int main (int argcs , char* pArgs[]) {
	ofstream sprache_def;
    ifstream in;
    string str;
	if(argcs == 2)
		in.open (pArgs[1]);
	else
		in.open("menu2.txt");
	
	if(in) {
		sprache_def.open("sprache_def.h");
		sprache_def << "const char* text_de[] = {" << endl;
		vector<string> raw_text;
		//~ copy(istream_iterator<string>(in), istream_iterator<string>(), back_inserter(raw_text));
		while(getline(in,str)) 	{
			raw_text.push_back(str);
		}
		//~ copy(raw_text.begin(), raw_text.end(), ostream_iterator<string>(cout,",\n"));
		//~ return 0;
		unsigned int dir = 300;
		for(it_str i = raw_text.begin(); i != raw_text.end(); ++i) {
			unsigned int pos = (*i).find('#');
			if(pos != npos) {
				ostringstream os;
				os << dir++;
				(*i).insert(pos+1, os.str());			
			}
			pos = (*i).find('$');
			if(pos != npos) {
				(*i).replace(pos,1,"#");			
			}
			pos = (*i).find('?');
			unsigned int pk1 = (*i).find('\"');
			unsigned int cm = (*i).find("\",");
			if(pk1 != npos) {
				if(pos != npos) {
					string sx;
					istringstream in((*i).substr(pos+1,100));
					in >> sx;
					
					if(cm != npos) {
						string s = (*i).substr(0,cm+2);
						//~ (*i).insert(cm+1, " xstr(" + sx +")");
						s.insert(cm+1, " xstr(" + sx +")");
						//~ sprache_def << '\t' << s.substr(pk1,100) << endl;
						sprache_def << s.substr(0,100) << endl;
					}
				} else {
					//~ sprache_def << '\t' << (*i).substr(pk1,cm+2 - pk1) << endl;
					sprache_def << (*i).substr(0, cm + 2) << endl;
				}
			}
		}
		sprache_def << "};" << endl;
		sprache_def.close();	
	}	
}		
