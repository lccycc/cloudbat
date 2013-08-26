#ifndef PRESENT_H
#define PRESENT_H
#include "include/head.h"
const uint32_t SUBLOG_BITS = 8;
const uint32_t MAX_WINDOW = (65-SUBLOG_BITS)*(1<<SUBLOG_BITS);
const int WindowSub = 10000;
const int MaxCache = 631100;
class Present{
public:
	int debug;

    string name;
    string dir;
    string cmd;
    pthread_t thread;
    int id;
    int pid;
    double cputime, lastrunt;

    double stdruntime;

    Present();
    Present(string _name, string _cmd, int _id);
//----footprint----
    double ft2c_c[MAX_WINDOW];
    double mr[MAX_WINDOW];
    LL N, M;
    int cnt;
    void footprint_init(string filename);
    double missnum(double filltime);
    double fillcache(double filltime);
//----bubble------
    int plevel;
    map<int, double> delay[5];
//----reusedst----
	int rdmrcnt;
	double rdmr[MAX_WINDOW];
	int rdftcnt;
	double rdft_c[MAX_WINDOW];
	void reusedst_init(string filename);
	double rdmissnum(double filltime);
    double rdfillcache(double filltime);
};
#endif
