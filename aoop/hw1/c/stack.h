#ifndef __STACK_H__
#define __STACK_H__

#define NULL_STACK  -2147483648
#define EMPTY_STACK -2147483647

struct stack {
    struct element* top;
};

extern void push(struct stack* this, int x);
extern int pop(struct stack* this);
extern struct stack* new_stack();
extern void delete_stack(struct stack* this);

#endif
