#ifndef SCHED_CPP
#define SCHED_CPP
#include "sched.h"
void Sched::init(int _KPP, int _P){

	FILE *fin = fopen("./config", "r");
	char tmp[200];
	fscanf(fin, "cache %d\n", &cachesize);
	fscanf(fin, "bandwith %lf\n", &bandwith);
	bandwith/=64;
	/* for prefetch */
	//bandwith /= 2;
	fscanf(fin, "speccmd %s\n", tmp);
	speccmd = string(tmp);
	int cpuid, cpunum;
	fscanf(fin, "%d", &cpunum);
	while (cpunum--){
		fscanf(fin, "%d", &cpuid);
		cpumask.push_back(cpuid);
	}
	fclose(fin);

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
void Sched::loadbenchmark(string ordername){
	switch (method){
	case FREERUN: model = "freerun"; break;
	case NOPREDICTION: model = "nopred"; break;
	case FOOTPRINTMETHOD: model = "foot"; break;
	case REUSEDSTMETHOD: model = "reuse"; break;
	case BUBBLEMETHOD: model = "bubble"; break;
	};
	char tmpk[20];
	sprintf(tmpk, "K%d", K+P);
	string ferrfile = "./log/"+model+"."+ordername+tmpk;
	ferr.open(ferrfile.c_str(), ios::out);

	ferr<<"cache = "<<cachesize<<endl;
	ferr<<"speccmd = "<<speccmd<<endl;

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

    ifstream fin(speccmd.c_str());
    while (std::getline(fin, name)){
        std::getline(fin, dir);
        std::getline(fin, cmd);
        di[name] = dir;
        cm[name] = cmd;
    }
    fin.close();

	string orderfile = "./benchmark/order_"+ordername;
    ifstream oin(orderfile.c_str());
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
	assert(task.size() >= P);
    ferr<<"tasksize = "<<task.size()<<endl;

    //if (method == FOOTPRINTMETHOD){
        for (unsigned i = 0; i<task.size(); i++){
            task[i].footprint_init("./benchmark/footprint/"+task[i].name+".dat");
        }
    //}else
	if (method == REUSEDSTMETHOD){
		for (unsigned i = 0; i<task.size(); i++){
			task[i].reusedst_init("./benchmark/reusedst/"+task[i].name+".dat");
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
                    ferr<<"task "<<i<<" "<<task[i].name<<" pressure level: "<<task[i].plevel<<endl;
                }
            }
        }
        bpin.close();
    }

#ifdef SCHEDDEBUG
	for (int i = 0; i<task.size(); i++){
		totalworkload.push_back(0);
	}
