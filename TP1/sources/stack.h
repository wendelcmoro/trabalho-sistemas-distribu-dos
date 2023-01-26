#ifndef _stack_
#define _stack_

/* --------------------------------------------------------------------------------------
   Disciplina Sistemas Distribuídos
   Objetivo: Arquivo Header de API de Pilha utilizando Alocação Dinâmica para utilização no algoritmo do vCube
   Autores: Wendel Caiu Moro e Bruno Augusto Luvizott
   Data da ultima atualizacao: 25/01/2023
----------------------------------------------------------------------------------------*/

/* Estrutura nodo para utilização da pilha, contém ponteiros para os elementos próximo e anterior, contém um valor inteiro para identificação de um elemento */
typedef struct Node {
    struct Node *next;
    struct Node *previous;
    int value;
} node;

/* Estrutura pilha, contém ponteiros para os elementos do topo e base da pilha */
typedef struct Stack {
    node *bottom;
    node *top;
} stack;

stack *declareStack(); // inicializa uma pilha
void push(stack *stack, node *node); // insere um nodo na pilha
void pop(stack *stack); // remove o elemento do topo da pilha

#endif