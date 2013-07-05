#ifndef SCHED_CPP
#define SCHED_CPP
#include "sched.h"
Sched::Sched(int _K, int _P){

    cachesize = 4096;

    K = _K;
    P = _P;
    nexttaskid = 0;
    history_pressure = 0;
    history_sensitive = 0;

    systartime = getsystime();

    sem_init(&pmtx, 0, P);
    sem_init(&arrmtx, 0, 1);
    sem_init(&wfttr, 0, 0);
}
double Sched::getsystime(){
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec/1000000.0;
}
double Sched::gettime(){
    return getsystime()-systartime;
}
void Sched::loadtasklist(string tasklist){
    ifstream fin(tasklist.c_str());
    string cmd;
    string datafile;
    while (std::getline(fin, cmd)){
        std::getline(fin, datafile);
        addtask(cmd, cmd, datafile);
    }
    fin.close();
}
double Sched::try_getpressure(int u){
    double ft = 0;
    for (unsigned i = 0; i<running.size(); i++){
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
    for (unsigned i = 0; i<running.size(); i++){
        mr += task[running[i]].sensitive(ft);
    }
    mr += task[u].sensitive(ft);
    return mr;
}
double Sched::getworkload(vector<int> &ids){
    double ft = 0;
    for (unsinged i = 0; i<ids.size(); i++){
        ft += 1/task[ids[i]].pressure(cachesize);
    }
    assert(ft>0);
    ft = 1/ft;
    doublem mr = 0;
    for (unsigned i = 0; i<ids.size(); i++){
        mr += task[ids[i]].sensitive(ft);
    }
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
    cerr<<"           "<<k<<" finish, "<<gettime()<<endl;
    sem_post(&arrmtx);
    sem_post(&pmtx);
}
void Sched::trypush(){
    do{
        sem_wait(&pmtx);
        sem_wait(&arrmtx);
        while (keep.size() < K && nexttaskid < task.size()){
            int id = nexttaskid++;
            keep.push_back(id);
            cerr<<id<<" push into keep"<<endl;
        }
        if (keep.size()){
            tryrun();
        }
        sem_post(&arrmtx);
    }while (keep.size() || nexttaskid < task.size());
    for (unsigned i = 0; i<task.size(); i++){
        pthread_join(task[i].thread, NULL);
    }
}
void Sched::tryrun(){
    if (keep.empty()){
        return;
    }
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
    //for debug
    /*
    get = keep;
    int fd = get[0];
    */

    for (vector<int>::iterator it = keep.begin(); it!=keep.end(); it++){
        if (*it == fd){
            keep.erase(it);
            break;
        }
    }
    running.push_back(fd);
    cerr<<fd<<" prepare to run, cmd = "<<task[fd].cmd<<endl;

    pthread_create(&task[fd].thread, NULL, realrun, this);
    sem_wait(&wfttr);
}
void* Sched::realrun(void *arg){
    Sched &s = *((Sched*)arg);
    int id = s.running[s.running.size()-1];
    s.task[id].pid = getpid(s.task[id].cmd);
    sem_post(&s.wfttr);
    system(s.task[id].cmd.c_str());
    s.taskfinish(id);
}
int Sched::getpid(string cmd){
    cmd = " " + cmd + "\n";
    FILE *fp = popen("ps -ao pid,command", "r");
    assert(fp);
    int pid;
    char pscmd[500];
    while (fscanf(fp, "%d", &pid)!=EOF){
        fgets(pscmd, 488, fp);
        if (strcmp(cmd.c_str(), pscmd) == 0){
            break;
        }
    }
    fclose(fp);
    return pid;
}
void Sched::pausetask(int id){
    kill(task[id].pid, 19);
}
void Sched::fgtask(int id){
    kill(task[id].pid, 18);
}


vector<vector<int> > Sched::timetable(vector<int> list){
    int K = list.size();
    int GCD = gcd(K, P);
    int D = P/GCD;
    int psperK =  K/P;
    vector<pair<LL, double> > lev0;
    vector<LL> que;
    map<LL, double> fus;
    map<LL, int> last;
    for (i = 0; i<(1<<K); i++){
        if (count1bit(i) != P){
            continue;
        }
        vector<int> ids;
        for (int j = 0; j<K; j++){
            if (i & (1<<j)){
                ids.push_back(list[j]);
            }
        }
        double mr = getworkload(ids);
        lev0.push_back(make_pair(i, mr));
        que.push_back(i);
        fus[i] = mr;
    }
    for (unsigned head = 0; head < que.size(); head++){
        int u = que[head];
        int uy = fus[u];
        for (unsigned i = 0; i<lev0.size(); i++){
            int x = lev0[i].first;
            if (x & u){
                continue;
            }
            double y = lev0[i].second;
            int v = (x | u);
            if (fus.find(v) == fus.end()){
                fus[v] = y;
                last[v] = x;
                que.push_back(v);
            }else{
                double vy = fus[v];
                if (vy > uy + y){
                    fus[v] = uy + y;
                    last[v] = x;
                }
            }
        }
    }
    int R = K%P;
    int RD = R/GCD:



#endif
