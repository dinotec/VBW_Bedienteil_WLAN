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

class enum_tab1
{
		deque<string> en;
		deque<string> tab;
		string datei;
		int cnt;
	public:
		enum_tab1(string cs)
		{ 
			en.push_back("enum\n{\n");
			tab.push_back("code WERTE tab1[] =\n{\n");
			tab.push_back("// dflt,  min,  max \n");
			datei = cs;
			cnt = 0;
		}
		void operator()(string s)
		{
			unsigned int x1,x2;
			x1 = s.find("?");
			if(x1 != string::npos)
			{
				++x1;
				string sx = s.substr(x1,100);
				x2 = sx.find(" ");
				if(x2 != string::npos)
					sx = sx.substr(0,x2);
				ostringstream os;
				os << sx << "=" << cnt++ << ",\n";
				// en.push_back(sx + ",\n");
				en.push_back(os.str());
			}
			string st = "	{    0,     0,     0}, // " + s +"\n";
			tab.push_back(st);
		}
		// operator vector<string>()
		// {
			// return str;
		// }
		void operator()()
		{
			en.push_back("};\n");
			ofstream aus;
			//aus.open("en.h");
			aus.open(datei.c_str());
			if(aus)
			{
				typedef deque<string>::iterator it;
				for(it i = en.begin(); i != en.end(); ++i)
					aus << *i;
				for(it i = tab.begin(); i != tab.end(); ++i)
					aus <<  *i;
			}
		}
};
class txt_de
{
		deque<string> str;
		string datei;
	public:
		txt_de(string cs)
		{ 
			datei = cs; 
		}
		void operator()(string s)
		{
			unsigned int x1;
			x1 = s.find("\",");
			if(x1 != string::npos)
			{
				x1 += 2;
				s = s.substr(0,x1);
				str.push_back(s + "\n");
			}
		}
		void operator()()
		{
			ofstream aus;
			//aus.open("en.h");
			aus.open(datei.c_str());
			if(aus)
			{
				typedef deque<string>::iterator it;
				for(it i = str.begin(); i != str.end(); ++i)
					aus << *i;
				//~ copy(str.begin(), str.end(),ostream_iterator<string>(aus));					
			}
		}
};
class enum_file {
		vector<string> str;
		int cnt;
	public:
		enum_file(int anz) : cnt(anz){}
		void operator()(string s) {
			unsigned int x1;//,x2;
			ostringstream os;
			os << "/* " << setfill(' ') << setw(3) << cnt++ << " */ ";
			x1 = s.find("?");
			if(x1 != string::npos) {
				++x1;
				string sx;
				while(isalpha(s[x1]) || isdigit(s[x1]) || s[x1]=='_')
						sx += s[x1++];
				os << sx;
				str.push_back(os.str());
			}
		}
		operator vector<string>() {
			return str;
		}
};
class Tab1 {
		vector<string> str;
		int cnt;
		bool b;
		char vr_type;
	public:
		Tab1(int i, char c = '$') { 
			b = false;
			vr_type = c;
			if(!i) {
				str.push_back("code WERTE tab1[] =\n{");
				str.push_back("// dflt,  min,  max");
				b = false;
			}
			if(c == '$') cnt = 0;
			else         cnt = i - 1;
		}
		void operator()(string s) {
			unsigned int a, e;
			a = s.find(vr_type);
			if(a != string::npos) {
				a = s.find("{");
				e = s.find("}");
				if(a != e && e != string::npos) {
					string st = s.substr(a,e) + ",";
					string enm;
					a = s.find("?");
					if(a != string::npos) {	
						while(isalpha(s[++a]) || isdigit(s[a]) || s[a]=='_')
							enm += s[a];
						st += " // " + enm;
					}
					ostringstream os;
					os << "/* " << setfill(' ') << setw(3) << cnt++ << " */ " << st;
					if(vr_type == ']') {
						if(b == false)
							b = true, str.push_back(os.str()); 
					} else 
						str.push_back(os.str());
				}
			}
		}
		operator vector<string>() {
			//~ if(b)
				//~ str.push_back("};");
			return str;
		}
};
class Tab2 {
		vector<string> str;
		int cnt;
	public:
		Tab2(int i) : cnt(i - 4) { 
		}
		void operator()(string s) {
			unsigned int ch,a, e;
			ch = s.find('µ');
			if(ch != string::npos) {
				a = s.find("{");
				e = s.find("}");
				if(a != e && e != string::npos) {
					string st = s.substr(a,e) + ",";
					string enm;
					//~ a = s.find("?");
					//~ if(a != string::npos)
					{	
						while(isalpha(s[++ch]) || isdigit(s[ch]) || s[ch]=='_')
							enm += s[ch];
						st += " // " + enm;
					}
					ostringstream os;
					os << "/* " << setfill(' ') << setw(3) << cnt++ << " */ " << st;
					str.push_back(os.str());
				}
			}
		}
		operator vector<string>() {
			return str;
		}
};

