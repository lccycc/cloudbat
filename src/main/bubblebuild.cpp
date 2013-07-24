#ifndef BUBBLEBUILD_CPP
#define BUBBLEBUILD_CPP

#include "include/head.h"
#include "include/mth.h"

bool threadstop = false;
void* runpressure(void* args){
    int level = *((int*)args);
    char cmd[100];
    sprintf(cmd, "./bubbletest/estream %d", level);
    cerr<<cmd<<endl;
    while (!threadstop){
        system(cmd);
    }
    return NULL;
}

class NDC{
public:
    string name, dir, cmd;
    NDC(string _name, string _dir, string _cmd):name(_name), dir(_dir), cmd(_cmd){
    }
};
vector<NDC> benchmarklist;

vector<string> bbpressurecmd;
double getpmu(){
    sleep(1);
    ifstream resetin("/proc/helloworld");
    resetin.close();
    sleep(12);
    ifstream fin("/proc/helloworld");

    double tot = 0;
    for (int i = 0; i<20; i++){
        double r;
        fin>>r;
        tot += r;
    }
    fin.close();
    tot/=20;
    return tot;
}

double getdisturb(NDC ndc, vector<int> level){
    vector<int> pid;
    chdir(ndc.dir.c_str());
    int cmdpid = runandgetpid(ndc.cmd, 0);
    for (unsigned i = 0; i<level.size(); i++){
        pid.push_back(runandgetpid(bbpressurecmd[level[i]], i+1));
    }
    double pmu = getpmu();
    killpid(cmdpid);
    for (unsigned i = 0; i<pid.size(); i++){
        killpid(pid[i]);
    }
    return pmu;
}
void getsensitive(NDC ndc, ofstream &fout){
    double freerun = getdisturb(ndc, vector<int>());
    for (int P = 1; P<=3; P++){
        for (int i = 1; i<=10; i++){
            vector<int> lev;
            for (int j = 0; j<P; j++){
                lev.push_back(i);
            }
            double ds = getdisturb(ndc, lev);
            fout<<ndc.name<<' '<<P<<' '<<i<<' '<<ds/freerun<<endl;
        }
    }
}
void loadbenchmark(){
    ifstream fin("./benchmark/speccmd.cmd");
    string name, dir, cmd;
    while(std::getline(fin, name)){
        std::getline(fin, dir);
        std::getline(fin, cmd);
        benchmarklist.push_back(NDC(name, dir, cmd));
    }
}
void getbenchmarksensitive(){
    ofstream fout("./benchmark/bubble/benchmarklevel.dat");
    for (unsigned i = 0; i<benchmarklist.size(); i++){
        getsensitive(benchmarklist[i], fout);
    }
    fout.close();
}

int main(){

    for (int i = 1; i<=10; i++){
        string pref = "~/cloudbat/bubbletest/estreamaccess ";
        char c[5];
        sprintf(c, "%d", i);
        string cmd = pref + c + " infinite";
        cout<<cmd<<endl;
        bbpressurecmd.push_back(cmd);
    }
    loadbenchmark();
    getbenchmarksensitive();


    /*
    char sensitive[] = "./bubbletest/eup 5";
    double runtime[110];
    runtime[0] = getsystime();
    system(sensitive);
    runtime[0] = getsystime() - runtime[0];
    double pair[12][12];
    for (int i = 1; i<=20; i++){
        pthread_t p1, p2;
        threadstop = false;
        pthread_create(&p1, NULL, runpressure, &i);
        sleep(1);

        runtime[i] = getsystime();
        system(sensitive);
        runtime[i]  = getsystime() - runtime[i];
        cerr<<runtime[i]<<endl;

        threadstop = true;
        pthread_join(p1, NULL);
    }
    */
    return 0;
}
#endif
