#include "head.h"
#include "present.h"
#include "sched.h"
Sched sched(1,2);
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
    sched.trypush();

    return 0;
}