#endif
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
double Sched::getrdfilltime(vector<int> &ids){
    const double MaxTime = 200.0 * 1000000000.0;
    double l = 1, r = MaxTime;
    double ttc = 0;
    while (l+1<r){
        double md = (l+r)/2;
        ttc = 0;
        for (unsigned i = 0; i<ids.size(); i++){
            ttc +=task[ids[i]].rdfillcache(md);
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
	const double L1 = 1000000;
	const double L2 = 1000;
    double ft = getfpfilltime(ids);
    double mn = 0;
    for (unsigned i = 0; i<ids.size(); i++){
        mn += task[ids[i]].missnum(ft);
    }
	if (mn>=bandwith){
		//mn += L1;
		mn += L2 * (mn-bandwith);
	}
    return mn;
}
double Sched::getsingleworkload(vector<int> ids, int spe){
	if (method != FOOTPRINTMETHOD){
		return 0;
	}
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
	double tot = 0;
	ferr<<"model: "<<model<<endl;
    for (unsigned i = 0; i<task.size(); i++){
		tot += task[i].cputime;
        double deta = task[i].cputime - task[i].stdruntime;
        ferr<<task[i].name<<"\t"<<task[i].cputime<<"\t"<<deta/task[i].stdruntime<<endl;
    }
	ferr<<"CPU Time: "<<tot<<"s"<<endl;
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
double Sched::getrdworkload(vector<int> &ids){
    double ft = getrdfilltime(ids);
    double mn = 0;
    for (unsigned i = 0; i<ids.size(); i++){
        mn += task[ids[i]].rdmissnum(ft);
    }
	return mn;
}
double Sched::getworkload(vector<int> &ids){
    if (method == NOPREDICTION){
        return 0;
    }
    if (method == FOOTPRINTMETHOD){
        return getfpworkload(ids);
    }else
	if (method == REUSEDSTMETHOD){
		return getrdworkload(ids);
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
    s.ferr<<"Thread: _trypush end"<<endl;
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
        while (((task[u].pid = getpid(cutcmd))==-1
                    || pidset.find(task[u].pid) != pidset.end())
					&& getpidcnt--){
            usleep(10000);
        }
		pidset.insert(task[u].pid);
        assert(task[u].pid>=0);
		ferr<<"task "<<u<<" pid: "<<task[u].pid<<endl;
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
	string cmd = s.task[id].cmd;
	cmd = cmd + " 1>/dev/null 2>/dev/null";
    system(cmd.c_str());

	s.taskstatic(id);

    s.taskfinish(id);
}

void Sched::taskstatic(int id){
	double during = getsystime() - task[id].lastrunt;
    task[id].cputime += during;
#ifdef SCHEDDEBUG
	double wk = getsingleworkload(running, id);
	totalworkload[id] += wk * during;
#endif
}

void Sched::pausetask(int id){
    pausepid(task[id].pid);
	taskstatic(id);
    //ferr<<"task pause, id = "<<id<<", pid = "<<task[id].pid<<endl;
}
void Sched::fgtask(int id){
    task[id].lastrunt = getsystime();
    fgpid(task[id].pid);
    //ferr<<"task fg, id = "<<id<<", pid = "<<task[id].pid<<endl;
}

double Sched::printfpmiss(vector<int> list, int i){
	if (method != FOOTPRINTMETHOD){
		return 0;
	}
	for (int j = K-1; j>=0; j--){
		if ((1<<j) & i){
			cout<<"1";
		}else{
			cout<<"0";
		}
	}
	cout<<endl;
	vector<int> ids;
	for (int j = 0; j<K; j++){
		if (i & (1<<j)){
			ids.push_back(list[j]);
		}
	}
	double ft = getfpfilltime(ids);
	double tot = 0;
	for (int j = 0; j<ids.size(); j++){
		task[ids[j]].debug = true;
		double wk = task[ids[j]].missnum(ft);
		cout<<"debug: "<<ids[j]<<' '<<wk<<endl;
		task[ids[j]].debug = false;
		tot += wk;
	}
	return tot;
}

vector<int> Sched::gettimetable(vector<int> list){
    typedef unordered_map<int, pair<double, int> > FUSTYPE;
    typedef unordered_map<uLL, pair<double, int> > FUSRDTYPE;

    int K = list.size();
    assert(K<31);
    assert(K>=P);
	while (K%P && P%(K%P)){
		K--;
	}
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

	map<LL, double> debugdp;
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
		debugdp[i] = mr;
        lev0.push_back(make_pair(i, mr));
    }

	//common below
	/*
	for (map<LL, double>::iterator it = debugdp.begin();
			it != debugdp.end(); it++){
		int i = it->first;
		int j = (1<<K)-1-i;
		double w1 = debugdp[i], w2 = debugdp[j];
		cout<<getbits(i, K)<<"\t"<<w1<<"\t"<<getbits(i, K)<<"\t"<<w2<<"\t"<<w1+w2<<endl;
	}
	{
		int i;
		double w1, w2;
		i = 1+2+4;
		w1 = printfpmiss(list, i);
		i = (1<<K)-1-i;
		w2 = printfpmiss(list, i);
		cout<<"w1 "<<w1<<" w2 "<<w2<<" + "<<w1+w2<<endl;

		i = 1+2+(1<<4);
		w1 = printfpmiss(list, i);
		i = (1<<K)-1-i;
		w2 = printfpmiss(list, i);
		cout<<"w1 "<<w1<<" w2 "<<w2<<" + "<<w1+w2<<endl;
	}
	*/
	//commonend
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
                fusRD[v] = make_pair(uy + y, x);
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
		vector<int> ids;
        for (int j = K-1; j>=0; j--){
            if (seq[i] & (1<<j)){
                ferr<<'1';
				ids.push_back(list[j]);
            }else{
                ferr<<'0';
            }
        }
		double wl = getfpworkload(ids);
		ferr<<"\t"<<wl;
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
        sleep(10);
    }
    ferr<<"thread: _timeinterrupt end"<<endl;
}
#endif
