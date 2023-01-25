/* --------------------------------------------------------------------------------------
   Disciplina Sistemas Distribuídos
   Objetivo: Implementação do VCube
   Restricoes: O programa assume que o usuario entrou com os valores corretos, inteiros.

   Autores: Wendel Caiu Moro e Bruno Augusto Luvizott
   Disciplina: Sistemas Distribuidos 2022-2
   Data da ultima atualizacao: 22/01/2023
----------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "cisj.h"
#include "smpl.h"
#include "math.h"
#include "stack.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

/* cada processo conta seu tempo */

/*---- aqui definimos os eventos ----*/
#define test 1
#define fault 2
#define recovery 3

/*---- declaramos agora o TipoProcesso ----*/
typedef struct
{
    int id; /* identificador de facility SMPL */
    /* variáveis locais do processo são declaradas aqui */
    int nextTest; // indica qual o prox. processo que irá ser testado
    int *state;   // vetor State
    int *tests;   // vetor de quem testa o processo
    int nodeRound;
} TipoProcesso;

TipoProcesso *processo;

int main(int argc, char *argv[])
{
    stack *pilha = declareStack();

    int round = 1;

    int tests = 0;

    static int N, /* number of nodes is parameter */
        token,    /* node identifier, natural number */
        event,
        r,
        i;

    int lastTested;
    static char fa_name[5];

    if (argc != 2)
    {
        puts("Uso correto: tempo <num-processos>");
        exit(1);
    }

    N = atoi(argv[1]);
    smpl(0, "Um Exemplo de Simulação");
    reset();
    stream(1);

    int clusters = ceil(log2(N));

    printf(ANSI_COLOR_YELLOW "\n\n--> Número de clusters: %d\n", clusters);
    printf("--> Número de processos: %d\n" ANSI_COLOR_RESET, N);

    node_set *nodes;
    node_set *nodesAux;

    /*----- inicializacao -----*/
    processo = (TipoProcesso *)malloc(sizeof(TipoProcesso) * N);

    for (i = 0; i < N; i++)
    {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
        processo[i].state = (int *)malloc(sizeof(int) * N);
        memset(processo[i].state, -1, N * sizeof(int));
        processo[i].nodeRound = 1;
    }

    int *global_state = (int *)malloc(sizeof(int) * N);
    memset(global_state, 0, N * sizeof(int));

    int total_faults = 1;
    int total_recovery = 1;
    int total_intervals = 3;
    int detectNextRound = 0;
    int events = 0;

    int old_latency = 0;

    printf("\n\n");

    /*----- Escalonando eventos(altere aqui os eventos para testes) -----*/

    for (i = 0; i < N; i++)
    {
        schedule(test, 30.0, i);
    }
    total_intervals--;

    /*------------------------------------------------------------------*/

    /*----- Escalonando eventos de falha -------------------------------*/

    srand(0);
    int rand_process = 0;

    if (total_faults > 0)
    {
        schedule(fault, 20.0, rand_process);
    }

    /*------------------------------------------------------------------*/

    /*----- Escalonando eventos de recuperação -------------------------*/

    // schedule(recovery, 90.0, 0);

    /*------------------------------------------------------------------*/

    /*----- Switch Case de eventos -----*/
    while (time() < 4000000.0)
    {
        cause(&event, &token);
        switch (event)
        {
        case test:

            detectNextRound++;

            printf(ANSI_COLOR_CYAN "Processo %d\n" ANSI_COLOR_RESET, token);
            processo[token].nodeRound++;
            if (status(processo[token].id) != 0)
            {
                printf("[Tempo:%6.1f]" ANSI_COLOR_RED " ESTOU FALHO! \n\n" ANSI_COLOR_RESET, time());
                break; // processo falho não testa!
            }

            processo[token].state[token] = 0;

            for (int s = 1; s <= clusters; s++)
            {
                nodesAux = cis(token, s);

                // Não testar processos que não existem
                if (nodesAux->nodes[0] >= N)
                    continue;

                tests++;

                if (status(processo[nodesAux->nodes[0]].id) == 0)
                {
                    printf("[Tempo:%6.1f] O processo %d testou o processo %d " ANSI_COLOR_GREEN "CORRETO  \n" ANSI_COLOR_RESET, time(), token, nodesAux->nodes[0]);

                    if (processo[token].state[nodesAux->nodes[0]] % 2 != 0)
                        processo[token].state[nodesAux->nodes[0]]++;

                    for (int m = (token + 1) % N; m != token; m++, m %= N)
                    {
                        if (processo[nodesAux->nodes[0]].state[m] > processo[token].state[m])
                            processo[token].state[m] = processo[nodesAux->nodes[0]].state[m];
                    }
                }
                else
                {
                    printf("[Tempo:%6.1f] O processo %d testou o processo %d" ANSI_COLOR_RED " INCORRETO \n" ANSI_COLOR_RESET, time(), token, nodesAux->nodes[0]);

                    if (processo[token].state[nodesAux->nodes[0]] == -1)
                        processo[token].state[nodesAux->nodes[0]] = 1;

                    if ((processo[token].state[nodesAux->nodes[0]] % 2) != 1)
                        processo[token].state[nodesAux->nodes[0]]++;
                }

                int aux;
                int alvo;
                for (int k = 0; k < N; k++)
                {

                    if ((processo[token].state[k] % 2) == 1)
                    {

                        nodes = cis(k, s);
                        alvo = nodes->nodes[0];

                        set_free(nodes);
                        nodes = cis(alvo, s);

                        for (int m = 0; m < nodes->size; m++)
                        {
                            aux = nodes->nodes[m];

                            if (aux == token && alvo != token)
                            {
                                tests++;
                                if (status(processo[alvo].id) == 0)
                                {
                                    printf("[Tempo:%6.1f] O processo %d testou o processo %d " ANSI_COLOR_GREEN "CORRETO  \n" ANSI_COLOR_RESET, time(), token, alvo);

                                    if (processo[token].state[alvo] % 2 != 0)
                                        processo[token].state[alvo]++;

                                    for (int m = (token + 1) % N; m != token; m++, m %= N)
                                        if (processo[alvo].state[m] > processo[token].state[m])
                                            processo[token].state[m] = processo[alvo].state[m];
                                }
                                else
                                {
                                    printf("[Tempo:%6.1f] O processo %d testou o processo %d " ANSI_COLOR_RED "INCORRETO \n" ANSI_COLOR_RESET, time(), token, alvo);

                                    if (processo[token].state[alvo] % 2 != 1)
                                        processo[token].state[alvo]++;
                                }
                            }
                            else
                            {
                                if ((processo[token].state[aux] % 2) == 0 || processo[token].state[aux] == -1)
                                    break;
                            }
                        }
                        set_free(nodes);
                    }
                }
                set_free(nodesAux);
            }

            printf("\n");

            // Latencia: Verificando se foi detectado o evento por todos os processos
            int verify_latency = 1;
            if (events > 0)
            {
                for (i = 0; i < N; i++)
                    for (int j = 0; j < N; j++)
                        if (global_state[j] > 0 && j != i)
                            if (processo[i].state[j] != global_state[j])
                                verify_latency = 0;

                if (verify_latency == 1)
                {
                    events--;
                    rand_process = rand() % N;

                    if (total_faults > 0)
                        schedule(fault, time() + 20, rand_process);

                    else if (total_recovery > 0)
                    {
                        // remover da pilha o token e colocar para recovery
                        schedule(recovery, time() + 20, pilha->top->value);
                        pop(pilha);
                    }
                }
            }
            else
                verify_latency = 0;

            if (verify_latency)
            {
                printf(ANSI_COLOR_YELLOW "Latência do último evento: %d\n\n" ANSI_COLOR_RESET, round - old_latency);
                old_latency = round;
            }

            if (detectNextRound == N)
            {
                detectNextRound = 0;

                // Escalona os proximos processos
                if (total_intervals)
                {
                    total_intervals--;
                    for (i = 0; i < N; i++)
                        schedule(test, time() + 30, i);
                }
                printf(ANSI_COLOR_CYAN "Vetor states na rodada de testes %d:\n" ANSI_COLOR_RESET, round);
                for (i = 0; i < N; i++)
                {
                    printf(ANSI_COLOR_BLUE "Processo %2d" ANSI_COLOR_RESET ": [", i);

                    for (int j = 0; j < N; j++)
                    {
                        if (processo[i].state[j] == -1)
                            printf(" -");
                        else
                            printf("%2d", processo[i].state[j]);
                    }
                    printf(" ]\n");
                }
                printf("Número de testes executado até o momento: %d\n\n", tests);
                round++;
            }

            break;
        case fault:
            r = request(processo[token].id, token, 0);
            if (r != 0)
            {
                puts("Não foi possível falhar o nodo...");
                break;
            }
            total_faults--;
            global_state[token]++;
            events++;
            // Adiciona na pilha o token
            node *node = malloc(sizeof(node));
            node->value = rand_process;
            push(pilha, node);
            printf(ANSI_COLOR_BLUE "EVENTO:" ANSI_COLOR_RESET "\n");
            printf("[Tempo:%6.1f] O processo %d " ANSI_COLOR_RED "FALHOU" ANSI_COLOR_RESET "\n\n", time(), token);
            memset(processo[token].state, -1, N * sizeof(int));

            break;
        case recovery:
            release(processo[token].id, token);
            total_recovery--;
            global_state[token]++;
            events++;
            printf(ANSI_COLOR_BLUE "EVENTO:" ANSI_COLOR_RESET "\n");
            printf("[Tempo:%6.1f] O processo %d " ANSI_COLOR_GREEN "RECUPEROU" ANSI_COLOR_RESET "\n\n", time(), token);
            memset(processo[token].state, -1, N * sizeof(int)); // Reinciando o vetor State
            processo[token].state[token] = 0;
            break;
        }
    }
}