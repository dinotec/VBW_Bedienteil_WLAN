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
#include <string.h>

using namespace std;

int main (int argcs , char* pArgs[]) {
	ofstream aus_h;
    ifstream in,in2,in3;
	string lang,lg;
	if(argcs == 2)
		lang = pArgs[1];
	else 	
		lang = "texte_20170102_en.txt";
	in2.open (lang.c_str());
	unsigned int pos  = lang.find('.');
	if(pos != string::npos) {
		lg = lang.substr(pos - 2, 2);
		lang = "sprache_" + lg +".h"; 
	} else
		lang = "format_.h";
	
	in.open("texte.txt");
	in3.open("sprache_de.h");
	
	if(in && in2 && in3) {
		//~ aus_h.open("format_.h");
		aus_h.open(lang.c_str());
		cout << "Dateien geoeffnet!" << endl;
		map<string,string> dict;
		string str;
		vector<string> raw_text1;
		vector<string> raw_text2;
		while(getline(in,str)) {
			raw_text1.push_back(str);
		}
		while(getline(in2,str)) {
			raw_text2.push_back(str);
		}
		cout << "raw_text1.size() = " << raw_text1.size() << endl;
		cout << "raw_text2.size() = " << raw_text2.size() << endl;
		for(unsigned i=0; i < raw_text2.size(); ++i) {
			dict.insert(make_pair(raw_text1[i],raw_text2[i]));
		}
		//~ for(map<string,string>::iterator it=dict.begin(); it != dict.end(); ++it) {
			//~ cout << (*it).first << " : " << (*it).second << endl;
		//~ }
		//~ exit(0);
		// vector<string> h_file = for_each(raw_text.begin(),raw_text.end(),h_datei());
		// copy(h_file.begin(),h_file.end(),ostream_iterator<string>(aus_h,"\n"));
		string s;
		getline(in3,str);
		pos  = str.find('[');
		if(pos != string::npos) {
			str.replace(pos-2,2,lg);
			aus_h << str << endl;
		}
		while(getline(in3,str)) {
			cout << str << endl;
			pos  = str.find('\"');
			if(pos  != string::npos) {
				s = str.substr(++pos,100);
				unsigned int pos2 = s.find('|');
				if(pos2 != string::npos) {
					if(s[0] !='|')
						pos2 -= 1;
				} else {
					pos2 = s.find('#');
					if(pos2 == string::npos) {
						pos2 = s.find('\"');
					}
				}	  	 
				if(pos2 != string::npos) {
					s = s.substr(0,pos2);
					pos2 = s.find(',');
					if(pos2 != string::npos) {
						s = s.substr(0,pos2);
					}
					pos2 = s.find("Ausgang");
					if(pos2 != string::npos) {
						s = s.substr(0,7);
					}
				    //~ istringstream is(s);
					//~ string rs;
					//~ while( is >> rs) {
						//~ int b = rs.length();
						//~ for(int i = 0; i < b; ++i) {
							//~ if(replaceJunk(rs[i])==' ') {
								//~ rs.erase(i,1);
								//~ b = rs.length();
								//~ i = 0;
							//~ }
						//~ }
						//~ if(rs.length() > 0)
							//~ vs.push_back(rs);
					//~ }
				}
			}
			cout << s << endl;
			//~ copy(vs.begin(), vs.end(), ostream_iterator<string>(cout,"\n"));
	//~ char c;
	//~ cout << "Beliebige Taste drücken + Enter..." << endl;
	//~ cin >> c;
			//~ for(unsigned int i=0; i < vs.size(); ++i)
			//~ {
				for(map<string,string>::iterator it=dict.begin(); it != dict.end(); ++it) {
					// int pos = str.find((*it).first);
					// transform(s.begin(), s.end(), s.begin(), replaceJunk);	
					// if(str[pos + (*it).first.length() + 1] == ' ' || str[pos + (*it).first.length() + 1] == ':'|| str[pos + (*it).first.length() + 1] == '#')
					//~ if(vs[i] == (*it).first)
					if(s == (*it).first) {	
						//~ cout << "gefunden!" << endl;
						pos = str.find(s);
						str.replace(pos,(*it).first.length(), (*it).second);
						//~ cout << str << endl;
						break;
					}
				}
				//~ cout << "gefunden! " << i << endl;
			//~ }	
			aus_h << str << endl;
		}
	}
	else
		cout << "Fehler bei der Dateioeffnung..." << endl;
	
	char c;
	cout << "Beliebige Taste drücken + Enter..." << endl;
	cin >> c;
}

