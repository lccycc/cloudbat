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
    for (unsigned i = 0; i<ids.size(); i++){
        ft += 1/task[ids[i]].pressure(cachesize);
    }
    assert(ft>0);
    ft = 1/ft;
    double mr = 0;
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

    runtask(fd);
}
void Sched::runtask(int u){
    for (vector<int>::iterator it = keep.begin(); it!=keep.end(); it++){
        if (*it == u){
            keep.erase(it);
            break;
        }
    }
    running.push_back(u);
    cerr<<u<<" prepare to run, cmd = "<<task[u].cmd<<endl;
    if (task[u].pid == -1){
        void* arg[2];
        arg[0] = this;
        arg[1] = &u;
        pthread_create(&task[u].thread, NULL, runthread, arg);
        sem_wait(&wfttr);
        while ((task[u].pid = getpid(task[u].cmd))==-1){
            usleep(10000);
        }
    }else{
        //this task is pause
        fgtask(u);
    }
}


void* Sched::runthread(void *arg){
    Sched &s = *((Sched*)((void**)arg)[0]);
    int id = *((int*)((void**)arg)[1]);
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
    bool findit = false;
    while (fscanf(fp, "%d", &pid)!=EOF){
        fgets(pscmd, 488, fp);
        if (strcmp(cmd.c_str(), pscmd) == 0){
            findit = true;
            break;
        }
    }
    fclose(fp);
    if (!findit){
        return -1;
    }
    return pid;
}
void Sched::pausetask(int id){
    kill(task[id].pid, 19);
}
void Sched::fgtask(int id){
    kill(task[id].pid, 18);
}


vector<int> Sched::gettimetable(vector<int> list){
    int K = list.size();
    assert(K<31);
    assert(K>=P);
    int GCD = gcd(K, P);
    cerr<<"K = "<<K<<endl;
    int psperK =  K/P;
    vector<pair<int, double> > lev0;
    vector<int> que;
    map<int, double> fus;
    map<int, int> last;
    vector<pair<int, double> > levF;
    for (int i = 0; i<(1<<K); i++){
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
    }
    que.push_back(0);
    fus[0] = 0;
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
                fus[v] = uy + y;
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
    int RD = R/GCD;
    cerr<<"RD = "<<RD<<endl;
    LL maskK = (1<<K)-1;
    for (int i = que.size()-1; i>=0 && count1bit(que[i]) == K-R; i--){
        levF.push_back(make_pair(que[i]^maskK, fus[i]));
    }
    cerr<<"levF size = "<<levF.size()<<endl;
    vector<LL> queRD;
    map<LL, double> fusRD;
    map<LL, int> lastRD;
    queRD.push_back(0);
    fusRD[0] = 0;
    for (unsigned head = 0; head < queRD.size(); head++){
        LL u = queRD[head];
        double uy = fusRD[u];
        for (unsigned i = 0; i<levF.size(); i++){
            int x = levF[i].first;
            double y = levF[i].second;
            LL v = tryadd(u, x, K, RD);
            if (v == 0){
                continue;
            }
            if (fusRD.find(v) == fusRD.end()){
                fusRD[v] = y;
                lastRD[v] = x;
                queRD.push_back(v);
            }else{
                double vy = fusRD[v];
                if (vy > uy + y){
                    fusRD[v] = uy + y;
                    lastRD[v] = x;
                }
            }
        }
    }
    cerr<<"queRD size = "<<queRD.size()<<endl;
    //now find solution
    LL finalx = 1;
    for (int i = 0; i<K; i++){
        finalx*=(RD+1);
    }
    finalx--;
    assert(finalx == queRD[queRD.size()-1]);
    vector<int> seq;
    vector<int> midseq;
    while (finalx != 0){
        int x = lastRD[finalx];
        finalx = tryadd(finalx, x, K, RD, -1);

        x ^= maskK;
        midseq.push_back(x);
    }
    if (midseq.size() == 0){
        midseq.push_back(maskK);
    }
    for (unsigned i = 0; i<midseq.size(); i++){
        int x = midseq[i];
        while (x != 0){
            int dx = last[x];
            assert(dx);
            assert((x & dx) == dx);
            seq.push_back(dx);
            x ^= dx;
        }
    }
    cerr<<"---timetable size: "<<seq.size()<<endl;
    for (signed i = 0; i<seq.size(); i++){
        for (int j = 0; j<K; j++){
            if (seq[i] & (1<<j)){
                cerr<<'1';
            }else{
                cerr<<'0';
            }
        }
        cerr<<endl;
    }
    cerr<<"---timetable end---"<<endl;
    return seq;
}
void* Sched::_timeinterrupt(void* args){
    Sched &s = *((Sched*)args);
    s.timeinterrupt();
    return NULL;
}
void Sched::timeinterrupt(){
    while (1){
        sem_wait(&arrmtx);
        if (keep.size() == 0){
            sem_post(&arrmtx);
            break;
        }
        //cut all the running thread
        for (unsigned i = 0; i<running.size(); i++){
            pausetask(running[i]);
            keep.push_back(running[i]);
        }
        running.clear();
        sort(keep.begin(), keep.end());

        if (taskttpnt == -1){
            tasktimetable = gettimetable(keep);
            assert(tasktimetable.size());
            taskttpnt = 0;
        }
        int bat = tasktimetable[taskttpnt++];
        if (taskttpnt == tasktimetable.size()){
                taskttpnt = 0;
        }
        vector<int> wtr;
        for (int i = 0; i<K; i++){
            if (bat & (1<<i)){
                wtr.push_back(keep[i]);
            }
        }
        assert(wtr.size() == P);
        for (int i = 0; i<wtr.size(); i++){
            runtask(wtr[i]);
        }
        sem_post(&arrmtx);
    }
}
#endif
