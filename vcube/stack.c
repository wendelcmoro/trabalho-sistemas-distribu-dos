#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

stack *declareStack() {
    stack *stack = malloc(sizeof(stack));

    stack->bottom = NULL;
    stack->top = NULL;

    return stack;
}

void push(stack *stack, node *new) {

    if (stack->bottom == NULL) {
        new->next = NULL;
        new->previous = NULL;
        stack->bottom = new;
        stack->top = new;

        return;
    }

    stack->top->next = new;
    new->previous = stack->top;
    new->next = NULL;
    stack->top = new;

    return;
}

void pop(stack *stack) {
    node* toRemove = stack->top;

    if (toRemove == stack->bottom) {
        stack->top = NULL;
        stack->bottom = NULL;

        if (toRemove != NULL) {
            free(toRemove);
        }

        return;
    }

    stack->top = toRemove->previous;
    stack->top->next = NULL;

    free(toRemove);

    return;
}