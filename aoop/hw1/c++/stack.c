#include <cstdlib>
#include <cstdio>
#include <errno.h>
#include <cstring>
#include "stack.h"

void Stack::clean(){
	element* temp;

    while(this->top != NULL){
        temp = this->top;
        this->top = this->top->next;
        delete temp;
    }
}

Stack::element* Stack::new_element(){
	element* elt = new element;
	if(elt == NULL){
		this->clean();
		perror(strerror(errno));
		exit(errno);
	}
	return elt;
}

void Stack::push(int x){
	if(this->top == NULL){
    	this->top        = this->new_element();
    	this->top->value = x;
    	this->top->next  = NULL;
    }
    else{
    	element* new_elt = this->new_element();
    	new_elt->value   = x;
    	new_elt->next    = this->top;
    	this->top        = new_elt;
    }
}

int Stack::pop(){
	if(this->top == NULL) return EMPTY_STACK;

    element* temp_elt = this->top;
    int pop_value = this->top->value;

    this->top = this->top->next;
    delete temp_elt;
    return pop_value;
}
