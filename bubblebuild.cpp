#include "head.h"
bool threadstop = false;
double getsystime(){
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec/1000000.0;
}
void* runpressure(void* args){
    int level = *((int*)args);
    char cmd[100];
    sprintf(cmd, "./bubbletest/estream %d", level);
    cerr<<cmd<<endl;
    while (!threadstop){
        system(cmd);
    }
    return NULL;
}
int main(){
    char sensitive[] = "./bubbletest/eup 5";
    double runtime[110];
    runtime[0] = getsystime();
    system(sensitive);
    runtime[0] = getsystime() - runtime[0];
    double pair[12][12];
    /*
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
    for (int i = 1; i<=10; i++){
        for (int j = i; j<=10; j++){
            pthread_t p1, p2;
            threadstop = false;

            pthread_create(&p1, NULL, runpressure, &i);
            pthread_create(&p2, NULL, runpressure, &j);
            sleep(1);

            pair[i][j] = getsystime();
            system(sensitive);
            pair[i][j] = getsystime() - pair[i][j];

            threadstop = true;
            pthread_join(p1, NULL);
            pthread_join(p2, NULL);
        }
    }
    for (int i = 1; i<=10; i++){
        for (int j = i; j<=10; j++){
            cout<<i<<" \t"<<j<<" \t"<<pair[i][j]/runtime[0]<<endl;
        }
    }
    /*
    for (int i = 1; i<=20; i++){
        cout<<i<<" \t"<<runtime[i]/runtime[0]<<endl;
    }
    */
    return 0;
}
