#include "head.h"
#include "present.h"
#ifndef SCHED_H
#define SCHED_H
class Sched{
public:
    int K;//the tasks that we could see
    int P;//the prosessors that we could  run
    int cachesize;
    vector<int> keep;//K tasks
    vector<int> running;
    vector<Present> task;//all tasks
    int nexttaskid;

    double history_pressure;
    double history_sensitive;

    sem_t pmtx;
    sem_t arrmtx;

    Sched(int _K, int _P);
    double try_getpressure(int u);
    double try_getmissrate(int u);
    int addtask(string name, string cmd, string datafile);
    void taskfinish(int k);
    void trypush();
    void tryrun();
    static void* realrun(void *arg);
};
#endif
