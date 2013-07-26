#ifndef SCHED_CPP
#define SCHED_CPP
#include "sched.h"
ofstream ferr("./log/sched.msg");
Sched::Sched(int _KPP, int _P){

    cachesize = (1<<17);

    K = _KPP-_P;
    P = _P;
    nexttaskid = 0;

    taskttpnt = -1;

    history_pressure = 0;
    history_sensitive = 0;

    systartime = getsystime();

    sem_init(&pmtx, 0, P);
    sem_init(&arrmtx, 0, 1);
    sem_init(&wfttr, 0, 0);
}
double Sched::gettime(){
    return getsystime()-systartime;
}
void Sched::loadtasklist(string tasklist){
    ifstream fin(tasklist.c_str());
    string cmd;
    string datafile;
    int cnt = 0;
    while (std::getline(fin, cmd)){
        std::getline(fin, datafile);
        addtask(cmd, cmd, datafile);
        //if (++cnt >=4){ break;}
    }
    fin.close();
}
void Sched::loadbenchmark(){
    if (method == FOOTPRINTMETHOD){
        ferr<<"Footprint Schedule!"<<endl;
    }else
    if (method == BUBBLEMETHOD){
        ferr<<"BUBBLE Schedule!"<<endl;
    }

    string name, dir, cmd;

    map<string, double> tc;
    map<string, string> di;
    map<string, string> cm;

    ifstream tin("./benchmark/timecost");
    double timecost;
    while (tin>>name>>timecost){
        tc[name] = timecost;
    }
    tin.close();

    ifstream fin("./benchmark/speccmd.cmd");
    while (std::getline(fin, name)){
        cout<<name<<endl;
        std::getline(fin, dir);
        std::getline(fin, cmd);
        di[name] = dir;
        cm[name] = cmd;
    }
    fin.close();

    ifstream oin("./benchmark/order");
    while (oin>>name){
        dir = di[name];
        cmd = cm[name];
        int id = task.size();
        Present p(name, cmd, id);
        p.dir = dir;
        p.stdruntime = tc[name];
        task.push_back(p);
    }
    oin.close();
    ferr<<"tasksize = "<<task.size()<<endl;

    if (method == FOOTPRINTMETHOD){
        for (unsigned i = 0; i<task.size(); i++){
            task[i].footprint_init("./benchmark/footprint/"+task[i].name+".dat");
        }
    }else
    if (method == BUBBLEMETHOD){
        bubble.init();
        ifstream bsin("./benchmark/bubble/sensitivecurve.dat");
        string name;
        int p, lev;
        double delay;
        while (bsin>>name>>p>>lev>>delay){
            for (unsigned i = 0; i<task.size(); i++){
                if (task[i].name.compare(name) == 0){
                    task[i].delay[p][lev] = delay;
                }
            }
        }
        bsin.close();
        ifstream bpin("./benchmark/bubble/benchmarkpplevel.dat");
        double pplev;
        while (bpin>>name>>pplev){
            for (unsigned i = 0; i<task.size(); i++){
                if (task[i].name.compare(name) == 0){
                    task[i].plevel = bubble.findcloseplevel(pplev);
                }
            }
        }
        bpin.close();
    }
}

