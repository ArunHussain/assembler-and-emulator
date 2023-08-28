#include "readfile.h"
#include "ll.h"
#include <stdio.h>
#include <stdlib.h>
void ll_printf2(ll *linkedlist)
{
    // ll_node *current = linkedlist->head;
    // if (current!=NULL)
    // {

    //     printf("%s \n", ((char*)current->value));
    // }
    // current=current->next;
    // while(current!=NULL)
    // {
    //     printf("%s \n", ((char*)current->value));
    //     current=current->next;
    // }

    ll *lines = linkedlist;

    for (ll_node *pointer = lines->head; pointer != NULL; pointer = pointer -> next) {
        char *line = (char *) pointer->value;
        printf("(debug) BUILD_SYMBOL_TABLE: line = %s\n", line);
    }
}

void test() {
    FILE* fp = fopen("test.s","r");
    ll *test = readfile(fp);
    printf("---------------------\n");
    ll_printf2(test);
    printf("\n---------------------\n ");    fclose(fp);
    printf("%d ", ll_length(test));
    return ;
}
int main(void) {
    test();
    return 0;
}
