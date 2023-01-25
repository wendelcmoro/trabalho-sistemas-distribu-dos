/* Disciplina Sistemas Distribuídos 
   Data da Última Modificação: 25/01/2022
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

/*---- declaramos agora o TipoProcesso ----*/
typedef struct {
    int id;       /* identificador de facility SMPL */


    /* variáveis locais do processo são declaradas aqui */
    int nextTest; // indica qual o prox. processo que irá ser testado


    // vetor State
    int *state;
} TipoProcesso;

TipoProcesso *processo; 

int main (int argc, char *argv[]) {
    static int N,      /* number of nodes is parameter */
               token,  /* node identifier, natural number */ 
               event,
               r,
               i;

    int lastTested;
    
    int showStates = 0;

    int testers[N];
    for (i = 0; i < N; i++) {
        testers[i] = 0;
    }

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
    printf("Executando Tarefa-4\n\n");
    printf("Simulando vRing com %d processos\n\n", N);
    processo = (TipoProcesso *) malloc(sizeof(TipoProcesso)*N);
    printf("Vetores State iniciais dos processos: \n");    
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
        processo[i].state = malloc(sizeof(int)*N);        
        printf("Processo %d: ", i);   
        for (int j = 0; j < N; j++) {
            if (j == i) {
                processo[i].state[j] = 0;
            }
            else {
                processo[i].state[j] = -1;
            }

            if (processo[i].state[j] == -1) {
                printf(" -");
            }
            else {
                printf(" %d", processo[i].state[j]);
            }
        }
        printf("\n");
    } /* end for */
    printf("\n");
        
    /*----- Escalona os testes -----*/
    float nextInterval = 30;
    for (i = 0; i < N; i++) {
       schedule(test, nextInterval, i);
       nextInterval += 1;
    }

    /*----- Escalona os falhos -----*/
    for (i = 1; i < N; i++) {
        schedule(fault, 29.0, i);
    }

    /*----- Escalona as recuperações -----*/
    nextInterval = 119;
    for (i = 1; i < N; i++) {
        schedule(recovery, nextInterval, i);
        nextInterval += 20;
    }
    
    /*----- loop principal do simulador -----*/
    while (time() < 400.0) {
        cause(&event, &token); 
        switch(event) {
        case test:              
                testers[token] = 1;  

                if (status(processo[token].id) != 0) {
                    break; // processo falho não testa!
                }

                // Se status do processo testado for válido, imprime na tela o teste
                if (status(processo[processo[token].nextTest].id) == 0) {
                    printf("o processo %d testou o processo %d CORRETO no tempo %5.1f\n", token, processo[token].nextTest, time());
                    
                    if (processo[token].state[processo[token].nextTest] % 2 == 1) {
                        processo[token].state[processo[token].nextTest]++;
                    }

                    if (processo[token].state[processo[token].nextTest] == -1) {
                        processo[token].state[processo[token].nextTest] = 0;
                    }

                    for (i = (processo[token].nextTest + 1) % N; i != token; i++, i = i % N) {
                        processo[token].state[i] = processo[processo[token].nextTest].state[i];
                    }

                    processo[token].nextTest = (token + 1) % N;

                   
                }
                // se não encontrou um processo correto, então continua testando
                else {
                    lastTested = processo[token].nextTest;
                    // indica qual será o próximo processo a ser testado
                    processo[token].nextTest = (processo[token].nextTest + 1) % N;

                    if (processo[token].state[lastTested] % 2 == 0) {
                        processo[token].state[lastTested]++;
                    }

                    if (processo[token].state[lastTested] == -1) {
                        processo[token].state[lastTested] = 1;
                    }
                    
                    // se a soma cair no próprio nodo, significa que já testamos todos os outros processos possíveis
                    if (processo[token].nextTest == token) {
                        processo[token].nextTest = (token + 1) % N;

                        printf("O processo %d testou o processo %d INCORRETO no tempo %5.1f, todos os processos do Anel foram testados falhos\n\n", token, lastTested, time());
                    }
                    else {               
                        printf("O processo %d testou o processo %d INCORRETO no tempo %5.1f, o próximo processo a ser testado por %d será %d\n", token, time(), lastTested, token, processo[token].nextTest);
                        
                        schedule(test, 1, token);
                        break;
                    }
                }

                // Verifica se todos os processos testaram na rodada, se sim imprime o vetor States de cada um
                showStates = 1;
                for (i = 0; i < N; i++) {
                    if (testers[i] == 0) {
                        showStates = 0;
                        break;
                    }
                }

                if (showStates) {
                    printf("\nVetor State de cada processo: \n");
                    for (i = 0; i < N; i++) {
                        printf("Processo %d: ", i);
                        for (int j = 0; j < N; j++) {
                            if (processo[i].state[j] == -1) {
                                printf(" -");
                            }
                            else {
                                printf(" %d", processo[i].state[j]);
                            }
                        }
                        printf("\n");
                    }
                    printf("\n");

                    for (i = 0; i < N; i++) {
                        testers[i] = 0;
                    }
                }
                break;
        case fault:
                r = request(processo[token].id, token, 0);
                if (r != 0) {
                    puts("Não foi possível falhar o nodo...");
                    break;
                }
                printf("o processo %d FALHOU no tempo %5.1f\n", token, time());
                break;
        case recovery:
                release(processo[token].id, token);
                printf("\no processo %d RECUPEROU no tempo %5.1f\n", token, time());
                for (i = 0; i < N; i++) {
                    schedule(test, 1, i);
                }
                break;
        } /* end switch */
    } /* end while */
} /* end tempo.c */

