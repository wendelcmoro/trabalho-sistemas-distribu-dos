/* Disciplina Sistemas Distribuídos 
   Data da Última Modificação: 14/01/2022
   Aluno Wendel Caio Moro
   Tarefa 3
*/

#include <stdio.h>
#include <stdlib.h>
#include "cisj.h"
#include "smpl.h"

/* cada processo conta seu tempo */

/*---- aqui definimos os eventos ----*/
#define test   1
#define fault  2
#define recovery 3

/*---- declaramos agora o TipoProcesso ----*/
typedef struct {
    int id;       /* identificador de facility SMPL */


    /* variáveis locais do processo são declaradas aqui */
    int nextTest; // indica qual o prox. processo que irá ser testado

    int *state; // vetor State
    int *tests; // vetor de quem testa o processo
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

    int clusters = log2(N);
    printf("Número de clusters: %d\n", clusters);
    printf("Número de processos: %d\n", N);

    node_set* nodes;
    node_set* nodesAux;
 
    /*----- inicializacao -----*/
    processo = (TipoProcesso *) malloc(sizeof(TipoProcesso)*N);

    for (i = 0; i < N; i++) {
        memset (fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name,1);
        //printf("fa_name = %s, processo[%d].id = %d\n", fa_name, i, processo[i].id);

        // indica com qual processo, o nodo i irá começar testando
        processo[i].nextTest = (i+1) % N;

        // aloca espaço para o vetor State[]
        // Inicia todos os valores para -1 com exceção do valor i
        // 0 indica que o processo está correto, -1 desconhecido, e 1 que está falho        
        processo[i].tests = malloc(sizeof(int)*N);
        processo[i].state = malloc(sizeof(int)*N);
        //printf("Vetor State inicial do processo %d:", i);            
        for (int j = 0; j < N; j++) {
            if (j == i) {
                processo[i].state[j] = 0;
            }
            else {
                processo[i].state[j] = -1;
            }

            processo[i].tests[j] = 0;
            //printf(" %d", processo[i].state[j]);
        }
        //printf("\n\n\n\n");

        for (int s = 1; s <= clusters; s++) {
            for (int j = 0; j < N; j++) {
                if (j != i) {
                    nodes = cis(j, s);
                    if (nodes->nodes[0] == i) {
                        processo[i].tests[j] = 1;
                    }
                    set_free(nodes);
                }                
            }
        }
        
        printf("Processo %d é testado pelos processos: ", i);
        for (int j = 0; j < N; j++) {
            if (processo[i].tests[j] == 1) {
                printf("%d ", j);
            }
        }
        printf("\n");
    } /* end for */

    printf("\n\n");
        
/*----- vamos escalonar os eventos iniciais -----*/
    for (i = 0; i < N; i++) {
        schedule(test, 30.0, i);
        schedule(test, 40.0, i);
        schedule(test, 100.0, i);
    }

    // // falha no tempo 31 o processo 1
    schedule(fault, 29.0, 2);
    schedule(fault, 29.0, 4);
    schedule(fault, 29.0, 5);
    // for (i = 1; i < N; i++) {
    //     schedule(recovery, 330.0, i);
    // } 
    
/*----- agora vem o loop principal do simulador -----*/

    while (time() < 400.0) {
        cause(&event, &token); 
        switch(event) {
            case test: 
                    printf("Processo %d\n", token);
                    if (status(processo[token].id) != 0) {
                        break; // processo falho não testa!
                    }

                    for (i = 0; i < N; i++) {
                        if (i != token) {
                            if (processo[token].tests[i] == 1) {
                                if (status(processo[i].id) != 0) {
                                    printf("o processo %d testou o processo %d INCORRETO no tempo %5.1f\n", token, i, time());

                                    processo[token].tests[i] = 0;
                                    if (processo[token].state[i] == -1) {
                                        processo[token].state[i] = 1;
                                    }
                                    else {
                                        processo[token].state[i]++;
                                    }

                                    for (int s = 1; s <= clusters; s++) {
                                        for (int j = 0; j < N; j++) {
                                            if (j != token && j != i) {
                                                nodes = cis(j, s);
                                                if (nodes->nodes[0] == i) {
                                                    processo[token].tests[j] = 1;
                                                }
                                                set_free(nodes);
                                            }                
                                        }
                                    }

                                }
                                else {
                                    printf("o processo %d testou o processo %d CORRETO no tempo %5.1f\n", token, i, time());

                                    processo[token].state[i] = 0;

                                    for (int m = i; m < N; m++) {
                                        if (processo[i].state[m] > processo[token].state[m]) {
                                            processo[token].state[m] = processo[i].state[m];
                                        }
                                    }

                                    for (int m = 0; m < token; m++) {
                                        if (processo[i].state[m] > processo[token].state[m]) {
                                            processo[token].state[m] = processo[i].state[m];
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // for (int s = 1; s <= clusters; s++) {
                    //     nodesAux = cis(token, s);
                        
                        
                        // for (int j = 0; j < nodesAux->size; j++) {
                        //     nodes = cis(nodesAux->nodes[j], s);

                        //     int found = 0;
                        //     for (int k = 0; k < nodes->size; k++) {
                        //         if (nodes->nodes[k] == token) {
                        //             if (processo[token].state[nodesAux->nodes[j]] == -1 || processo[token].state[nodesAux->nodes[j]] == 0) {
                        //                 if (status(processo[nodesAux->nodes[j]].id) != 0) {
                        //                     printf("o processo %d testou o processo %d INCORRETO no tempo %5.1f\n", token, nodesAux->nodes[j], time());
                        //                     if (processo[token].state[nodesAux->nodes[nodesAux->nodes[j]]] == -1) {
                        //                         processo[token].state[nodesAux->nodes[nodesAux->nodes[j]]] = 1;
                        //                     }
                        //                     else {
                        //                         processo[token].state[nodesAux->nodes[nodesAux->nodes[j]]]++;
                        //                     }

                        //                     for (int l = 0; l < N; l++) {
                        //                         if (l != token) {
                        //                             processo[token].tests[l] = 1;
                        //                         }
                        //                     }
                        //                     break;
                        //                 }
                        //                 else {
                        //                     printf("o processo %d testou o processo %d CORRETO no tempo %5.1f\n", token, nodesAux->nodes[j], time());

                        //                     processo[token].state[nodesAux->nodes[j]] = 0;

                        //                     for (int m = nodesAux->nodes[j]; m < N; m++) {
                        //                         processo[token].state[m] = processo[nodes->nodes[k]].state[m];
                        //                     }

                        //                     for (int m = 0; m < token; m++) {
                        //                         processo[token].state[m] = processo[nodes->nodes[k]].state[m];
                        //                     }

                        //                     found++;
                        //                     break;
                        //                 }
                        //             }
                        //         }
                        //     }
                        //     set_free(nodes);

                        //     if (found != 0) {
                        //         break;
                        //     }
                        // }
                    //}

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

