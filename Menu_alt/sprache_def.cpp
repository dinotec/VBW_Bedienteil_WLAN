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
	ofstream sprache_def;
    ifstream in;
    string str;
	if(argcs == 2)
		in.open (pArgs[1]);
	else
		in.open("menu.txt");
	
	if(in) {
		sprache_def.open("sprache_def.h");
		vector<string> raw_text;
		//~ copy(istream_iterator<string>(in), istream_iterator<string>(), back_inserter(raw_text));
		while(getline(in,str)) 	{
			raw_text.push_back(str);
		}
		//~ copy(raw_text.begin(), raw_text.end(), ostream_iterator<string>(cout,",\n"));
		//~ return 0;
		for(it_str i = raw_text.begin(); i != raw_text.end(); ++i) {
			unsigned int pos = (*i).find('?');
			if(pos != string::npos) {
				string sx;
				istringstream in((*i).substr(pos+1,100));
				in >> sx;
				
				unsigned int cm = (*i).find("\",");
				if(cm != string::npos) {
					string s = (*i).substr(0,cm+2);
					//~ (*i).insert(cm+1, " xstr(" + sx +")");
					s.insert(cm+1, " xstr(" + sx +")");
					sprache_def << s << endl;
				}
			}
		}
		sprache_def.close();	
	}	
}		
