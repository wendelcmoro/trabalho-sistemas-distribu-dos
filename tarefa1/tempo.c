/* Disciplina Sistemas Distribuídos 
   Data da Última Modificação: 25/01/2022
   Aluno Wendel Caio Moro GRR20182641
   Tarefa 1
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
    printf("Simulando vRing com %d processos\n\n", N);
    processo = (TipoProcesso *) malloc(sizeof(TipoProcesso)*N);
    for (int i = 0; i < N; i++) {
        memset (fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name,1);
        //printf("fa_name = %s, processo[%d].id = %d\n", fa_name, i, processo[i].id);

        // indica com qual processo, o nodo i irá começar testando
        processo[i].nextTest = (i+1) % N;
    } /* end for */
        
    /*----- Escalona os testes -----*/
    int nextInterval = 30;
    for (i = 0; i < N; i++) {
       schedule(test, nextInterval, i);
       nextInterval += 10;
    }

    /*----- Escalona os falhas -----*/
    schedule(fault, 29.0, 1);

    /*----- Escalona os recuperação -----*/
    schedule(recovery, 170.0, 1);
    
    /*----- loop principal do simulador -----*/
    while (time() < 470.0) {
      cause(&event, &token); 
      switch(event) {
       case test: 
            if (status(processo[token].id) != 0) {
                break; // processo falho não testa!
            }

            // Se status do processo testado for válido, imprime na tela o teste
            if (status(processo[processo[token].nextTest].id) == 0) {
                printf("o processo %d testou o processo %d CORRETO no tempo %5.1f\n", token, processo[token].nextTest, time());

                processo[token].nextTest = (token + 1) % N;
            }
            // se não encontrou um processo correto, então continua testando
            else {
                lastTested = processo[token].nextTest;

                // indica qual será o próximo processo a ser testado
                processo[token].nextTest = (processo[token].nextTest + 1) % N;
                
                // se a soma cair no próprio nodo, soma mais 1 para o nextTest
                if (processo[token].nextTest == token) {
                    processo[token].nextTest = (processo[token].nextTest + 1) % N;
                }

                printf("O processo %d testou o processo %d INCORRETO no tempo %5.1f, o próximo processo a ser testado por %d será %d\n", token, time(), lastTested, token, processo[token].nextTest);

                schedule(test, 10.0, token);
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

            nextInterval = time();
            for (i = 0; i < N; i++) {
                schedule(test, nextInterval, i);
                nextInterval += 10;
            }
            break;
      } /* end switch */
    } /* end while */
} /* end tempo.c */

