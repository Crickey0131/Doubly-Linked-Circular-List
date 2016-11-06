#ifndef _MY402TRANSAC_H_
#define _MY402TRANSAC_H_

typedef struct tagMy402Transac {
    char add;
    int time;
    int amount;
    char *descrip;
    //char descrip[1024];
} My402Transac;

#endif /*_MY402TRANSAC_H_*/
