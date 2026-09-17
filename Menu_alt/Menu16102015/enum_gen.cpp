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

#define DIR_VAR 300


using namespace std;

class enum_file {
		vector<string> str_i;
		vector<string> str_t;
		vector<string> str_tn;
		vector<string> str_in;
		vector<string> str_ix;
		vector<string> str;
		int cnt;
		vector<string>::iterator p;
	public:
		enum_file(int anz) : cnt(anz){}
		
		bool no_exist(vector<string>& st, string& test) {
			p = find(st.begin(), st.end(), test);
			return p == st.end();
		}
		
		void operator()(string s) {
			unsigned int x1;
			x1 = s.find("?");
			if(x1 != string::npos) {
				++x1;
				string sx;
				istringstream in(s.substr(x1,100));
				in >> sx;
				//~ p = find(str.begin(), str.end(), sx);
				//~ if(p == str.end()) {
					if(sx.find("_in") != string::npos) {	// int nicht speichern
						if(no_exist(str_in, sx))
							str_in.push_back(sx);
					} else if(sx.find("_nt") != string::npos) { // txt nicht speichern
						if(no_exist(str_tn, sx))
							str_tn.push_back(sx);
					} else if(sx.find("_t") != string::npos) { // txt speichern
						if(no_exist(str_t, sx))
							str_t.push_back(sx);
					} else if(sx.find("_ix") != string::npos)  {// int nicht speichern ohne default werte
						if(no_exist(str_ix, sx))
							str_ix.push_back(sx);
					} else {
						if(no_exist(str_i, sx))
							str_i.push_back(sx);
					}		
				//~ }	
			}
		}
		operator vector<string>() {
			vector<string> ret;
			for(p = str_i.begin(); p != str_i.end(); ++p) {
				ostringstream os;
				os << "/* " << setfill(' ') << setw(3) << cnt++ << " */ " << *p;
				ret.push_back(os.str());
			}
			for(p = str_t.begin(); p != str_t.end(); ++p) {
				ostringstream os;
				os << "/* " << setfill(' ') << setw(3) << cnt++ << " */ " << *p;
				ret.push_back(os.str());
			}
			for(p = str_tn.begin(); p != str_tn.end(); ++p) {
				ostringstream os;
				os << "/* " << setfill(' ') << setw(3) << cnt++ << " */ " << *p;
				ret.push_back(os.str());
			}
			for(p = str_in.begin(); p != str_in.end(); ++p) {
				ostringstream os;
				os << "/* " << setfill(' ') << setw(3) << cnt++ << " */ " << *p;
				ret.push_back(os.str());
			}
			//~ for(int m = 0; m < 3; ++m) {	
				//~ ostringstream os;
				//~ os << "/* " << setfill(' ') << setw(3) << cnt++ << " */ " << "lng_adr" << m;
				//~ ret.push_back(os.str());
			//~ }
			for(p = str_ix.begin(); p != str_ix.end(); ++p) {
				ostringstream os;
				os << "/* " << setfill(' ') << setw(3) << cnt++ << " */ " << *p;
				ret.push_back(os.str());
			}
			return ret;
		}
};

//~ char replaceJunk(char c) {
	//~ return (isalpha(c) || isdigit(c) || c == '(' || c==')' || c == '-' || c=='*' || c=='/' || c=='ö' || c=='.' || c==',' || c=='_') ? c : ' ';
//~ }

int main (int argcs , char* pArgs[]) {
	ofstream enm_h;
    ifstream in;
	if(argcs == 2)
		in.open (pArgs[1]);
	else
		//~ in.open("menu_intvar.txt");
		in.open("menu2.txt");
	
	if(in) {
		enm_h.open("enum.h");
		cout << "Datei geoeffnet!" << endl;
		string str;
		deque<string> raw_text;
		copy(istream_iterator<string>(in),istream_iterator<string>(),back_inserter(raw_text));
		//~ while(getline(in,str)) 	{
			//~ raw_text.push_back(str);
		//~ }
		cout << "raw_text.size() = " << raw_text.size() << endl;
		vector<string> enm = for_each(raw_text.begin(), raw_text.end(), enum_file(0));
		copy(enm.begin(), enm.end(), ostream_iterator<string>(cout,",\n"));
		enm_h << "enum {\n";
		copy(enm.begin(), enm.end(), ostream_iterator<string>(enm_h,",\n"));
		enm_h << "};\n";
	}
}

