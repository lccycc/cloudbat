#ifndef SCHED_CPP
#define SCHED_CPP
#include "sched.h"
Sched::Sched(int _K, int _P){
    K = _K;
    P = _P;
    nexttaskid = 0;
    history_pressure = 0;
    history_sensitive = 0;

    sem_init(&pmtx, 0, P);
    sem_init(&arrmtx, 0, 1);
}
double Sched::try_getpressure(int u){
    double ft = 0;
    for (unsigned i = 0; i<keep.size(); i++){
        ft += 1/task[running[i]].pressure(cachesize);
    }
    ft += 1/task[u].pressure(cachesize);
    assert(ft>0);
    ft = 1/ft;
    return ft;
}
double Sched::try_getmissrate(int u){
    double ft = try_getpressure(u);
    double mr = 0;
    for (unsigned i = 0; i<keep.size(); i++){
        mr += task[i].sensitive(ft);
    }
    mr += task[u].sensitive(ft);
    return mr;
}
int Sched::addtask(string name, string cmd, string datafile){
    int id = task.size();
    Present p(name, cmd, id);
    p.init(datafile);
    task.push_back(p);
    return id;
}
void Sched::taskfinish(int k){
    sem_wait(&arrmtx);
    for (vector<int>::iterator it = running.begin();
                it != running.end(); it++){
        if (*it == k){
            running.erase(it);
            break;
        }
    }
    sem_post(&arrmtx);
    trypush();
}
void Sched::trypush(){
    do{
        sem_wait(&pmtx);
        sem_wait(&arrmtx);
        while (keep.size() < K && nexttaskid < task.size()){
            int id = nexttaskid++;
            keep.push_back(id);
        }
        if (keep.size()){
            tryrun();
        }
        sem_post(&arrmtx);
    }while (keep.size() || nexttaskid < task.size());
}
void Sched::tryrun(){
    vector<int> get;
    double runningpres = 0;
    for (unsigned i = 0; i<running.size(); i++){
        runningpres += task[running[i]].pressure(cachesize);
    }
    for (unsigned i = 0; i<keep.size(); i++){
        if ((task[keep[i]].pressure(cachesize)+runningpres)
                /(running.size()+1)  > history_pressure){
            get.push_back(keep[i]);
        }
    }
    if (get.empty()){
        for (unsigned i = 0; i<keep.size(); i++){
            if (task[keep[i]].sensitive(try_getpressure(keep[i]))
                    > history_sensitive){
                get.push_back(keep[i]);
            }
        }
    }
    if (get.empty()){
        get = keep;
    }
    double minmiss = 999999999;
    int fd = -1;
    for (unsigned i = 0; i< get.size(); i++){
        double mr = try_getmissrate(get[i]);
        if (fd == -1 || minmiss > mr){
            fd = get[i];
            minmiss = mr;
        }
    }
    double pres = task[fd].pressure(cachesize);
    double sens = task[fd].sensitive(try_getpressure(fd));
    history_pressure = (history_pressure * (P+P-1)+pres)/(P+P);
    history_sensitive = (history_sensitive * (P+P-1)+sens)/(P+P);

    for (vector<int>::iterator it = keep.begin(); it!=keep.end(); it++){
        if (*it == fd){
            keep.erase(it);
            break;
        }
    }
    running.push_back(fd);
    pthread_create(&task[fd].thread, NULL, realrun, this);
}
void* Sched::realrun(void *arg){
    Sched &s = *((Sched*)arg);
    int id = s.running[s.running.size()-1];
    system(s.task[id].cmd.c_str());
    s.taskfinish(id);
}
#endif