double Sched::try_getworkload(int u){
    vector<int> t = running;
    t.push_back(u);
    return getworkload(t);
}
double Sched::getfpfilltime(vector<int> &ids){
    const double MaxTime = 200.0 * 1000000000.0;
    double l = 1, r = MaxTime;
    double ttc = 0;
    while (l+1<r){
        double md = (l+r)/2;
        ttc = 0;
        for (unsigned i = 0; i<ids.size(); i++){
            ttc +=task[ids[i]].fillcache(md);
        }
        if (ttc < cachesize){
            l = md;
        }else{
            r = md;
        }
    }
    return l;
}
double Sched::getfpworkload(vector<int> &ids){
    double ft = getfpfilltime(ids);
    double mn = 0;
    for (unsigned i = 0; i<ids.size(); i++){
        mn += task[ids[i]].missnum(ft);
    }
    return mn;
}
double Sched::getsingleworkload(vector<int> &ids, int spe){
    double ft = getfpfilltime(ids);
    return task[spe].missnum(ft);
}
void Sched::printall(){
    vector<double> swl;
    for (unsigned i = 0; i<task.size();i++){
        vector<int> ids;
        ids.push_back(i);
        swl.push_back(getsingleworkload(ids, i));
    }
    for (int i = 0; i<task.size(); i++){
        ferr<<"\t"<<task[i].name;
    }
    ferr<<endl;
    for (unsigned i = 0; i<task.size(); i++){
        ferr<<task[i].name;
        for (unsigned j = 0; j<task.size(); j++){
            vector<int> ids;
            ids.push_back(i);
            ids.push_back(j);
            double r = getsingleworkload(ids, i);
            ferr<<"\t"<<r;
        }
        ferr<<endl;
    }
}
void Sched::printcputime(){
    for (unsigned i = 0; i<task.size(); i++){
        double deta = task[i].cputime - task[i].stdruntime;
        ferr<<task[i].name<<"\t"<<task[i].cputime<<"\t"<<deta/task[i].stdruntime<<endl;
    }
}
double Sched::getbbworkload(vector<int> &ids){
    double tot = 0;
    for (unsigned i = 0; i<ids.size(); i++){
        vector<int> tp;
        for (unsigned j = 0; j<ids.size(); j++){
            if (j != i){
                tp.push_back(task[ids[j]].plevel);
            }
        }
        tot += bubble.lookup(task[ids[i]], tp);
    }
    return tot;
}
double Sched::getworkload(vector<int> &ids){
    if (method == FOOTPRINTMETHOD){
        return getfpworkload(ids);
    }else
    if (method == BUBBLEMETHOD){
        return getbbworkload(ids);
    }
}


int Sched::addtask(string name, string cmd, string datafile){
    int id = task.size();
    Present p(name, cmd, id);
    p.footprint_init(datafile);
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
    ferr<<"           "<<k<<" finish, "<<gettime()<<endl;
    taskttpnt = -1;
    sem_post(&arrmtx);
    sem_post(&pmtx);
}
void* Sched::_trypush(void* args){
    Sched &s = *((Sched*)args);
    s.trypush();
    ferr<<"Thread: _trypush end"<<endl;
    return NULL;
}
void Sched::trypush(){
    do{
        sem_wait(&pmtx);
        sem_wait(&arrmtx);
        trykeep();
        if (keep.size()){
            tryrun();
        }
        trykeep();
        sem_post(&arrmtx);
    }while (keep.size() || nexttaskid < task.size());
    for (unsigned i = 0; i<task.size(); i++){
        pthread_join(task[i].thread, NULL);
    }
}
void Sched::trykeep(){
    while (keep.size() < K && nexttaskid < task.size()){
        int id = nexttaskid++;
        keep.push_back(id);
        taskttpnt = -1;
        ferr<<id<<" push into keep"<<endl;
    }
}
void Sched::tryrun(){
    if (keep.empty()){
        return;
    }
    vector<int> get;
    /*
    double runningpres = 0;
    for (unsigned i = 0; i<running.size(); i++){
        runningpres += task[running[i]].filltime(cachesize);
    }
    for (unsigned i = 0; i<keep.size(); i++){
        if ((task[keep[i]].filltime(cachesize)+runningpres)
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
    */
    if (get.empty()){
        get = keep;
    }
    double minmiss = 999999999;
    int fd = -1;
    for (unsigned i = 0; i< get.size(); i++){
        double mr = try_getworkload(get[i]);
        if (fd == -1 || minmiss < mr){
            fd = get[i];
            minmiss = mr;
        }
    }
    /*
    double pres = task[fd].pressure(cachesize);
    double sens = task[fd].sensitive(try_getpressure(fd));
    history_pressure = (history_pressure * (P+P-1)+pres)/(P+P);
    history_sensitive = (history_sensitive * (P+P-1)+sens)/(P+P);
    */
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
    if (task[u].pid == -1){
        ferr<<u<<" prepare to run, cmd = "<<task[u].cmd<<endl;
        void* arg[2];
        arg[0] = this;
        arg[1] = &u;
        pthread_create(&task[u].thread, NULL, runthread, arg);
        sem_wait(&wfttr);

        int getpidcnt = 1000;
        int cutpnt = task[u].cmd.find(" ");
        if (cutpnt == std::string::npos){
            cutpnt = task[u].cmd.size();
        }
        string cutcmd = task[u].cmd.substr(0, cutpnt);
        ferr<<cutcmd<<endl;
        while ((task[u].pid = getpid(cutcmd))==-1
                    && getpidcnt--){
            usleep(10000);
        }
        assert(task[u].pid>=0);
    }else{
        //this task is pause
        fgtask(u);
    }
}


