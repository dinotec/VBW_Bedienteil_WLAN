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

//~ char replaceJunk(char c) {
	// Only keep alphas, space (as a delimiter), and '
	//~ return (isalpha(c) || c == '-' || c=='*' || c=='/' || c=='ö' || c=='.' || c=='_') ? c : ' ';
	//~ return (isalpha(c) || isdigit(c) || c == '-' || c=='*' || c=='/' || c=='ö' || c=='.' || c=='_') ? c : ' ';
	// return (isalpha(c)) ? c : ' ';
//~ }

class txt_datei {
		set<string> txtlist;
	public:
		void operator()(string str) {
			
			unsigned int pos = str.find('\"');
			if(pos != string::npos ) {	
				string s = str.substr(pos+1, 100);
				pos = s.find('\"');
				if(pos != string::npos) {
					s = s.substr(0,pos);
					//~ transform(s.begin(), s.end(), s.begin(), replaceJunk);			
					istringstream is(s);
					string word, 
						   phrase;
					while(is >> word) {
						pos = word.find('#');
						if(pos == string::npos) {
							if(phrase.size() == 0)
								phrase = word;
							else
								phrase += (" " + word);
						} else {
							word = word.substr(0,pos);
							if(phrase.size() == 0)
								phrase = word;
							else
								phrase += (" " + word);
							txtlist.insert(phrase);
							phrase = "";
						}
					}
					txtlist.insert(phrase);
					phrase = "";
				}
			}
		}
		operator set<string>() {
			return txtlist;
		}
};

int main (int argcs , char* pArgs[]) {
	ofstream list_txt;
    ifstream in1, in2;
	if(argcs != 3) {
		cout << "usage: merge Datei1 Datei2\n";
		return 0;
	} else {
		in1.open (pArgs[1]);
		in2.open (pArgs[2]);
	}
	if(in1) {
		if(in2) {
			list_txt.open("merge.txt");
			cout << "Datei geoeffnet!" << endl;
			string str;
			deque<string> raw_text;
			// copy(istream_iterator<string>(in),istream_iterator<string>(),back_inserter(raw_text));
			while(getline(in1,str)) {
				raw_text.push_back(str);
			}
			while(getline(in2,str)) {
				raw_text.push_back(str);
			}
			in1.close();
			in2.close();
			cout << "raw_text.size() = " << raw_text.size() << endl;
			//-------- einzelne Menueinträge -----
			//~ set<string> list_file = for_each(raw_text.begin(), raw_text.end(), txt_datei());
			set<string> list;
			//~ copy(raw_text.begin(), raw_text.end(), back_inserter(list));
			for(deque<string>::iterator it=raw_text.begin(); it != raw_text.end(); ++it)
				list.insert(*it);
			copy(list.begin(), list.end(), ostream_iterator<string>(list_txt,"\n"));
			list_txt.close();
			//-------------
			//~ copy(var3.begin(), var3.end(), ostream_iterator<string>(cout,"\n"));
		} else
			cout << "Datei " << pArgs[2] << " nicht gefunden...\n";
	} else
		cout << "Datei " << pArgs[1] << " nicht gefunden...\n";
	return 0;	
}
