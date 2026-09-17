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
	ofstream enm_def;
    ifstream in;
    string str;
	if(argcs == 2)
		in.open (pArgs[1]);
	else
		in.open("enum.h");
	
	if(in) {
		enm_def.open("enum_def.h");
		vector<string> raw_text;
		//~ copy(istream_iterator<string>(in), istream_iterator<string>(), back_inserter(raw_text));
		while(getline(in,str)) 	{
			raw_text.push_back(str);
		}
		int z = 0;
		//~ copy(raw_text.begin(), raw_text.end(), ostream_iterator<string>(cout,",\n"));
		//~ return 0;
		for(it_str i = raw_text.begin(); i != raw_text.end(); ++i) {
			unsigned int pos = (*i).find("*/");
			if(pos != string::npos) {
				string sx;
				istringstream in((*i).substr(pos+3,100));
				in >> sx;
				unsigned int cm = sx.find(',');
				if(cm != string::npos)
					sx.erase(cm,1);
				cout << "#define " << sx << " " << z << endl;
				enm_def << "#define " << sx << " " << z++ << endl;
			}
		}
		enm_def << endl << "#define xstr(s) str(s)" << endl;
		enm_def << "#define str(s) #s" << endl;

		enm_def.close();	
	}	
}		
