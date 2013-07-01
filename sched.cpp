#ifndef SCHED_CPP
#define SCHED_CPP
#include "sched.h"
Sched::Sched(int _K, int _P){
    K = _K;
    P = _P;
    nexttaskid = 0;
    history_pressure = 0;
    history_sensitive = 0;
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
    double ft = try_getpresure(u);
    double mr = 0;
    for (unsigned i = 0; i<keep.size(); i++){
        mr += task[i].sensitive(ft);
    }
    mr += task[u].sensitive(ft);
    return mr;
}
int Sched::addtask(string name, string cmd, string datafile){
    int id = task.size();
    Present p(name, cmd);
    p.init(datafile);
    task.push_back(p);
    trypush();
    return id;
}
void Sched::taskfinish(int k){
    for (vector<int>::iterator it = running.begin();
                it != running.end(); it++){
        if (*it == k){
            running.erase(it);
            break;
        }
    }
    trypush();
}
void Sched::trypush(){
    bool nextloop = true;
    while (nextloop){
        nextloop = false;
        while (keep.size() < K && nexttaskid < task.size()){
            int id = nexttaskid++;
            keep.push_back(id);
            nextloop = true;
        }
        while (running.size() < P && keep.size()){
            tryrun();
            nextloop = true;
        }
    }
}
void Sched::tryrun(){
    vector<int> get;
    for (unsigned i = 0; i<keep.size(); i++){
        if (task[keep[i]].pressure(cachesize) > history_pressure){
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
        if (fd == -1 || minmiss > ms){
            fd = get[i];
            minmiss = ms;
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
    //todo: run it in terminal
}
#endif
