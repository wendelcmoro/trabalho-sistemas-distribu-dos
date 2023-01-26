#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

/* --------------------------------------------------------------------------------------
   Disciplina Sistemas Distribuídos
   Objetivo: API de Pilha utilizando Alocação Dinâmica para utilização no algoritmo do vCube
   Autores: Wendel Caiu Moro e Bruno Augusto Luvizott
   Data da ultima atualizacao: 25/01/2023
----------------------------------------------------------------------------------------*/

/* Inicializa a Pilha */
stack *declareStack() {
    stack *stack = malloc(sizeof(stack));

    stack->bottom = NULL;
    stack->top = NULL;

    return stack;
}

/* Inclui um elemento nodo na pilha */
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

/* remove o elemento do topo da pilha */
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