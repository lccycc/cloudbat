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

	debug = 0;
    pid = -1;
    cputime = 0;
}

void Present::footprint_init(string filename){
    FILE *f = fopen(filename.c_str(), "r");
    fscanf(f, "N:%lld M:%lld total_time:%*lf", &N, &M);
    int i, ws;
    double fp, missrate, td;
    memset(mr, 0 , sizeof(mr));
    cnt = 1;
    while (fscanf(f, "%d%d%lf%lf%lf", &i, &ws, &fp, &td, &missrate)!=EOF){
        ft2c_c[i] = fp;
        mr[i] = missrate;
    }
    cnt = i + 1;
    fclose(f);
}
double Present::missnum(double filltime){
    filltime = filltime * N /(stdruntime*1000000000);
    int idx =  sublog_value_to_index<MAX_WINDOW, SUBLOG_BITS> ((LL)filltime);
    idx = min(idx, cnt-1);
	if (debug){
		cout<<"debug: "<<idx<<' '<<cnt<<endl;
	}
    return mr[idx]*(N/stdruntime)/1000000000;
}
double Present::fillcache(double filltime){
    filltime = filltime * N /(stdruntime*1000000000);
    int idx =  sublog_value_to_index<MAX_WINDOW, SUBLOG_BITS> ((uLL)filltime);
    idx = min(idx, cnt-1);
    return ft2c_c[idx];
}

void Present::reusedst_init(string filename){
	FILE *f = fopen(filename.c_str(), "r");
	fscanf(f, "%lld", &N);
	int i, subi;
	LL rd, sum = 0;
	rdmr[0] = 1;
	rdft_c[0] = 0;
	double t = 0;
	rdftcnt = 0;
	double maxvalue =
			sublog_index_to_value<MAX_WINDOW, SUBLOG_BITS> (MAX_WINDOW-1);
	while (fscanf(f, "%d%lld", &i, &rd)!=EOF){
		sum += rd;
		double mr = (double)(N-sum)/(double)N;
		subi = sublog_value_to_index<MAX_WINDOW, SUBLOG_BITS>(i);
		rdmr[subi] = mr;
		if (mr != 0){
			t += 1/mr;
			if (t < maxvalue){
				int subt =
						sublog_value_to_index<MAX_WINDOW, SUBLOG_BITS>((LL)t);
				while (rdftcnt <= subt && rdftcnt < WindowSub){
					rdft_c[rdftcnt++] = i;
				}
			}
		}
	}
	rdmrcnt = subi+1;
}
double Present::rdfillcache(double filltime){
    filltime = filltime * N /(stdruntime*1000000000);
    int idx =  sublog_value_to_index<MAX_WINDOW, SUBLOG_BITS> ((uLL)filltime);
    idx = min(idx, rdftcnt-1);
    return rdft_c[idx];
}
double Present::rdmissnum(double filltime){
	int cache = rdfillcache(filltime);
	int subc = sublog_value_to_index<MAX_WINDOW, SUBLOG_BITS>(cache);
	subc = min(subc, rdmrcnt-1);
    return rdmr[subc]*N/stdruntime/1000000000;
}

#endif
