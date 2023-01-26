/* --------------------------------------------------------------------------------------
   Disciplina Sistemas Distribuídos
   Objetivo: Implementação do VCube
   Restricoes: O programa assume que o usuario entrou com os valores corretos, inteiros.

   Autores: Wendel Caiu Moro e Bruno Augusto Luvizott
   Data da ultima atualizacao: 25/01/2023
----------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "cisj.h"
#include "smpl.h"
#include "math.h"
#include "stack.h"

#define ANSI_COLOR_RED ""
#define ANSI_COLOR_GREEN ""
#define ANSI_COLOR_YELLOW ""
#define ANSI_COLOR_BLUE ""
#define ANSI_COLOR_MAGENTA ""
#define ANSI_COLOR_CYAN ""
#define ANSI_COLOR_RESET ""

// #define ANSI_COLOR_RED "\x1b[31m"
// #define ANSI_COLOR_GREEN "\x1b[32m"
// #define ANSI_COLOR_YELLOW "\x1b[33m"
// #define ANSI_COLOR_BLUE "\x1b[34m"
// #define ANSI_COLOR_MAGENTA "\x1b[35m"
// #define ANSI_COLOR_CYAN "\x1b[36m"
// #define ANSI_COLOR_RESET "\x1b[0m"

/* cada processo conta seu tempo */

/*---- aqui definimos os eventos ----*/
#define test 1
#define fault 2
#define recovery 3

int detectNextRound = 0; // Verifica se a rodada acabou
int max_intervals;       // define um limite máximo de intervalos caso o sistemas ainda não tenha concluido a latencia de todos os eventos
int total_faults;
int total_recovery;
int events;
int current_round = 1;
int tests;
stack *pilha;
int *global_state;
int rand_process = 0;

/*---- declaramos agora o TipoProcesso ----*/
typedef struct
{
    int id; /* identificador de facility SMPL */
    /* variáveis locais do processo são declaradas aqui */
    int nextTest; // indica qual o prox. processo que irá ser testado
    int *state;   // vetor State
} TipoProcesso;

TipoProcesso *processo;

void printStates(int N)
{
    int i;
    detectNextRound = 0;

    // Escalona os proximos processos
    if (max_intervals && (events || total_faults || total_recovery))
    {
        max_intervals--;
        for (i = 0; i < N; i++)
            schedule(test, time() + 30, i);
    }
    printf(ANSI_COLOR_CYAN "Vetor states na rodada de testes %d:\n" ANSI_COLOR_RESET, current_round);
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
    current_round++;
}

int verifyLatency(int N)
{
    int verify_latency = 1;
    int i;
    if (events > 0)
    {
        for (i = 0; i < N; i++)
        {
            if (global_state[i] % 2 == 1)
                continue;

            for (int j = 0; j < N; j++)
                if (global_state[j] > 0 && j != i)
                    if (processo[i].state[j] != global_state[j])
                        verify_latency = 0;
        }

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

    // printf("eventos %d\n\n\n\n\n\n", events);
    return verify_latency;
}

int main(int argc, char *argv[])
{
    pilha = declareStack();

    static int N, /* number of nodes is parameter */
        token,    /* node identifier, natural number */
        event,
        r,
        i,
        old_latency = 0; // Marca a ultima latencia para verifica a latencia do novo evento

    static char fa_name[5];

    if (argc != 5)
    {
        puts("Uso correto: ./vcube <num-processos> <num-intervalos> <num-falhas> <num-recuperacoes>");
        exit(1);
    }

    N = atoi(argv[1]);
    max_intervals = atoi(argv[2]);
    total_faults = atoi(argv[3]);
    total_recovery = atoi(argv[4]);

    if ((total_faults < total_recovery))
    {
        puts("Uso correto: <num-recuperacoes> nao pode ser maior que <num-falhas>");
        exit(1);
    }

    smpl(0, "VCUBE");
    reset();
    stream(1);

    int clusters = ceil(log2(N)); // Número de clusters
    printf(ANSI_COLOR_YELLOW "\n\n--> Número de clusters: %d\n", clusters);
    printf("--> Número de processos: %d\n\n" ANSI_COLOR_RESET, N);

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
    }
    global_state = (int *)malloc(sizeof(int) * N); // Vetor global contendo um gabarito da situação dos processos
    memset(global_state, 0, N * sizeof(int));

    /*----- Escalonando eventos de teste para o inicio da simulaçao -----*/
    for (i = 0; i < N; i++)
        schedule(test, 30.0, i);
    /*------------------------------------------------------------------*/

    /*----- Escalonando o primeiro evento de falha (caso exista) -------------------------------*/
    srand(0);
    if (total_faults)
        schedule(fault, 20.0, rand_process);
    /*------------------------------------------------------------------*/

    /*----- Switch Case de eventos -----*/
    while (time() < 4000000.0)
    {
        cause(&event, &token);
        switch (event)
        {
        // Caso evento seja um teste
        case test:

            detectNextRound++;

            printf(ANSI_COLOR_CYAN "Processo %d\n" ANSI_COLOR_RESET, token);
            if (status(processo[token].id) != 0)
            {
                printf("[Tempo:%6.1f]" ANSI_COLOR_RED " ESTOU FALHO! \n\n" ANSI_COLOR_RESET, time());

                // Caso o processo falho seja o ultimo da rodada
                /* Verifica se uma rodada de testes terminou, caso sim imprime o vetor State de cada processo e o total de testes executados até então */
                if (detectNextRound == N)
                {
                    // Latencia: Verificando se foi detectado o evento por todos os processos

                    int verify_latency = verifyLatency(N);
                    if (verify_latency)
                    {
                        printf(ANSI_COLOR_YELLOW "Latência do último evento: %d\n\n" ANSI_COLOR_RESET, current_round - old_latency);
                        old_latency = current_round;
                    }
                    printStates(N);
                }
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

                /* O processo responsável, testa os processos em cada cluster de acordo com a função cis */
                if (status(processo[nodesAux->nodes[0]].id) == 0) // Caso o processo testado estiver correto
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
                else // Caso o processo testado esteja falho
                {
                    printf("[Tempo:%6.1f] O processo %d testou o processo %d" ANSI_COLOR_RED " INCORRETO \n" ANSI_COLOR_RESET, time(), token, nodesAux->nodes[0]);

                    if (processo[token].state[nodesAux->nodes[0]] == -1)
                        processo[token].state[nodesAux->nodes[0]] = 1;

                    if ((processo[token].state[nodesAux->nodes[0]] % 2) != 1)
                        processo[token].state[nodesAux->nodes[0]]++;
                }

                /* Ao identificar um processo falho, verifia se ele é o proximo processo correto da lista de testadores dos outros processos de com a função cis */
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

            /* Verifica se uma rodada de testes terminou, caso sim imprime o vetor State de cada processo e o total de testes executados até então */
            if (detectNextRound == N)
            {
                // Latencia: Verificando se foi detectado o evento por todos os processos
                int verify_latency = verifyLatency(N);
                if (verify_latency)
                {
                    printf(ANSI_COLOR_YELLOW "Latência do último evento: %d\n\n" ANSI_COLOR_RESET, current_round - old_latency);
                    old_latency = current_round;
                }
                printStates(N);
            }
            break;

        // Caso evento seja uma falha
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

        // Caso evento seja uma recuperação
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