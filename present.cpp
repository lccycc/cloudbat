#ifndef PRESENT_CPP
#define PRESENT_CPP
#include "present.h"
Present::Present(){
    /*empty*/
}
Present::Present(string _name, string _cmd){
    name = _name;
    cmd = _cmd;
}

void Present::init(string filename){
    FILE *f = fopen(filename.c_str(), "r");
    fscanf(f, "N:%llu M:%llu total_time:%lf", &N, &M, &total_time);
    int i, ws;
    double fp, missrate;
    memset(mr, 0 , sizeof(mr));
    memset(ft, 0 , sizeof(ft));
    cnt = 1;
    while (fscanf(f, "%d%d%lf%*lf%lf", &i, &ws, &fp, &missrate)!=EOF){
        while (cnt<MaxCache && fp >= cnt){
            mr[cnt] = missrate;
            ft[cnt] = ws;
            //todo: this is instruction time. should change it to real time? but total_time is the pin_time, can we use it?
            //ft[cnt] = ws * total_time/N;
            cnt++;
        }
    }
    fclose(f);
}
double Present::sensitive(double filltime){
    /*
     * search c when ft[c] = filltime.
     * return mr[c]
     */
    int l = 1, r = cnt;
    while (l+1<r){
        int md = (l+r)>>1;
        if (ft[md] <=filltime){
            l = md;
        }else{
            r = md;
        }
    }
    return mr[l];
}
double Present::pressure(int cachesize){
    assert(0 < cachesize && cachesize < MaxCache);
    assert(ft[cachesize]>0);
    return ft[cachesize];
}
#endif
