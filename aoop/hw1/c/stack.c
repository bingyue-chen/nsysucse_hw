#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "stack.h"

struct element{
    int value;
    struct element* next;
};

static struct element* new_element(struct stack* this);


void push(struct stack* this, int x)
{
    if(this->top == NULL){
    	this->top        = new_element(this);
    	this->top->value = x;
    	this->top->next  = NULL;
    }
    else{
    	struct element* new_elt = new_element(this);
    	new_elt->value = x;
    	new_elt->next  = this->top;
    	this->top      = new_elt;
    }
}

int pop(struct stack* this)
{
    if(this == NULL) return NULL_STACK;
    if(this->top == NULL) return EMPTY_STACK;

    struct element* temp_elt = this->top;
    int pop_value = this->top->value;

    this->top = this->top->next;
    free(temp_elt);
    return pop_value;
}

struct stack* new_stack()
{
    struct stack* stk = malloc(sizeof(struct stack));

    if(stk == NULL){
    	perror(strerror(errno));
    	exit(errno);
    }

    stk->top = NULL;

    return stk;
}

void delete_stack(struct stack* this)
{
    struct element* current;
    if(this != NULL){
        while(this->top != NULL){
        	current   = this->top;
        	this->top = this->top->next;
        	free(current);
        }
        free(this);
    }
}

struct element* new_element(struct stack* this)
{
	
	struct element* elt = malloc(sizeof(struct element));
	
	if(elt == NULL){
        delete_stack(this);
    	perror(strerror(errno));
        exit(errno);
    }

    return elt;
}
