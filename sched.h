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

    double systartime;

    sem_t pmtx;
    sem_t arrmtx;
    sem_t wfttr;

    Sched(int _K, int _P);
    double getsystime();
    double gettime();

    void loadtasklist(string tasklist);
    double try_getpressure(int u);
    double try_getmissrate(int u);
    int addtask(string name, string cmd, string datafile);
    void taskfinish(int k);
    void trypush();
    void tryrun();
    static void* realrun(void *arg);
    static int getpid(string cmd);
    void pausetask(int id);
    void fgtask(int id);
};
#endif
