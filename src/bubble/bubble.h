#ifndef BUBBLE_H
#define BUBBLE_H
#include "include/head.h"
const int MAX_BUBBLE_LEVEL = 50;
class Bubble{
public:
    int lookup(vector<int> &ids);
    void init();
private:
    map<LL, int> up;

    LL compress(vector<int> &ids);
    void add(vector<int> &ids, int lv);
};
#endif