void* Sched::runthread(void *arg){
    Sched &s = *((Sched*)((void**)arg)[0]);
    int id = *((int*)((void**)arg)[1]);
    sem_post(&s.wfttr);

    chdir(s.task[id].dir.c_str());

    s.task[id].lastrunt = getsystime();
    system(s.task[id].cmd.c_str());
    s.task[id].cputime += getsystime() - s.task[id].lastrunt;

    s.taskfinish(id);
}
void Sched::pausetask(int id){
    pausepid(task[id].pid);
    task[id].cputime += getsystime() - task[id].lastrunt;
    //ferr<<"task pause, id = "<<id<<", pid = "<<task[id].pid<<endl;
}
void Sched::fgtask(int id){
    task[id].lastrunt = getsystime();
    fgpid(task[id].pid);
    //ferr<<"task fg, id = "<<id<<", pid = "<<task[id].pid<<endl;
}


vector<int> Sched::gettimetable(vector<int> list){
    typedef unordered_map<int, pair<double, int> > FUSTYPE;
    typedef unordered_map<uLL, pair<double, int> > FUSRDTYPE;

    int K = list.size();
    assert(K<31);
    assert(K>=P);
    ferr<<"K = "<<K<<", P = "<<P<<endl;
    int psperK =  K/P;
    vector<pair<int, double> > lev0;

    vector<int> singleworkload;
    for (int i = 0; i<K; i++){
        vector<int> ids;
        ids.push_back(list[i]);
        double r = getworkload(ids);
        singleworkload.push_back(r);
    }
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
        for (int j = 0; j<K; j++){
            /*
            if (i&(1<<j)){
                ferr<<'1';
            }else{
                ferr<<'0';
            }
            */
            if (i&(1<<j)) ferr<<j;
        }
        double testdeta = mr;
        for (int j = 0; j<K; j++) if (i&(1<<j)){
            testdeta -= singleworkload[j];
        }
        ferr<<"  "<<mr<<" "<<testdeta<<endl;
    }
    vector<int> que;
    FUSTYPE fus;
    que.push_back(0);
    fus[0] = make_pair(0, 0);
    for (unsigned head = 0; head < que.size(); head++){
        int u = que[head];
        double uy = fus[u].first;
        for (unsigned i = 0; i<lev0.size(); i++){
            int x = lev0[i].first;
            if (x & u){
                continue;
            }
            double y = lev0[i].second;
            int v = (x | u);
            FUSTYPE::iterator ft;
            if ((ft = fus.find(v)) == fus.end()){
                fus[v] = make_pair(uy + y, x);
                que.push_back(v);
            }else{
                double vy = ft->second.first;
                if (vy > uy + y){
                    ft->second.first = uy + y;
                    ft->second.second = x;
                }
            }
        }
    }
    int R = K%P;
    int RD = R/gcd(R, K);
    ferr<<"RD = "<<RD<<endl;
    LL maskK = (1<<K)-1;
    vector<pair<int, double> > levF;
    for (int i = que.size()-1; i>=0 && count1bit(que[i]) == K-R; i--){
        levF.push_back(make_pair(que[i]^maskK, fus[i].first));
    }
    ferr<<"levF size = "<<levF.size()<<endl;

    vector<uLL> queRD;
    FUSRDTYPE fusRD;
    queRD.push_back(0);
    fusRD[0] = make_pair(0, 0);

    int cbit = 1;
    while ((1<<cbit)<(RD+1)){
        cbit++;
    }
    assert(cbit * K <=64);

    for (unsigned head = 0; head < queRD.size(); head++){
        uLL u = queRD[head];

        /*
        int extbit = 0;
        uLL tmpu = u;
        for (unsigned i = 0; i<K; i++){
            if (tmpu%(RD+1) < RD){
                extbit++;
            }
            tmpu /= (RD+1);
        }
        if (extbit < R){
            continue;
        }
        */

        double uy = fusRD[u].first;
        for (unsigned i = 0; i<levF.size(); i++){
            int x = levF[i].first;
            double y = levF[i].second;
            uLL v = tryadd(u, x, K, RD, cbit, true);
            if (v == 0){
                continue;
            }
            FUSRDTYPE::iterator ft;
            if ((ft = fusRD.find(v)) == fusRD.end()){
                fusRD[v] = make_pair(y, x);
                queRD.push_back(v);
            }else{
                double vy = ft->second.first;
                if (vy > uy + y){
                    ft->second.first = uy + y;
                    ft->second.second = x;
                }
            }
        }
    }
    ferr<<"queRD size = "<<queRD.size()<<endl;
    //now find solution
    uLL finalx = 0;
    for (int i = 0; i<K; i++){
        //finalx*=(RD+1);
        finalx = ((finalx<<cbit)|(RD));
    }
    //finalx--;
    assert(finalx == queRD[queRD.size()-1]);
    vector<int> seq;
    vector<int> midseq;
    while (finalx != 0){
        int x = fusRD[finalx].second;
        finalx = tryadd(finalx, x, K, RD, cbit, false);

        x ^= maskK;
        midseq.push_back(x);
    }
    if (midseq.size() == 0){
        midseq.push_back(maskK);
    }
    for (unsigned i = 0; i<midseq.size(); i++){
        int x = midseq[i];
        while (x != 0){
            int dx = fus[x].second;
            assert(dx);
            assert((x & dx) == dx);
            seq.push_back(dx);
            x ^= dx;
        }
    }
    ferr<<"---timetable size: "<<seq.size()<<endl;
    for (signed i = 0; i<seq.size(); i++){
        for (int j = K-1; j>=0; j--){
            if (seq[i] & (1<<j)){
                ferr<<'1';
            }else{
                ferr<<'0';
            }
        }
        ferr<<endl;
    }
    ferr<<"---timetable end---"<<endl;
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
        ferr<<"interrupt!"<<endl;
        if (keep.size() == 0){
            sem_post(&arrmtx);
            break;
        }
        if (keep.size() < K && nexttaskid < task.size()){
            sem_post(&arrmtx);
            continue;
        }

        //cut all the running thread
        for (unsigned i = 0; i<running.size(); i++){
            pausetask(running[i]);
            keep.push_back(running[i]);
        }
        running.clear();
        sort(keep.begin(), keep.end());

        if (taskttpnt == -1){
            std::random_shuffle(keep.begin(), keep.end());
            tasktimetable = gettimetable(keep);
            assert(tasktimetable.size());
            taskttpnt = 0;
        }
        int bat = tasktimetable[taskttpnt++];
        if (taskttpnt == tasktimetable.size()){
                taskttpnt = 0;
        }
        vector<int> wtr;
        for (int i = 0; i<keep.size(); i++){
            if (bat & (1<<i)){
                wtr.push_back(keep[i]);
            }
        }
        ferr<<"bat = "<<bat<<" table next:";
        for (int i = 0; i<wtr.size(); i++){
            ferr<<' '<<wtr[i];
        }
        ferr<<endl;
        assert(wtr.size() == P);
        for (int i = 0; i<wtr.size(); i++){
            runtask(wtr[i]);
        }
        sem_post(&arrmtx);
        sleep(50);
    }
    ferr<<"thread: _timeinterrupt end"<<endl;
}
#endif
