#ifndef DLIST
#define DLIST
#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"

struct dList
{
    int val;
    struct dList*next;
    struct dList*previus;
    
};


struct dList * dListCreateList();
bool dListInsertList(struct dList *node);
bool dListDeleteList(struct dList *list);
void regularPrint(struct dList *list);
void reversePrint(struct dList *list);



#endif