//
// Created by qht on 2023/6/14.
//
#include "ll.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
ll *l1;
void make_ll_here() {
    ll_callbacks c1;
    c1.fprintf = NULL;
    c1.free = NULL;
    l1 = ll_make(1000, c1);
}
void test() {
    ll_add(l1,"1231");
    ll_add(l1,"1");
    ll_add(l1,"2");
    ll_add(l1,"3");
    ll_add(l1,"4");
    assert(strcmp((char*)(ll_get(l1,0)->value), "1231") == 0);
    assert(strcmp((char*)(ll_get(l1,1)->value), "1") == 0);
    assert(strcmp((char*)(ll_get(l1,2)->value), "2") == 0);
    assert(strcmp((char*)(ll_get(l1,3)->value), "3") == 0);
    assert(strcmp((char*)(ll_get(l1,4)->value), "4") == 0);
    ll_pop(l1,0);
    assert(strcmp((char*)(ll_get(l1,0)->value), "1234") == 0);

    return ;
}
int main(void) {
    make_ll_here();
    test();
    printf("test success\n");
    return 0;
}