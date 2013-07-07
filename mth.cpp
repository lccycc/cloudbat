#ifndef MTH_CPP
#define MTH_CPP
#include "mth.h"
LL gcd(LL a, LL b){
    return b?gcd(b,a%b):a;
}
int count1bit(LL a){
    int c = 0;
    while (a){
        c+=(a&1);
        a>>=1;
    }
    return c;
}
void init(LL* mi, int base, int len){
    mi[0] = 1;
    for (int i = 1; i<len; i++){
        mi[i] = mi[i-1]*len;
    }
}
LL getpos(LL a, LL* mi, int pos){
    return a/mi[pos]%mi[1];
}
uLL tryadd(uLL a, int d, int K, int D, int cbit, bool sgn){
    uLL res = 0;
    uLL mask = (1LL<<cbit)-1;
    int bs = 0;
    for (int i = 0; i<K; i++){
        uLL ba = (a & mask);
        bool bd = (d&(1LL<<i));
        uLL nw;
        if (sgn){
            nw = ba + bd;
            if (nw > D){
                return 0;
            }
        }else{
            assert(ba >= bd);
            nw = ba - bd;
        }
        res |= (nw<<bs);
        bs += cbit;
        a>>=cbit;
    }
    return res;
}
#endif
