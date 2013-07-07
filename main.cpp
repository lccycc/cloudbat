#include "head.h"
#include "present.h"
#include "sched.h"
//Sched(K, P);
Sched sched(4,12);
int main(){
    /*
    for (int i = 10; i>=1; i--){
        char tp[20];
        sprintf(tp, "%d", i);
        string name = string("./stupidtest/sleep")+tp;
        sched.addtask(name, "sh "+name, "");
    }
    */
    sched.loadtasklist(string("bubbletest/data/tasklist"));
    //sched.trypush();
    for (int i = 0; i<sched.K+sched.P; i++){
        sched.keep.push_back(i);
    }
    sched.gettimetable(sched.keep);

    return 0;
}
