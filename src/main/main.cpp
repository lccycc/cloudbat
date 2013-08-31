#ifndef MAIN_CPP
#define MAIN_CPP
#include "include/head.h"
#include "present/present.h"
#include "sched/sched.h"
//Sched(K+P, P);
Sched sched;
int main(int argc, char** argv){
    bool freerun = false;
    assert(argc>=5);
    if (strstr(argv[1], "FOOTPRINT") != 0){
        sched.method = FOOTPRINTMETHOD;
    }else
    if (strstr(argv[1], "REUSEDST") != 0){
        sched.method = REUSEDSTMETHOD;
    }else
    if (strstr(argv[1], "BUBBLE") != 0){
        sched.method = BUBBLEMETHOD;
    }else
    if (strstr(argv[1], "NOPREDICTION") != 0){
        sched.method = NOPREDICTION;
    }
    if (strstr(argv[1], "FREERUN")!=0){
        freerun = true;
		sched.method = FREERUN;
    }
	int _K, _P;
	sscanf(argv[2], "%d", &_K);
	sscanf(argv[3], "%d", &_P);
	sched.init(_K, _P);
	string ordername(argv[4]);

    sched.loadbenchmark(ordername);

    pthread_t ppush;
    pthread_create(&ppush, NULL, sched._trypush, &sched);
    sleep(4);

    pthread_t pinter;
    if (!freerun){
        pthread_create(&pinter, NULL, sched._timeinterrupt, &sched);
    }

    pthread_join(ppush, NULL);
    if (!freerun){
        pthread_join(pinter, NULL);
    }

    sched.printcputime();
    return 0;
}
#endif
