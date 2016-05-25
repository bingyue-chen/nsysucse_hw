#ifndef __STACK_H__
#define __STACK_H__

#define EMPTY_STACK -2147483647

class Stack {

private:

    struct element{
        int value;
        element* next;
    };

    element* top;
    void clean();
    element* new_element();

public:

    Stack(){ 
        this->top = NULL; 
    }

    ~Stack(){
        
        element* temp;

        while(this->top != NULL){
            temp = this->top;
            this->top = this->top->next;
            delete temp;
        }
        
    }

    void push(int x);
    int pop();

};

#endif
