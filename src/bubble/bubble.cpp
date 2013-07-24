#ifndef BUBBLE_CPP
#define BUBBLE_CPP

#include "bubble.h"
int Bubble::lookup(vector<int> &ids){
    return up[compress(ids)];
}

LL Bubble::compress(vector<int> &ids){
    sort(ids.begin(), ids.end());
    int a = 0;
    for (unsigned i = 0; i<ids.size(); i++){
        if (ids[i] <= 0){
            ids[i] = 1;
        }
        if (ids[i] >= MAX_BUBBLE_LEVEL){
            ids[i] = MAX_BUBBLE_LEVEL - 1;
        }
        a = a*MAX_BUBBLE_LEVEL + ids[i];
    }
    return a;
}
void Bubble::add(vector<int> &ids, int lv){
    up[compress(ids)] = lv;
}

#endif
