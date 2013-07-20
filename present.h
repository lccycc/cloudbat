#include "head.h"
#ifndef PRESENT_H
#define PRESENT_H
const  uint32_t              SUBLOG_BITS = 8;
const  uint32_t              MAX_WINDOW = (65-SUBLOG_BITS)*(1<<SUBLOG_BITS);
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
//    double mr[MaxCache];//miss rate
//    double ft[MaxCache];//fill time
    double ft2c_c[WindowSub];
    double mr[WindowSub];

    LL N, M;
    double total_time;
    int cnt;
    Present();
    Present(string _name, string _cmd, int _id);
    void init(string filename);
    double missnum(double filltime);
    double fillcache(double filltime);
};
#endif
