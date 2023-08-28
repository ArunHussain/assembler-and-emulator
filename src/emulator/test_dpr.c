#include<assert.h>
#include "dpr.h"
#include "arch.h"
#include "machine.h"
#include "stdint.h"
#include "stdbool.h"
#include "dpr.h"
void test_add(machine *m){
       uint32_t inst = 0xbc10043;// b'0001101100000010000000001000011'
       //adding register 1 and 2 to register 3
       printf("start the getsreg\n");
       set_register32(m,2,1);
       set_register32(m,1,1);
       int vm = get_register32(m,2);
       int vn = get_register32(m,1);
       printf("start the de\n");
       de_dpr(m, inst);
       printf("end the de\n");
       int vd = get_register32(m,3);
       int real_d = vm+vn;
       printf("real_d:%d\n",real_d);
       printf("vd:%d\n",vd);
       assert(vd==real_d);
}
void test_sub(machine *m){
       uint32_t inst = 0x4bc10043;// b'01001011110000010000000001000011'
       //adding register 1 and 2 to register 3
       int vm = get_register32(m,2);
       int vn = get_register32(m,1);
       de_dpr(m, inst);
       int vd = get_register32(m,3);
       int real_d = vn - vm;
       assert(vd==real_d);
}

void test_adds(machine *m){
       uint32_t inst = 0x2bc10043;// b'00001011110000011111110001000011'
       //adding register 1 and 2 to register 3
       int vm = get_register32(m,2);
       int vn = get_register32(m,1);
       de_dpr(m, inst);
       int vd = get_register32(m,3);
       int real_d = vm+vn;
       assert(vd==real_d);
}

void test_subs(machine *m){
       uint32_t inst = 0x6bc10043;// b'00011011xx0000010000000001000011'
       //adding register 1 and 2 to register 3
       int vm = get_register32(m,2);
       int vn = get_register32(m,1);
       de_dpr(m, inst);
       int vd = get_register32(m,3);
       int real_d = vn - vm;
       assert(vd==real_d);
}

void test_and(machine *m){
       uint32_t inst = 0xa010043;// b'00001011110000011111110001000011'
       //adding register 1 and 2 to register 3
       int vm = get_register32(m,2);
       int vn = get_register32(m,1);
       de_dpr(m, inst);
       int vd = get_register32(m,3);
       printf("\n");
       printf(" vm = %d, vn = %d, vd = %d\n",vm,vm,vd);
       int real_d = vm&vn;
       assert(vd==real_d);
}


void test_or(machine *m){
       uint32_t inst = 0xbc1fc43;// b'00001011110000011111110001000011'
       //adding register 1 and 2 to register 3
       int vm = get_register32(m,2);
       int vn = get_register32(m,1);
       de_dpr(m, inst);
       int vd = get_register32(m,3);
       int real_d = vm|vn;
       assert(vd==real_d);
}


void test_xor(machine *m){
       uint32_t inst = 0xbc1fc43;// b'00001011110000011111110001000011'
       //adding register 1 and 2 to register 3
       int vm = get_register32(m,2);
       int vn = get_register32(m,1);
       de_dpr(m, inst);
       int vd = get_register32(m,3);
       int real_d = vm^vn;
       assert(vd==real_d);
}

  
void test_and_with_left_shift(machine *m){
       uint32_t inst = 0xac10043;// b'00001011110000011111110001000011'
       //adding register 1 and 2 to register 3
       int vm = get_register32(m,2);
       int vn = get_register32(m,1);
       de_dpr(m, inst);
       int vd = get_register32(m,3);
       printf("vm = %d, vn = %d\n, vd = %d",vn,vm,vd);
       int real_d = (vm<<4)&vn;
       printf("assert\n");
       assert(vd==real_d);
}


void test_or_with_right_shift(machine *m){
       uint32_t inst = 0xbc1fc43;// b'00001011110000011111110001000011'
       //adding register 1 and 2 to register 3
       int vm = get_register32(m,2);
       int vn = get_register32(m,1);
       de_dpr(m, inst);
       int vd = get_register32(m,3);
       int real_d = vn|(vm>>1);
       assert(vd==real_d);
}

int main(void){
       printf("test begin\n");
    machine m1;
    machine *m = &m1;
    printf("program start\n");
    init_machine(m);
    printf("init machine success\n");
    test_add(m);
    printf("test1 success\n");
    test_adds(m);
    printf("test2 success\n");
    test_sub(m);
    printf("test3 success\n");
    test_subs(m);
    printf("test4 success\n");
    test_and(m);
    printf("test5 success\n");
}