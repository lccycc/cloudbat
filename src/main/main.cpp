#ifndef MAIN_CPP
#define MAIN_CPP
#include "include/head.h"
#include "present/present.h"
#include "sched/sched.h"
//Sched(K+P, P);
Sched sched(12,2);
int main(int argc, char** argv){
    assert(argc>1);
    if (argv[1].strstr("FOOTPRINT")!=0){
        sched.method = FOOTPRINTMETHOD;
    }
    if (argv[1].strstr("BUBBLE")!=0){
        sched.method = BUBBLEMETHOD;
    }

    //sched.loadtasklist(string("bubbletest/data/tasklist"));
    sched.loadbenchmark();
    //sched.printall();
    pthread_t ppush;
    pthread_create(&ppush, NULL, sched._trypush, &sched);
    sleep(4);

    pthread_t pinter;
    pthread_create(&pinter, NULL, sched._timeinterrupt, &sched);

    pthread_join(ppush, NULL);
    pthread_join(pinter, NULL);

    sched.printcputime();

    return 0;
}
#endif
