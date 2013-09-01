#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <vector>
using namespace std;
int main(){
	ifstream fin("order_bak");
	vector<string> s;
	string v;
	while (fin>>v){
		s.push_back(v);
	}
	for (int i = 0; i<5; i++){
		char c[100];
		sprintf(c, "order_rand%d", i);
		ofstream fout(c);
		random_shuffle(s.begin(), s.end());
		for (int i = 0; i<s.size(); i++){
			fout<<s[i]<<endl;
		}
		fout.close();
	}
	return 0;
}
