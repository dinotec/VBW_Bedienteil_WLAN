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
int main (int argcs , char* pArgs[]) {
	ofstream aus;
	ifstream in;
	aus.open("num_num.txt");
	if(argcs == 2)
		in.open (pArgs[1]);
	else
		in.open("num.txt");
	
	ostringstream os;
	int i = 0, vr = 0;
	unsigned int pos;
	string str;
	
	while(getline(in,str)) {
		pos = str.find("#");
		if(pos != string::npos){
			string s = str.substr(++pos,10);
			cout << s << endl;
			istringstream in(s);
			in >> vr;
			if(vr > 81 && vr < 300){
				unsigned int pos2;
				pos2 = s.find(" ");
				if(pos2 == string::npos)
					pos2 = s.find("\"");
				if(pos2 != string::npos) {
					ostringstream rpl;
					rpl << ++vr;
					str.replace(pos, pos2, rpl.str());
				}
				
			}
		}
		
		os << "/*" << setw(3) << setfill(' ') << i++ << "*/ " << str << endl;
	}

	cout << os.str();
	aus << os.str();
	aus.close();
	in.close();
}
