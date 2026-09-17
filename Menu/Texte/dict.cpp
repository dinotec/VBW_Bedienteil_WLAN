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


class h_datei
{
		vector<string> hh;
		int i;
	public:
		h_datei() : i(0){}
		void operator()(string& str)
		{
			unsigned int anf, end;
			anf = str.find_first_of("\"");
			end = str.find_last_of("\"");
			if(anf != end && end != string::npos)
			{	
				string s = str.substr(anf, end-anf+2);
				while(s.length() < 50)
					s +=" ";
				s += "|--------------"; 
				ostringstream os;
				os << "/* " << setfill(' ') << setw(3) << ++i << " */ " << s;
				hh.push_back(os.str());
			}
		}
		operator vector<string>()
		{
			return hh;
		}
};


char replaceJunk(char c) 
{
	// Only keep alphas, space (as a delimiter), and '
	if(c=='„' || c=='ƒ')
		return c;
	else
	{	
		return (isdigit(c) || c == '#' || c=='!' || c==':' || c=='\"' || c==',') ? ' ' : c;
	}
}




int main (int argcs , char* pArgs[]) {
	ofstream aus_h;
    ifstream in,in2,in3;
	if(argcs == 2)
		in2.open (pArgs[1]);
	else {	
		in2.open("text_conv.txt");
	}
	in.open("list_txt.txt");
	in3.open("menu_txt.txt");
	
	if(in && in2 && in3) {
		aus_h.open("format_.h");
		cout << "Dateien geoeffnet!" << endl;
		map<string,string> dict;
		string str;
		vector<string> raw_text1;
		vector<string> raw_text2;
		// copy(istream_iterator<string>(in),istream_iterator<string>(),back_inserter(raw_text));
		while(getline(in,str)) {
			raw_text1.push_back(str);
		}
		while(getline(in2,str)) {
			raw_text2.push_back(str);
		}
		cout << "raw_text1.size() = " << raw_text1.size() << endl;
		cout << "raw_text2.size() = " << raw_text2.size() << endl;
		for(unsigned int i=0; i < raw_text2.size(); ++i) {
			dict.insert(make_pair(raw_text1[i],raw_text2[i]));
		}
		//~ for(map<string,string>::iterator it=dict.begin(); it != dict.end(); ++it) {
			//~ cout << (*it).first << " : " << (*it).second << endl;
		//~ }
		//~ exit(0);
		// copy(h_file.begin(),h_file.end(),ostream_iterator<string>(aus_h,"\n"));
		string s;
		vector<string> vs, v_sec;
		unsigned int pos, anz = 0;
		while(getline(in3,str)) {
			//~ cout << str << endl;
			vs.clear();
			v_sec.clear();
			for(map<string,string>::iterator it=dict.begin(); it != dict.end(); ++it) {
				pos = str.find((*it).first);
				if(pos != string::npos) {
					vs.push_back((*it).first);
					v_sec.push_back((*it).second);
				}	
			}
			unsigned int len = 0, n = 0;
			for(unsigned int i=0; i < vs.size(); ++i) {
				if(vs[i].size() > len)
					len = vs[i].size(), n = i;
			}
			pos = str.find(vs[n]);
			if(pos != string::npos) {
				str.replace(pos,vs[n].length(), v_sec[n]);
				++anz;
			}	
			aus_h << str << endl;
		}
		cout << anz << " Zeilen ersetzt" << endl;
	}
	else
		cout << "Fehler bei der Dateioeffnung..." << endl;
	
	char c;
	cout << "Beliebige Taste drücken + Enter..." << endl;
	cin >> c;
}

