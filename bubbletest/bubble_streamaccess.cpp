#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <assert.h>
#include <iostream>
#include <math.h>
using namespace std;

int main(int argc, char** argv)
{
	long bw_stream_size;  // double type occupies 8 Bytes
	//double sizeofdouble;
	//printf("bubble stream program");
	//bw_stream_size = 1280 * 1024; //1M space

    int level = 5;
    bool infinite = 0;
    if (argc>=2){
        sscanf(argv[1], "%d", &level);
    }else{
        cerr<<"warning: default level = 5"<<endl;
    }
    if (argc>=3){
        infinite = 1;
    }
    bw_stream_size = 1024*256*level/2;

	double *bw_data;
	double *mid;
	double scalar;
	scalar = 1.1;
	bw_data = (double*)malloc(bw_stream_size*sizeof(double));
	mid=bw_data+bw_stream_size/2;

	//printf("smash space is 1280 * 8 KB\n");
    long long round = 15000000L * 100 / bw_stream_size ;

    round *= 2;

    for (long long j = 0; j<round || infinite; j++){
        for(int i=0;i<bw_stream_size/2;i++){
            bw_data[i] = scalar * mid[i];
        }
        for(int i=0;i<bw_stream_size/2;i++){
            mid[i] = scalar * mid[i];
        }
    }
	return 0;
}
