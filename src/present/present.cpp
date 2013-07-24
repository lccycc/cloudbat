#ifndef PRESENT_CPP
#define PRESENT_CPP
#include "present.h"
Present::Present(){
    /*empty*/
}
Present::Present(string _name, string _cmd, int _id){
    name = _name;
    cmd = _cmd;
    dir = "./";
    id = _id;

    pid = -1;
}

void Present::init(string filename){
    FILE *f = fopen(filename.c_str(), "r");
    fscanf(f, "N:%llu M:%llu total_time:%*lf", &N, &M);
    int i, ws;
    double fp, missrate, td;
    memset(mr, 0 , sizeof(mr));
    //memset(ft, 0 , sizeof(ft));
    cnt = 1;
    while (fscanf(f, "%d%d%lf%lf%lf", &i, &ws, &fp, &td, &missrate)!=EOF){
        ft2c_c[i] = fp;
        mr[i] = missrate;
        /*
        while (cnt<MaxCache && fp >= cnt){
            mr[cnt] = missrate;
            ft[cnt] = ws;
            //todo: this is instruction time. should change it to real time? but total_time is the pin_time, can we use it?
            //ft[cnt] = ws * total_time/N;
            cnt++;
        }
        */
    }
    cnt = i + 1;
    fclose(f);
}
double Present::missnum(double filltime){
    filltime = filltime * N /(total_time*1000000000);
    int idx =  sublog_value_to_index<MAX_WINDOW, SUBLOG_BITS> ((LL)filltime);
    idx = min(idx, cnt-1);
    return mr[idx]*N/filltime;
}
double Present::fillcache(double filltime){
    filltime = filltime * N /(total_time*1000000000);
    int idx =  sublog_value_to_index<MAX_WINDOW, SUBLOG_BITS> ((uLL)filltime);
    idx = min(idx, cnt-1);
    return ft2c_c[idx];
}
#endif
