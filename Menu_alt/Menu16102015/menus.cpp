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
typedef vector<int>::iterator it_int;
typedef vector<vector<int> >::iterator it_vek;

int main (int argcs , char* pArgs[]) {
	ofstream menus;
    ifstream in;
	if(argcs == 2)
		in.open (pArgs[1]);
	else
		in.open("menu2.txt");
	
	if(in) {
		menus.open("menus.h");
		vector<string> raw_text;
		vector<vector<int> > mn;
		string str;
		//~ copy(istream_iterator<string>(in), istream_iterator<string>(), back_inserter(raw_text));
		while(getline(in, str)) 	{
			raw_text.push_back(str);
		}
		//~ copy(raw_text.begin(), raw_text.end(), ostream_iterator<string>(cout,",\n"));
		//~ return 0;
		int cnt = 1;
		vector<int> tmp;
		for(it_str i = raw_text.begin(); i != raw_text.end(); ++i, ++cnt) {
			if((*i)[0] != '\t') {
				if((*i).find("Frei") == string::npos) {
					tmp.push_back(cnt);
				} else break; 
			}	
		}
		mn.push_back(tmp);
		cnt = 1;
		bool b = false;
		tmp.clear();
		for(it_str i = raw_text.begin(); i != raw_text.end(); ++i, ++cnt) {
			unsigned int pos = (*i).find("\t\"");
			if(pos == 0) {
				if(b == false) {
					b = true;
					tmp.push_back(cnt-1);
					tmp.push_back(cnt);
				} else {
					tmp.push_back(cnt);
				}
			} else if(pos == string::npos && b == true) {
				b = false;
				mn.push_back(tmp);
				tmp.clear();
			}	

		}	
		cnt = 1;
		b = false;
		tmp.clear();
		for(it_str i = raw_text.begin(); i != raw_text.end(); ++i, ++cnt) {
			unsigned int pos = (*i).find("\t\t\"");
			if(pos == 0) {
				if(b == false) {
					b = true;
					tmp.push_back(cnt-1);
					tmp.push_back(cnt);
				} else { 
					tmp.push_back(cnt);
				}
			} else if(pos == string::npos && b == true) {
				b = false;
				mn.push_back(tmp);
				tmp.clear();
			}	
		}

		cnt = 1;
		b = false;
		tmp.clear();
		for(it_str i = raw_text.begin(); i != raw_text.end(); ++i, ++cnt) {
			unsigned int pos = (*i).find("\t\t\t\"");
			if(pos == 0) {
				if(b == false) {
					b = true;
					tmp.push_back(cnt-1);
					tmp.push_back(cnt);
				} else { 
					tmp.push_back(cnt);
				}
			} else if(pos == string::npos && b == true) {
				b = false;
				mn.push_back(tmp);
				tmp.clear();
			}	
		}
		sort(mn.begin(), mn.end());
		int nr = 0;
		for(it_vek v = mn.begin(); v != mn.end(); ++v) {
			menus << "int menu_" << nr++ << "[] = {";
			for(it_int iy = (*v).begin(); iy != (*v).end(); ++iy)
				menus << (*iy) << ',';
			menus << "};" << endl;
		}		
		menus.close();	
	}	
}		