class dir_var {
		int dir, var;
		vector<string> var_str;
	public:
		dir_var() { dir = DIR_VAR, var = 0;}
		void operator()(string& str) {
			ostringstream os;
			unsigned int pos = str.find('#');
			if(pos != string::npos ) {	
				os << dir++;
				str.insert(pos+1,os.str());
			}
			ostringstream osv;
			pos = str.find('$');
			if(pos != string::npos ) {	
				var_str.push_back(str);
				osv << var++;
				str.replace(pos,1,"#");
				str.insert(pos+1,osv.str());
				// var_name.push_back(str);
			}
		}
		operator vector<string>() {
			return var_str;
		}
};


class var_2 {
		int var;
		vector<string> enum2;
	public:
	var_2(int v) : var(v) { }
		void operator()(string& str) {
			ostringstream os;
			unsigned int pos = str.find('!');
			if(pos != string::npos ) {	
				unsigned int pos2 = str.find('{');
				if(pos2 != string::npos) {
					os << var++;
					str.replace(pos,1,"#");
					str.insert(pos+1,os.str());
					enum2.push_back(str);
				}
			}
		}
		operator vector<string>() {
			return enum2;
		}
};

class var_3 {
		int var;
		vector<string> enum3;
	public:
		var_3(int v) : var(v) { }
		void operator()(string& str) {
			ostringstream os;
			unsigned int pos = str.find('!');
			if(pos != string::npos ) {	
				unsigned int pos2 = str.find('{');
				if(pos2 == string::npos) {
					os << var++;
					str.replace(pos,1,"#");
					str.insert(pos+1,os.str());
					enum3.push_back(str);
				}
			}
		}
		operator vector<string>() {
			return enum3;
		}
};

class var_txt {
		int var;
		char vr_type;
		vector<string> enum2;
	public:
	var_txt(int v, char c = '&') : var(v), vr_type(c) { }
		void operator()(string& str) {
			ostringstream os;
			//~ unsigned int pos = str.find('&');
			unsigned int pos = str.find(vr_type);
			if(pos != string::npos ) {	
				os << var++;
				str.replace(pos,1,"#");
				str.insert(pos+1,os.str());
				enum2.push_back(str);
			}
		}
		operator vector<string>() {
			return enum2;
		}
};

class h_datei {
		vector<string> hh;
		int i;
	public:
		h_datei(): i(0) {}
		void operator()(string& str) {
			unsigned int pos = str.find("\",");
			if(pos != string::npos ) {	
				string s = str.substr(0, pos+2);
				ostringstream os;
				os << "/* " << setfill(' ') << setw(3) << ++i << " */ " << s;
				hh.push_back(os.str());
			} else {
				ostringstream os;
				os << "/* " << setfill(' ') << setw(3) << ++i << " */ " << str << "  ERROR";
				hh.push_back(os.str());
			}
		}
		operator vector<string>() {
			return hh;
		}
};
char replaceJunk(char c) {
	// Only keep alphas, space (as a delimiter), and '
	return (isalpha(c) || isdigit(c) || c == '(' || c==')' || c == '-' || c=='*' || c=='/' || c=='ö' || c=='.' || c==',' || c=='_') ? c : ' ';
	//~ return (isalpha(c) || isdigit(c) || c == '-' || c=='*' || c=='/' || c=='ö' || c=='.' || c=='_') ? c : ' ';
	// return (isalpha(c)) ? c : ' ';
}

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
					transform(s.begin(), s.end(), s.begin(), replaceJunk);			
					istringstream is(s);
					string word;
					while(is >> word)
						txtlist.insert(word);
				}
			}
		}
		operator set<string>() {
			return txtlist;
		}
};

