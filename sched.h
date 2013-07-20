#include "head.h"
#include "present.h"
#include "mth.h"
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

    vector<int> tasktimetable;
    int taskttpnt;

    double history_pressure;
    double history_sensitive;

    double systartime;

    sem_t pmtx;
    sem_t arrmtx;
    sem_t wfttr;

    Sched(int _K, int _P);
    double getsystime();
    double gettime();

    int addtask(string name, string cmd, string datafile);
    void loadtasklist(string tasklist);
    void loadbenchmark();

//----footprint----
    double try_getfilltime(int u);
    double try_gettotalmiss(int u);
    double getfilltime(vector<int> &ids);
    double gettotalmiss(vector<int> &ids);
//-----------------

    void taskfinish(int k);
    static void* _trypush(void* args);
    void trykeep();
    void trypush();
    void tryrun();
    void runtask(int u);
    static void* runthread(void *arg);
    static int getpid(string cmd);
    void pausetask(int id);
    void fgtask(int id);
    vector<int> gettimetable(vector<int> list);
    static void* _timeinterrupt(void* args);
    void timeinterrupt();
};
#endif
