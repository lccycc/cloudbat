#include "head.h"
#ifndef PRESENT_H
#define PRESENT_H
const int WindowSub = 2500;
const int MaxCache = 131100;
class Present{
public:
    string name;
    string cmd;
    pthread_t thread;
    int id;
    double mr[MaxCache];//miss rate
    double ft[MaxCache];//fill time
    LL N, M;
    double total_time;
    int cnt;
    Present();
    Present(string _name, string _cmd, int _id);
    void init(string filename);
    double sensitive(double filltime);
    double pressure(int cachesize);
};
#endif