class num_1 {
		int i;
		vector<string> txt;
	public:
	num_1() { i = 0;}
		void operator()(string& str) {
			ostringstream os1;
			os1 << "/* " << setfill(' ') << setw(3) << ++i << " */ ";
			str = os1.str() + str;
			unsigned int pos = str.find('=');
			if(pos != string::npos ) {	
				ostringstream os2;
				os2 << i;
				str += os2.str();
				pos = str.find('?');
				txt.push_back(str.substr(++pos,100));
			}
		}
		operator vector<string>() {
			return txt;
		}
};



class mn_attr
{
		vector<int> menu_attr;
	public:
		mn_attr() {}
		void operator()(string& str)
		{
			unsigned int pos = str.find("//  ");
			if(pos != string::npos )
			{
				int c = 0;
				if(str[pos+4] =='?')
					c = 0;
				else
				{	
					if(str[pos+4] =='B')      c = 1; 
					else if(str[pos+4] =='C') c = 2;
					unsigned char a = atoi(str.substr(pos+6).c_str());
					a <<= 2;
					c |= a;
					a = atoi(str.substr(pos+8).c_str());
					a <<= 5;
					c |= a;
					//~ a = atoi(str.substr(pos+10).c_str());
					//~ a <<= 6;
					//~ c |= a;
					//~ if(str[pos +  8] == '1') c |= 0x20;
					//~ if(str[pos + 10] == '1') c |= 0x40;
					//~ if(str[pos + 12] == '1') c |= 0x80;
					menu_attr.push_back(c);
					cout << " : " << (int)c << "; " << str << endl;
				}
			}
		}
		operator vector<int>()
		{
			return menu_attr;
		}
};


class mn_attr_txt
{
		vector<string> attr_txt;
		int i;
	public:
		mn_attr_txt() { i = 0;}
		void operator()(string& str)
		{
			unsigned int pos = str.find("//  ");
			if(pos != string::npos )
			{
				int c = 0;
				if(str[pos+4] =='?')
					c = 0;
				else
				{	
					if(str[pos+8] =='1')
					{
						pos = str.find("?");
						if(pos != string::npos)
						{
							string sv = str.substr(pos+1,100);
							pos = sv.find(" ");
							if(pos != string::npos)
							{
								sv = sv.substr(0,pos);
								ostringstream os;
								os << sv <<"_txt=" << i;
								attr_txt.push_back(os.str());	
							}
						}
					}
				}
			}
			++i;
		}
		operator vector<string>()
		{
			return attr_txt;
		}
};

