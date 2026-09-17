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
void removeJunk(string& str) {
	unsigned int pos = str.find('\"');
	if(pos != string::npos ) {
		str = str.substr(pos+1, 100);
		pos = str.find('\"');
		if(pos != string::npos) {
			str = str.substr(0,pos);
			pos = str.find(',');
			if(pos != string::npos) {
				str = str.substr(0,pos);
			} else {
				pos = str.find('#');
				if(pos != string::npos) {
					str = str.substr(0,pos);
				}
			}
		}	
	} else 
	 str = "";
}

const char* notranslate [] = {
	"dinotec GmbH",
	"Spessartstr. 7",
	"D-63477 Maintal",
	"Tel:0049-(0)6109-6011-0",
	"Hotline:0049-(0)6109-6011-72",
	"www.dinotec.de",
	"service@dinotec.de",
	"30",
	"60",
	"90",
	"150",
	"200",
	"300",
	"Frei"
};
const int len_nt = sizeof(notranslate)/sizeof(notranslate[0]);
		
class txt_datei {
		set<string> txtlist;
	public:
		void operator()(string& str) {
			//~ unsigned int pos = str.find('\"');
			//~ if(pos != string::npos ) {	
				//~ string s = str.substr(pos+1, 100);
				//~ pos = s.find('\"');
				//~ if(pos != string::npos) {
					//~ s = s.substr(0,pos);
					//~ transform(s.begin(), s.end(), s.begin(), replaceJunk);			
					//~ istringstream is(s);
					//~ string word, 
						   //~ phrase;
					//~ while(is >> word) 
					//~ {
						//~ pos = s.find('#');
						//~ if(pos == string::npos) {
							//~ if(phrase.size() == 0)
								//~ phrase = word;
							//~ else
								//~ phrase += (" " + word);
						//~ } else {
							//~ word = word.substr(0,pos);
							//~ if(phrase.size() == 0)
								//~ phrase = word;
							//~ else
								//~ phrase += (" " + word);
							//~ txtlist.insert(phrase);
							//~ phrase = "";
						//~ }
					//~ }
					//~ pos = s.find('#');
					//~ if(pos != string::npos) {
						//~ s = s.substr(0,pos);
					//~ }
					//~ int i;
					//~ for(i = 0; i < len_nt; ++i) {
						//~ if(notranslate[i] == s)
							//~ break;
					//~ }
					//~ if(i == len_nt)
						//~ txtlist.insert(s);
				//~ }
			//~ }
			txtlist.insert(str);
		}
		operator set<string>() {
			return txtlist;
		}
};

int main (int argcs , char* pArgs[]) {
	ofstream list_txt;
    ifstream in, in2;
	if(argcs == 2)
		in.open (pArgs[1]);
	else
		in.open("sprache_de.h");
	
	if(in) {
		list_txt.open("texte.txt");
		cout << "Datei geoeffnet!" << endl;
		string str;
		deque<string> raw_text;
		int i;
		// copy(istream_iterator<string>(in),istream_iterator<string>(),back_inserter(raw_text));
		while(getline(in,str)) {
			removeJunk(str);
			if(str != "") {
				for(i = 0; i < len_nt; ++i) {
					if(notranslate[i] == str)
						break;
				}
				if(i == len_nt)
					raw_text.push_back(str);
			}
		}
		in.close();
		cout << "raw_text.size() = " << raw_text.size() << endl;
		//-------- einzelne Menueinträge -----
		set<string> list_file = for_each(raw_text.begin(), raw_text.end(), txt_datei());
		in2.open("texte_pcd3.txt");
		if(in2) {
			cout << "Datei geoeffnet!" << endl;
			while(getline(in2,str)) {
				//~ cout << str << endl;
				list_file.insert(str);
			}
			in2.close();	
		} else
			cout << "Datei nicht gefunden..." << endl;
		copy(list_file.begin(), list_file.end(), ostream_iterator<string>(list_txt,"\n"));
		//~ copy(raw_text.begin(), raw_text.end(), ostream_iterator<string>(list_txt,"\n"));
		list_txt.close();
		cout << "list_file.size() = " << list_file.size() << endl;
		//-------------
		//~ copy(var3.begin(), var3.end(), ostream_iterator<string>(cout,"\n"));
	}
}
