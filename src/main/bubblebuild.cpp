#ifndef BUBBLEBUILD_CPP
#define BUBBLEBUILD_CPP

#include "include/head.h"
#include "include/mth.h"

bool threadstop = false;
void* runpressure(void* args){
    int level = *((int*)args);
    char cmd[100];
    sprintf(cmd, "./bubbletest/eup %d", level);
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

map<int, string> bbpressurecmd;
double getpmu(){
    const int maxsave = 10;
    double r;

    sleep(1);
    ifstream resetin("/proc/helloworld");
    resetin>>r;
    resetin.close();
    sleep(2+maxsave);
    ifstream fin("/proc/helloworld");


    double tot = 0;
    for (int i = 1; i<=maxsave; i++){
        fin>>r;
        tot += r;
        cout<<r<<' ';
    }
    cout<<endl;
    fin.close();
    tot/=maxsave;
    return tot;
}

void runoncore(string cmd, int core){
    char s[100];
    sprintf(s, "taskset -c %d ", core);
    cmd = s + cmd + " 1>~/xxx 2>~/xxx &";
    system(cmd.c_str());
}

double getdisturb(NDC ndc, vector<int> level){
    vector<int> pid;
    chdir(ndc.dir.c_str());
    runoncore(ndc.cmd, 0);
    //int cmdpid = runandgetpid(ndc.dir, ndc.cmd, 0);
    int fd = ndc.cmd.find(" ");
    if (fd == -1) fd = ndc.cmd.length();
    string cmdcut = ndc.cmd.substr(0, fd);
    int cmdpid = getpid(cmdcut);
    cout<<"cmdpid = "<<cmdpid<<endl;
    for (unsigned i = 0; i<level.size(); i++){
       // pid.push_back(runandgetpid("", bbpressurecmd[level[i]], i+1));
       runoncore(bbpressurecmd[level[i]], i+1);
       int bbpid = getpid(bbpressurecmd[level[i]]);
       cout<<"bbpid = "<<bbpid<<endl;
       pid.push_back(bbpid);
    }
    double pmu = getpmu();
    killpid(cmdpid);
    system("killall eup -9");
    /*
    for (unsigned i = 0; i<pid.size(); i++){
        killpid(pid[i]);
    }
    */
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
            double res = (freerun-ds)/freerun;
            fout<<ndc.name<<' '<<P<<' '<<i<<' '<<res<<endl;
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
        string pref = "~/cloudbat/bubbletest/eup ";
        char c[5];
        sprintf(c, " %d", i);
        string cmd = pref + c + " infinite";
        cout<<cmd<<endl;
        bbpressurecmd[i]=cmd;
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
