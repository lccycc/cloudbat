#include "head.h"
#include "present.h"
#include "sched.h"
//Sched(K, P);
Sched sched(4,2);
int main(){
    /*
    for (int i = 10; i>=1; i--){
        char tp[20];
        sprintf(tp, "%d", i);
        string name = string("./stupidtest/sleep")+tp;
        sched.addtask(name, "sh "+name, "");
    }
    */
    //sched.loadtasklist(string("bubbletest/data/tasklist"));
    sched.loadbenchmark();
    pthread_t ppush;
    pthread_create(&ppush, NULL, sched._trypush, &sched);
    sleep(4);

    pthread_t pinter;
    pthread_create(&pinter, NULL, sched._timeinterrupt, &sched);

    pthread_join(ppush, NULL);
    pthread_join(pinter, NULL);

    return 0;
}
