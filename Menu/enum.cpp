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
int main (int argcs , char* pArgs[])
{
	ofstream aus;
	vector<string> vec;
	aus.open("enumerator.h");
	ostringstream os;
	os << "enum" << endl << "{" << endl;
	for(int i=0; i < 224; ++i)
	{
		os << "reg" << setw(3) << setfill('0') << i << ", // " << i << endl;
	}
	os << "};" << endl;
	cout << os.str();
	aus << os.str();
	aus.close();
}
