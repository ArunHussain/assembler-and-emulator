#include "ll.h"
#include <stdlib.h>

ll *ll_make(int size, ll_callbacks callbacks){
    ll* linkedlist = (ll*) malloc(sizeof(ll));
    linkedlist->size = size;
    linkedlist->head = NULL;
    linkedlist->callbacks = callbacks;
    return linkedlist;
}

void ll_add(ll *linkedlist, void *value){
    struct ll_node *temp = (struct ll_node *)malloc(sizeof(struct ll_node));
    struct ll_node *pointer = (struct ll_node *)malloc(sizeof(struct ll_node));
    
    if (temp==NULL)
    {
        exit(1);
    }
    
    temp->next = NULL;
    temp->value = value;
    pointer=linkedlist->head;
    
    if (pointer==NULL){
        linkedlist->head = temp;    
    }
    else{
        while(pointer->next != NULL)
	    {
	        pointer=pointer->next;
	    }
        pointer->next = temp;    
    }
    
           
}

ll_node *ll_pop(ll *linkedlist, int n)
{
    int i;
    struct ll_node *temp,*pointer;
    if (n<0)
    {
	n = (linkedlist->size)-1;
    }
    if (linkedlist->head == NULL)
    {
	    printf("null head");
        exit(1);
    }
    if (n==0)
    {
	pointer=linkedlist->head;
	linkedlist->head = pointer->next;
	linkedlist->callbacks.free(pointer);
    }
    else
    {
	pointer=linkedlist->head;
	for(i=0;i<n;i++){temp=pointer;pointer=pointer->next;
	    if(pointer==NULL)
	    {
		    printf("index is bigger than linked list");
            exit(1);
	    }
	}
    temp->next = pointer->next;
    linkedlist->callbacks.free(pointer);
	
    }
    return(pointer);
}

// 0th element is the head
ll_node *ll_get(ll *linkedlist, int n)
{
    int i;
    // struct ll_node *temp,*pointer;
    struct ll_node *pointer;

    if (n<0) {
	    n = (ll_length(linkedlist))-1;
    }

    if (linkedlist->head == NULL) {
        fprintf(stderr, "FATAL ERROR: attempted to access element in uncreated LL\n");
        exit(EXIT_FAILURE);
    }

    if (n==0) {
	    pointer = linkedlist->head;
    } else {
        pointer=linkedlist->head;

        if(pointer==NULL) {
            printf("index is bigger than linked list");
            exit(1);
        }
        
        for(i=0; i<n; i++) {
            pointer = pointer->next;
            if(pointer==NULL) {
                printf("index is bigger than linked list");
                exit(1);
            }
        }
    }

    return pointer;
}

int ll_length(ll *linkedlist)
{
    int count=0;
    struct ll_node *temp;
    if (linkedlist->head == NULL)
    {
        return(count);
    }  
    else
    {
	temp=linkedlist->head;
	while (temp!=NULL)
	{
	    count++;
	    temp = temp->next;
	}
    }
    return (count);
}

void ll_free(ll *linkedlist)
{
    ll_node *current = linkedlist->head;
    ll_node *next;
    while(current!=NULL)
    {
	next = current->next;
	linkedlist->callbacks.free(current);
	current=next;           
    }
    linkedlist->head = NULL;
    free(linkedlist);
}


//prints the pointers to each value.
void ll_printf(ll *linkedlist)
{
    ll_node *current = linkedlist->head;
    if (current!=NULL)
    {
    
	printf("%p",current->value);
    }
    current=current->next;
    while(current!=NULL)
    {
	printf(",%p",current->value);
        current=current->next;
    }
}

void ll_fprintf(ll *linkedlist, FILE *file)
{
    ll_node *current = linkedlist->head;
    if (current!=NULL)
    {
	fprintf(file,"%p",current->value);
    }
    current=current->next;
    while(current!=NULL)
    {
	fprintf(file,",%p",current->value);
        current=current->next;
    }
}

ll *ll_tail(ll *linkedlist, int i) {
    ll *tail = ll_make(linkedlist->size, linkedlist->callbacks);
    tail->head = ll_get(linkedlist, i);
    return tail;
}
