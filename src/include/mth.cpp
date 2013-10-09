#ifndef MTH_CPP
#define MTH_CPP
#include "mth.h"
LL gcd(LL a, LL b){
    return b?gcd(b,a%b):a;
}
int count1bit(LL a){
    int c = 0;
    while (a){
        c+=(a&1);
        a>>=1;
    }
    return c;
}
void init(LL* mi, int base, int len){
    mi[0] = 1;
    for (int i = 1; i<len; i++){
        mi[i] = mi[i-1]*len;
    }
}
LL getpos(LL a, LL* mi, int pos){
    return a/mi[pos]%mi[1];
}
uLL tryadd(uLL a, int d, int K, int D, int cbit, bool sgn){
    uLL res = 0;
    uLL mask = (1LL<<cbit)-1;
    int bs = 0;
    for (int i = 0; i<K; i++){
        uLL ba = (a & mask);
        bool bd = (d&(1LL<<i));
        uLL nw;
        if (sgn){
            nw = ba + bd;
            if (nw > D){
                return 0;
            }
        }else{
            assert(ba >= bd);
            nw = ba - bd;
        }
        res |= (nw<<bs);
        bs += cbit;
        a>>=cbit;
    }
    return res;
}
double getsystime(){
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1000000.0;
}
int getpid(string cmd){
    //cmd = " " + cmd + "\n";
    FILE *fp = popen("ps -ao pid,command", "r");
    assert(fp);
    int pid = -1, tpid;
    char pscmd[500];
    bool findit = false;
    while (fscanf(fp, "%d", &tpid)!=EOF){
        fgets(pscmd, 488, fp);
        if (strstr(pscmd, cmd.c_str())!=NULL && strstr(pscmd, "sh -c") == NULL){
			if (tpid > pid) pid = tpid;
            findit = true;
        }
    }
    fclose(fp);
    if (!findit){
        return -1;
    }
    return pid;
}
void pausepid(int pid){
    kill(pid, 19);
}
void killpid(int pid){
    kill(pid, 9);
}
void fgpid(int pid){
    kill(pid, 18);
}
int runandgetpid(string dir, string cmd, int core){
    cmd = cmd + "\" | ~/cloudbat/script/runandgetpid.sh";
    if (core != -1){
        char s[100];
        sprintf(s, "taskset -c %d ", core);
        cmd = s + cmd;
    }
    cmd = dir + " \n " + cmd;
    cmd = "echo \""+cmd;
    cout<<cmd<<endl;
    FILE *f = popen(cmd.c_str(), "r");
    int pid = -1;
    fscanf(f, "%d", &pid);
    assert(pid != 32767 && pid != -1);
    fclose(f);
    cout<<"pid = "<<pid<<endl;
    return pid;
}
string getfirstword(string s){
    int fd = s.find(" ");
    if (fd == -1) fd = s.length();
    return s.substr(0, fd);
}
string getbits(int i, int K){
	string s;
	for (int j = K-1; j>=0; j--){
		if (i&(1<<j)){
			s += "1";
		}else{
			s += "0";
		}
	}
	return s;
}
#endif
