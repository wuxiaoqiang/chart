
#include "test_uv_map.h"



SPLAY_GENERATE(MY_DATA_SPLAY, MY_DATA, node, cmp_my_data);

RB_GENERATE(MY_DATA_RB, MY_DATA, node1, cmp_my_data);


int cmp_my_data(struct MY_DATA *a, struct MY_DATA *b)
{
    return a->a - b->a;
}


void test_uv_splay()
{
    // SPLAY_HEAD(MY_DATA_SPLAY, MY_DATA) my_splay;
    struct MY_DATA_SPLAY my_splay;
    SPLAY_INIT(&my_splay);

    struct MY_DATA data1,data2,data3;
    data1.a = 11;
    data2.a = 12;
    data3.a = 13;

    SPLAY_INSERT(MY_DATA_SPLAY, &my_splay, &data1);
    SPLAY_INSERT(MY_DATA_SPLAY, &my_splay, &data2);
    SPLAY_INSERT(MY_DATA_SPLAY, &my_splay, &data3);
}

void test_uv_rb()
{
    // SPLAY_HEAD(MY_DATA_SPLAY, MY_DATA) my_splay;
    struct MY_DATA_RB my_rb;
    RB_INIT(&my_rb);

    struct MY_DATA data1,data2,data3;
    data1.a = 11;
    data2.a = 13;
    data3.a = 12;

    RB_INSERT(MY_DATA_RB, &my_rb, &data1);
    RB_INSERT(MY_DATA_RB, &my_rb, &data2);
    RB_INSERT(MY_DATA_RB, &my_rb, &data3);
}