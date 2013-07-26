#ifndef PRESENT_H
#define PRESENT_H
#include "include/head.h"
#include "bubble/bubble.h"
const uint32_t SUBLOG_BITS = 8;
const uint32_t MAX_WINDOW = (65-SUBLOG_BITS)*(1<<SUBLOG_BITS);
const int WindowSub = 10000;
const int MaxCache = 131100;
class Present{
public:
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
    double ft2c_c[WindowSub];
    double mr[WindowSub];
    LL N, M;
    int cnt;
    void init(string filename);
    double missnum(double filltime);
    double fillcache(double filltime);
//----bubble------
    double plevel;
    double delay[MAX_BUBBLE_LEVEL];
};
#endif
