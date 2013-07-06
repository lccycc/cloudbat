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
LL tryadd(LL a, LL d, int K, int D, int sgn){
    LL res = 0;
    LL ds = 1;
    for (int i = 0; i<K; i++){
        int ba = a%(D+1);
        bool bd = (d&(1<<i));
        int nw = ba + bd * sgn;
        if (nw > D){
            return 0;
        }
        res += nw * ds;
        ds *= (D+1);
        a /= (D+1);
    }
    return res;
}
#endif
