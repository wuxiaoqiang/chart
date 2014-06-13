#include "tree.h"

#define NULL 0

struct MY_DATA
{
    SPLAY_ENTRY(MY_DATA) node;
    RB_ENTRY(MY_DATA) node1;
    int a;
};

SPLAY_HEAD(MY_DATA_SPLAY, MY_DATA);

RB_HEAD(MY_DATA_RB, MY_DATA);

int cmp_my_data(struct MY_DATA *a, struct MY_DATA *b);

SPLAY_PROTOTYPE(MY_DATA_SPLAY, MY_DATA, node, cmp_my_data);

RB_PROTOTYPE(MY_DATA_RB, MY_DATA, node1, cmp_my_data);

