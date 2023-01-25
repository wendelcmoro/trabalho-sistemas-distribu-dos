#ifndef _stack_
#define _stack_

typedef struct Node {
    struct Node *next;
    struct Node *previous;
    int value;
} node;

typedef struct Stack {
    node *bottom;
    node *top;
} stack;

stack *declareStack();
void push(stack *stack, node *node);
void pop(stack *stack);

#endif