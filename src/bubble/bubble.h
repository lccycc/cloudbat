#ifndef BUBBLE_H
#define BUBBLE_H
#include "include/head.h"
#include "present/present.h"
const int MAX_BUBBLE_LEVEL = 10;
class Bubble{
public:
    double lookup(Present &p, vector<int> &lev);
    int findcloseplevel(double delay);
    void init();
private:
    map<LL, double> up;
    map<LL, double> db;
    //u*(i-1) + (1-u)*i

    LL compress(vector<int> &lev);
};
#endif
