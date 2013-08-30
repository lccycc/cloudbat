#include "include/head.h"
#include "include/mth.h"
#include "present/present.h"
#include "bubble/bubble.h"
#ifndef SCHED_H
#define SCHED_H
class Sched{
public:
#define NOPREDICTION 0
#define FOOTPRINTMETHOD 1
#define REUSEDSTMETHOD 2
#define BUBBLEMETHOD 3
#define FREERUN 4
    int method;
    int K;//the tasks that we could see
    int P;//the prosessors that we could  run
    int cachesize;
	string speccmd;
	ofstream ferr;
	vector<int> cpumask;

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

    Sched(int _KPP, int _P);
    double gettime();

    int addtask(string name, string cmd, string datafile);
    void loadtasklist(string tasklist);
    void loadbenchmark();

//----footprint----
    double getfpfilltime(vector<int> &ids);
    double getfpworkload(vector<int> &ids);
//----bubble------
    Bubble bubble;
    double getbbworkload(vector<int> &ids);
//----reusedst----
    double getrdfilltime(vector<int> &ids);
	double getrdworkload(vector<int> &ids);
//-----------------
    double try_getworkload(int u);
    double getworkload(vector<int> &ids);
    double getsingleworkload(vector<int> &ids, int spe);
    void printall();
    void printcputime();

    void taskfinish(int k);
    static void* _trypush(void* args);
    void trykeep();
    void trypush();
    void tryrun();
    void runtask(int u);
    static void* runthread(void *arg);
    void pausetask(int id);
    void fgtask(int id);
	double printfpmiss(vector<int> list, int i);
    vector<int> gettimetable(vector<int> list);
    static void* _timeinterrupt(void* args);
    void timeinterrupt();
};
#endif