int main (int argcs , char* pArgs[]) {
	ofstream aus, aus_h, list_txt;
    ifstream in;
	int var = 0;
	if(argcs == 2)
		in.open (pArgs[1]);
	else
		in.open("menu.txt");
	
	if(in) {
		aus.open("menu_nr.txt");
		aus_h.open("menu_txt.txt");
		list_txt.open("list_txt.txt");
		cout << "Datei geoeffnet!" << endl;
		string str;
		int anz_t, pos; 
		deque<string> raw_text;
		// copy(istream_iterator<string>(in),istream_iterator<string>(),back_inserter(raw_text));
		while(getline(in,str)) 	{
			raw_text.push_back(str);
		}
		cout << "raw_text.size() = " << raw_text.size() << endl;
		vector<string> tab1 = for_each(raw_text.begin(), raw_text.end(), Tab1(0));
		cout << "$ -> tab1.size() = " << tab1.size() << endl;
		vector<string> tab2 = for_each(raw_text.begin(), raw_text.end(), Tab1(tab1.size(), '&'));
		cout << "& -> tab2.size() = " << tab2.size() << endl;
		vector<string> tab3 = for_each(raw_text.begin(), raw_text.end(), Tab1(tab1.size() + tab2.size(), ']'));
		cout << "] -> tab3.size() = " << tab3.size() << endl;
		// Tabelle mit Default-Werten	
		copy(tab1.begin(),tab1.end(),ostream_iterator<string>(aus,",\n"));
		copy(tab2.begin(),tab2.end(),ostream_iterator<string>(aus,",\n"));
		copy(tab3.begin(),tab3.end(),ostream_iterator<string>(aus,",\n"));
		//-------- einzelne Menueinträge -----
		set<string> list_file = for_each(raw_text.begin(), raw_text.end(), txt_datei());
		copy(list_file.begin(), list_file.end(), ostream_iterator<string>(list_txt,"\n")); //"list_txt.txt"
		list_txt.close();
		//-------------
		vector<string> var_name = for_each(raw_text.begin(), raw_text.end(), dir_var());
		var = var_name.size();
		vector<string> vartxt = for_each(raw_text.begin(), raw_text.end(), var_txt(var,'&'));	// Textvariable
		var += vartxt.size();
		vector<string> vartxt1 = for_each(raw_text.begin(), raw_text.end(), var_txt(var,']'));	// Textvariable
		var += vartxt1.size();
		vector<string> var2 = for_each(raw_text.begin(), raw_text.end(), var_2(var));		// nicht änderbaren Variablen mit Endwerten
		var += var2.size();
		vector<string> var3 = for_each(raw_text.begin(), raw_text.end(), var_3(var));		// nicht änderbaren Variablen ohne Endwerten 
		cout << "var3.size()" << var3.size() << endl;
		//~ copy(var3.begin(), var3.end(), ostream_iterator<string>(cout,"\n"));
		//~ char cc;
		//~ cin >> cc;
		vector<string> en = for_each(var_name.begin(), var_name.end(), enum_file(0));
		var = en.size();
		vector<string> enum2 = for_each(vartxt.begin(), vartxt.end(), enum_file(var));
		var += enum2.size();
		vector<string> enum22 = for_each(vartxt1.begin(), vartxt1.end(), enum_file(var));
		var += enum22.size();
		vector<string> enum3 = for_each(var2.begin(), var2.end(), enum_file(var));
		var += enum3.size();
		vector<string> enum4 = for_each(var3.begin(), var3.end(), enum_file(var));
		
		//~ vector<int> menu_attr = for_each(raw_text.begin(), raw_text.end(), mn_attr());
		//-------------
		??? vector<string> h_file = for_each(raw_text.begin(),raw_text.end(),h_datei());
		copy(h_file.begin(),h_file.end(),ostream_iterator<string>(aus_h,"\n"));
		aus_h.close();
		//---------- Konsolenausgabe ------------	
		copy(en.begin(),en.end(),ostream_iterator<string>(cout,","));
		cout << endl;
		copy(enum2.begin(),enum2.end(),ostream_iterator<string>(cout,";"));
		cout << endl;
		
		copy(raw_text.begin(),raw_text.end(),ostream_iterator<string>(cout,"\n"));
		cout << endl;
		//~ copy(menu_attr.begin(),menu_attr.end(),ostream_iterator<int>(cout,", "));
		cout << "var = " << var << endl; 
		return 0;
		//~ //------------------------------------------------
		vector<int> menu_0;
		vector<int> tab;
		for(unsigned int x = 0; x < raw_text.size(); ++x) {	
			// for(anz_t = 0, pos = 0; str[pos]=='\t'; ++pos)
			for(anz_t = 0, pos = 0; raw_text[x][pos]=='\t'; ++pos)
				++anz_t;
			//~ cout << "anz_t = " << anz_t << endl;
			tab.push_back(anz_t);
			if(anz_t==1)
				menu_0.push_back(x+1);
		}
		vector<string> txt_var = for_each(raw_text.begin(),raw_text.end(), num_1());
		string asd="de.h";
		for_each(raw_text.begin(),raw_text.end(),txt_de(asd))();
		
		raw_text.push_front("char* text[] = {");
		copy(raw_text.begin(),raw_text.end(),ostream_iterator<string>(aus,"\n"));
		aus <<"};" << endl << "int menu_0[] = { ";
		copy(menu_0.begin(),menu_0.end(),ostream_iterator<int>(aus,","));
		aus << "};" << endl;
		copy(menu_0.begin(),menu_0.end(),ostream_iterator<int>(cout,","));
		cout << endl;
		
		//int anf, ende;
		vector<vector<int> > menu;
		vector<int> vec;
		for(unsigned int i=1, y=0,menu_nr=0; i < tab.size(); ++i) {
			if(tab[i] > tab[i-1] || i == tab.size()-1) {
				copy(vec.begin(),vec.end(),ostream_iterator<int>(cout,","));
				cout << endl;
				if(menu_nr) {	
					aus << "int menu_" << menu_nr << "[] = { ";
					copy(vec.begin(),vec.end(),ostream_iterator<int>(aus,","));
					aus << "};" << endl;
				}
				++menu_nr;
				vec.clear();
				vec.push_back(i);
				vec.push_back(i+1);
				menu.push_back(vec);
				y = i;
				while(tab[i] >= tab[y]) {
					if(tab[++i] == tab[y])
						vec.push_back(i+1);
				}
				i = y;
			}
		}
		aus << "unsigned char menu_attr[] = {";
		//copy(menu_attr.begin(),menu_attr.end(),ostream_iterator<int>(aus,","));
		//~ for(unsigned int i = 0; i < menu_attr.size(); ++i) {
			//~ if(i % 20 == 0) {	
				//~ aus << endl;
				//~ cout << endl;
			//~ }
			//~ aus << "0x" << hex << setw(2) << setfill('0') << static_cast<int>(menu_attr[i]) << ", ";
			//~ cout << "0x" << hex << setw(2) << setfill('0') << static_cast<int>(menu_attr[i]) << ", ";
		//~ }
		//~ aus << "};\n" << "menu_attr length = " << dec << menu_attr.size() << endl;
		aus << "enum{\n";
		copy(en.begin(),      en.end(),ostream_iterator<string>(aus,",\n"));
		aus << "// --------------------------------\n";
		copy(enum2.begin(),enum2.end(),ostream_iterator<string>(aus,",\n"));
		aus << "// --------------------------------\n";
		copy(enum3.begin(),enum3.end(),ostream_iterator<string>(aus,",\n"));
		aus << "// --------------------------------\n";
		copy(enum4.begin(),enum4.end(),ostream_iterator<string>(aus,",\n"));
	
		aus << "};\n"<< "Enumlänge: " << en.size() + enum2.size()+ enum3.size()+ enum4.size() << '\n';
		aus << "enum{\n";
		copy(txt_var.begin(),txt_var.end(),ostream_iterator<string>(aus,",\n"));
		aus << "};\n";
		
		vector<string> att_txt = for_each(raw_text.begin(), raw_text.end(), mn_attr_txt());
		aus << "enum{\n";
		copy(att_txt.begin(),att_txt.end(),ostream_iterator<string>(aus,",\n"));
		aus << "};\n"<< "Enumlänge: " << att_txt.size() << '\n';
		
		//vector<string> tab1 = for_each(++en.begin(), en.end(), Tab1());
		//~ vector<string> tab1 = for_each(raw_text.begin(), raw_text.end(), Tab1(0));
		//~ cout << "tab1.size()" << tab1.size() << endl;
		//~ vector<string> tab2 = for_each(raw_text.begin(), raw_text.end(), Tab1(tab1.size(), '&'));
		//~ cout << "tab2.size()" << tab1.size() << endl;
		//~ vector<string> tab3 = for_each(raw_text.begin(), raw_text.end(), Tab1(tab1.size() + tab2.size(), '!'));
		//~ cout << "tab3.size()" << tab1.size() << endl;
		//~ 
		//~ copy(tab1.begin(),tab1.end(),ostream_iterator<string>(aus,",\n"));
		//~ copy(tab2.begin(),tab2.end(),ostream_iterator<string>(aus,",\n"));
		//~ copy(tab3.begin(),tab3.end(),ostream_iterator<string>(aus,",\n"));

		//~ vector<string> tab2 = for_each(vartxt.begin(), vartxt.end(), Tab1(var_name.size(), '&'));
		//~ copy(tab2.begin(),tab2.end(),ostream_iterator<string>(aus,",\n"));
		//~ cout << "tab2.size()" << tab1.size() << endl;

		cout << "var = " << var << endl;
	}
}

