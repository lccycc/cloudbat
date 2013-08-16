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
NDC bbsensitive("eup", "", "/home/lcc/cloudbat/bubbletest/eup 5 infinite");

double getpmu(){
    const int maxsave = 10;
    double r;

    sleep(1);
    ifstream resetin("/proc/helloworld");
    resetin>>r;
    resetin.close();
    sleep(2+maxsave*10);
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
    chdir(ndc.dir.c_str());
    runoncore(ndc.cmd, 0);
    //int cmdpid = runandgetpid(ndc.dir, ndc.cmd, 0);
    int cmdpid = getpid(getfirstword(ndc.cmd));
    cout<<"cmdpid = "<<cmdpid<<endl;
    for (unsigned i = 0; i<level.size(); i++){
       runoncore(bbpressurecmd[level[i]], i+1);
    }
    double pmu = getpmu();
    killpid(cmdpid);
    system("killall estream -9");
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
            double res = (freerun-ds)/ds;
            fout<<ndc.name<<' '<<P<<' '<<i<<' '<<res<<endl;
        }
    }
}
void getpressure(NDC ndc, ofstream &fout){
    chdir(bbsensitive.dir.c_str());
    runoncore(bbsensitive.cmd, 0);
    double freerun = getpmu();

    chdir(ndc.dir.c_str());
    runoncore(ndc.cmd, 1);
    int pid = getpid(getfirstword(ndc.cmd));
    cout<<" benchpid = "<<pid<<endl;

    double ds = getpmu();
    fout<<ndc.name<<"\t"<<(freerun-ds)/ds<<endl;

    system("killall eup -9");
    killpid(pid);
}

void loadbenchmark(){
    ifstream fin("./benchmark/speccmd.cmd");
    string name, dir, cmd;
    bool cont = false;
    while(std::getline(fin, name)){
        std::getline(fin, dir);
        std::getline(fin, cmd);
        /*
        if (name.compare("omnetpp") == 0){
            cont = true;
        }
        if (cont)
        */
            benchmarklist.push_back(NDC(name, dir, cmd));
    }
}
void getbenchmarksensitive(){
    ofstream fout("./benchmark/bubble/benchmarklevel.dat");
    for (unsigned i = 9; i<benchmarklist.size(); i++){
        getsensitive(benchmarklist[i], fout);
    }
    fout.close();
}
void getbenchmarkpressure(){
    ofstream fout("./benchmark/bubble/benchmarkpplevel.dat");
    for (unsigned i = 0; i<benchmarklist.size(); i++){
        getpressure(benchmarklist[i], fout);
    }
    fout.close();
}
void getcorunpressure(){
    ofstream fout("./benchmark/bubble/corunlevel.dat");

    double freerun = getdisturb(bbsensitive, vector<int>());
    for (int i = 1; i<=10; i++){
        vector<int> ids;
        ids.push_back(i);
        double ds = getdisturb(bbsensitive, ids);
        double rate = (freerun-ds)/ds;
        fout<<"1\t"<<i<<"\t"<<rate<<endl;
    }
    for (int i = 1; i<=10; i++){
        for (int j = i; j<=10; j++){
            vector<int> ids;
            ids.push_back(i);
            ids.push_back(j);
            double ds = getdisturb(bbsensitive, ids);
            double rate = (freerun-ds)/ds;
            fout<<"2\t"<<i<<"\t"<<j<<"\t"<<rate<<endl;
        }
    }
    for (int i = 1; i<=10; i++){
        for (int j = i; j<=10; j++){
            for (int k = j; k<=10; k++){
                vector<int> ids;
                ids.push_back(i);
                ids.push_back(j);
                ids.push_back(k);
                double ds = getdisturb(bbsensitive, ids);
                double rate = (freerun-ds)/ds;
                fout<<"3\t"<<i<<"\t"<<j<<"\t"<<k<<"\t"<<rate<<endl;
            }
        }
    }
    fout.close();
}

int main(){

    for (int i = 1; i<=10; i++){
        string pref = "~/cloudbat/bubbletest/estream ";
        char c[5];
        sprintf(c, " %d", i);
        string cmd = pref + c + " infinite";
        bbpressurecmd[i]=cmd;
    }
    loadbenchmark();
    getbenchmarksensitive();
    //getbenchmarkpressure();
    //getcorunpressure();

    return 0;
}
#endif
