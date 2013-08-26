#ifndef MTH_H
#define MTH_H
#include "head.h"
LL gcd(LL a, LL b);
int count1bit(LL a);
void init(LL* mi, int base, int len);
LL getpos(LL a, LL* mi, int pos);
uLL tryadd(uLL a, int d, int K, int D, int cbit, bool sgn);
double getsystime();
int getpid(string cmd);
void pausepid(int pid);
void killpid(int pid);
void fgpid(int pid);
int runandgetpid(string dir, string cmd, int core);
string getfirstword(string s);
string getbits(int i, int K);
#endif
