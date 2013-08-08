#ifndef BUBBLE_CPP
#define BUBBLE_CPP

#include "bubble.h"
void Bubble::init(){
    ifstream fin("./benchmark/bubble/corunlevel.dat");
    int cop;
    vector<LL> levsav[5];
    while (fin>>cop){
        vector<int> lev;
        int x;
        for (int i = 0; i<cop; i++){
            fin>>x;
            lev.push_back(x);
        }
        double delay;
        fin>>delay;
        delay /= 1-delay;
        LL addr = compress(lev);
        up[addr] = delay;
        levsav[cop].push_back(addr);
    }
    map<int, map<int, double> > stddelay;
    for (int p = 1; p<=3; p++){
        stddelay[p][0] = 0;
        for (int i = 1; i<=MAX_BUBBLE_LEVEL; i++){
            vector<int> lev;
            for (int j = 0; j<p; j++){
                lev.push_back(i);
            }
            stddelay[p][i] = up[compress(lev)];
        }
    }
    for (int p = 1; p<=3; p++){
        for (unsigned t = 0; t<levsav[p].size(); t++){
            LL addr = levsav[p][t];
            double r = up[addr];
            int i;
            for (i = 1; i<MAX_BUBBLE_LEVEL; i++){
                if (stddelay[p][i] >= r){
                    break;
                }
            }
            r = min(r, stddelay[p][i]);
            double x= stddelay[p][i-1], y = stddelay[p][i];
            double u = (y - r)/(x - r);
            db[addr] = i + u;
        }
    }
}

double Bubble::lookup(Present &p, vector<int> &lev){
    LL addr = compress(lev);
    int i = db[addr];
    double u = db[addr] - i;
    map<int, double> &m = p.delay[lev.size()];
    return u*m[i-1] + (1-u)*m[i];
}
int Bubble::findcloseplevel(double delay){
    for (int i = 1; i<=MAX_BUBBLE_LEVEL; i++){
        vector<int> lev;
        lev.push_back(i);
        if (up[compress(lev)] >= delay){
            return i;
        }
    }
    return MAX_BUBBLE_LEVEL;
}
LL Bubble::compress(vector<int> &lev){
    sort(lev.begin(), lev.end());
    int a = 0;
    for (unsigned i = 0; i<lev.size(); i++){
        if (lev[i] <= 0){
            lev[i] = 1;
        }
        if (lev[i] > MAX_BUBBLE_LEVEL){
            lev[i] = MAX_BUBBLE_LEVEL;
        }
        a = a*(MAX_BUBBLE_LEVEL+1) + lev[i];
    }
    return a;
}
#endif
