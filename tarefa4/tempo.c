/* Disciplina Sistemas Distribuídos 
   Data da Última Modificação: 14/01/2022
   Aluno Wendel Caio Moro
   Tarefa 4
*/

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

/* cada processo conta seu tempo */

/*---- aqui definimos os eventos ----*/
#define test   1
#define fault  2
#define recovery 3

int currentRound = 0;
int tester = -1;

/*---- declaramos agora o TipoProcesso ----*/
typedef struct {
    int id;       /* identificador de facility SMPL */


    /* variáveis locais do processo são declaradas aqui */
    int nextTest; // indica qual o prox. processo que irá ser testado
    int *state; // vetor State
    int *tested; // vetor representando quais outros processos foram testados em uma rodada do testador, 0 representa não-testado e 1 testado
    int proccessRound; // indica em que rodada o processo se encontra
} TipoProcesso;

TipoProcesso *processo; 

int main (int argc, char *argv[]) {
    static int N,      /* number of nodes is parameter */
               token,  /* node identifier, natural number */ 
               event,
               r,
               i;

    int lastTested;

    static char fa_name[5];

    if (argc != 2) {
      puts("Uso correto: tempo <num-processos>");
      exit(1);
    }

    N = atoi(argv[1]);
    smpl(0, "Um Exemplo de Simulação");
    reset();
    stream(1);
 
/*----- inicializacao -----*/

    processo = (TipoProcesso *) malloc(sizeof(TipoProcesso)*N);

    for (i = 0; i < N; i++) {
        memset (fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name,1);
        printf("fa_name = %s, processo[%d].id = %d\n", fa_name, i, processo[i].id);

        // indica com qual processo, o nodo i irá começar testando
        processo[i].nextTest = (i+1) % N;

        processo[i].proccessRound = 0;


        // aloca espaço para o vetor State[] e tested
        processo[i].tested = malloc(sizeof(int)*N);        
        processo[i].state = malloc(sizeof(int)*N);
        printf("Vetor State inicial do processo %d:", i);            
        for (int j = 0; j < N; j++) {
            if (j == i) {
                processo[i].state[j] = 0;
                processo[i].tested[j] = 1;
            }
            else {
                processo[i].state[j] = -1;
                processo[i].tested[j] = 0;
            }

            printf(" %d", processo[i].state[j]);
        }
        printf("\n\n\n\n");

        // aloca espaço para o vetor State[]
        // Inicia todos os valores para -1 com exceção do valor i
        // 0 indica que o processo está correto, -1 desconhecido, e 1 que está falho
    } /* end for */
        
/*----- vamos escalonar os eventos iniciais -----*/

    for (i = 0; i < N; i++) {
        //if (i != 1) {
             schedule(test, 30.0, i);
        //}
    } 

    //schedule(test, 30.0, 0);

    schedule(fault, 50.0, 0);
    // falha todos os nodos após 2 rodadas de testes
    for (i = 2; i < N; i++) {
        //if (i != 1) {
           // schedule(fault, 151.0, i);
        //}
    } 
    // falha no tempo 31 o processo 1
    //schedule(fault, 29.0, 1);

    // recupera no tempo 61 o processo 1
    //  for (i = 2; i < N; i++) {
    //     schedule(recovery, 330.0, i);
    // } 
    schedule(recovery, 330.0, 0);
    
/*----- agora vem o loop principal do simulador -----*/
    printf("Iniciando rodada de testes %d:\n", currentRound);
 
    while (time() < 400.0) {
        cause(&event, &token); 
        switch(event) {
        case test: 
                if (status(processo[token].id) != 0) {
                    printf("processo %d está falho\n", token);
                    // if (token == head) {
                    //     head = (token + 1) % N;
                    //     if (head == token) {
                    //         head = (token + 1) % N;
                    //     }

                    //     printf("A cabeça da fila agora será %d\n", head);
                    // }
                    break; // processo falho não testa!
                }

                // atualiza a rodada de testes
                if (processo[token].proccessRound > currentRound) {
                    currentRound++;

                    printf("\n\n\n\n\n\nIniciando rodada de testes %d:\n", currentRound);
                }

                // se o testador mudou, atualiza o vetor de testandos
                if (tester != token) {
                    for (i = 0; i < N; i++) {
                        if (processo[token].tested[i] != token) {
                            processo[token].tested[i] = 0;
                        }
                    }
                    
                    tester = token;
                }

                //printf ("testador agora é %d\n", token);

                // Se status do processo testado for válido, imprime na tela o teste
                if (status(processo[processo[token].nextTest].id) == 0) {
                    // atualiza em qual rodada de testes o processo está
                    processo[token].proccessRound++;

                    // atualiza vetor de testados
                    processo[token].tested[processo[token].nextTest] = 1;

                    printf("o processo %d testou o processo %d CORRETO no tempo %5.1f\n", token, processo[token].nextTest, time());

                    // obtém o vetor state do processo sendo testado
                    for (i = 0; i < N; i++) {
                        if (processo[token].tested[i] == 0 && i != token) {
                            processo[token].state[i] = processo[processo[token].nextTest].state[i];
                        }
                    }

                    // atualiza testado correspondente no vetor state
                    processo[token].state[processo[token].nextTest] = 0;
                    printf("Vetor State do processo %d:", token);
                    for (i = 0; i < N; i++) {
                        printf(" %d", processo[token].state[i]);
                    }
                    printf("\n");

                    // escalona o próximo processo correto a testar e reseta nextTest para o padrão daquele nodo
                    schedule(test, 30.0, processo[token].nextTest);
                    //printf("o próximo processo à testar será %d\n", processo[token].nextTest);
                    processo[token].nextTest = (token + 1) % N;
                }
                // se não encontrou um processo correto, então continua testando
                else {
                    // atualiza vetor de testados
                    processo[token].tested[processo[token].nextTest] = 1;

                    // atualiza testado correspondente no vetor state
                    processo[token].state[processo[token].nextTest] = 1;

                    lastTested = processo[token].nextTest;

                    // indica qual será o próximo processo a ser testado
                    processo[token].nextTest = (processo[token].nextTest + 1) % N;
                    if (processo[token].nextTest == token) {
                        processo[token].nextTest = (processo[token].nextTest + 1) % N;
                    }

                    printf("O processo %d testou o processo %d INCORRETO no tempo %5.1f, o próximo processo a ser testado por %d será %d\n", token, time(), lastTested, token, processo[token].nextTest);
                    printf("Vetor State do processo %d:", token);
                    for (i = 0; i < N; i++) {
                        printf(" %d", processo[token].state[i]);
                    }
                    printf("\n");
                    schedule(test, 30.0, token);
                }
                break;
        case fault:
                r = request(processo[token].id, token, 0);
                if (r != 0) {
                    puts("Não foi possível falhar o nodo...");
                    break;
                }
                printf("o processo %d falhou no tempo %5.1f\n", token, time());
                break;
        case recovery:
                release(processo[token].id, token);
                printf("o processo %d recuperou no tempo %5.1f\n", token, time());
                //schedule(test, 30.0, token);
                break;
        } /* end switch */
    } /* end while */
} /* end tempo.c */

