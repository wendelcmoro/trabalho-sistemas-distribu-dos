/* Disciplina Sistemas Distribuídos
   Data da Última Modificação: 14/01/2022
   Aluno Wendel Caio Moro
   Tarefa 3
*/

// ⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠛⢉⢉⠉⠉⠻⣿⣿⣿⣿⣿⣿
// ⣿⣿⣿⣿⣿⣿⣿⠟⠠⡰⣕⣗⣷⣧⣀⣅⠘⣿⣿⣿⣿⣿
// ⣿⣿⣿⣿⣿⣿⠃⣠⣳⣟⣿⣿⣷⣿⡿⣜⠄⣿⣿⣿⣿⣿
// ⣿⣿⣿⣿⡿⠁⠄⣳⢷⣿⣿⣿⣿⡿⣝⠖⠄⣿⣿⣿⣿⣿
// ⣿⣿⣿⣿⠃⠄⢢⡹⣿⢷⣯⢿⢷⡫⣗⠍⢰⣿⣿⣿⣿⣿
// ⣿⣿⣿⡏⢀⢄⠤⣁⠋⠿⣗⣟⡯⡏⢎⠁⢸⣿⣿⣿⣿⣿
// ⣿⣿⣿⠄⢔⢕⣯⣿⣿⡲⡤⡄⡤⠄⡀⢠⣿⣿⣿⣿⣿⣿
// ⣿⣿⠇⠠⡳⣯⣿⣿⣾⢵⣫⢎⢎⠆⢀⣿⣿⣿⣿⣿⣿⣿
// ⣿⣿⠄⢨⣫⣿⣿⡿⣿⣻⢎⡗⡕⡅⢸⣿⣿⣿⣿⣿⣿⣿
// ⣿⣿⠄⢜⢾⣾⣿⣿⣟⣗⢯⡪⡳⡀⢸⣿⣿⣿⣿⣿⣿⣿
// ⣿⣿⠄⢸⢽⣿⣷⣿⣻⡮⡧⡳⡱⡁⢸⣿⣿⣿⣿⣿⣿⣿
// ⣿⣿⡄⢨⣻⣽⣿⣟⣿⣞⣗⡽⡸⡐⢸⣿⣿⣿⣿⣿⣿⣿
// ⣿⣿⡇⢀⢗⣿⣿⣿⣿⡿⣞⡵⡣⣊⢸⣿⣿⣿⣿⣿⣿⣿
// ⣿⣿⣿⡀⡣⣗⣿⣿⣿⣿⣯⡯⡺⣼⠎⣿⣿⣿⣿⣿⣿⣿
// ⣿⣿⣿⣧⠐⡵⣻⣟⣯⣿⣷⣟⣝⢞⡿⢹⣿⣿⣿⣿⣿⣿
// ⣿⣿⣿⣿⡆⢘⡺⣽⢿⣻⣿⣗⡷⣹⢩⢃⢿⣿⣿⣿⣿⣿
// ⣿⣿⣿⣿⣷⠄⠪⣯⣟⣿⢯⣿⣻⣜⢎⢆⠜⣿⣿⣿⣿⣿
// ⣿⣿⣿⣿⣿⡆⠄⢣⣻⣽⣿⣿⣟⣾⡮⡺⡸⠸⣿⣿⣿⣿
// ⣿⣿⡿⠛⠉⠁⠄⢕⡳⣽⡾⣿⢽⣯⡿⣮⢚⣅⠹⣿⣿⣿
// ⡿⠋⠄⠄⠄⠄⢀⠒⠝⣞⢿⡿⣿⣽⢿⡽⣧⣳⡅⠌⠻⣿
// ⠁⠄⠄⠄⠄⠄⠐⡐⠱⡱⣻⡻⣝⣮⣟⣿⣻⣟⣻⡺⣊

#include <stdio.h>
#include <stdlib.h>
#include "cisj.h"
#include "smpl.h"
#include "math.h"

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

    int *state; // vetor State
    int *tests; // vetor de quem testa o processo

    int nodeRound;
} TipoProcesso;

TipoProcesso *processo;

int main(int argc, char *argv[])
{
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

    int clusters = log2(N);
    printf("Número de clusters: %d\n", clusters);
    printf("Número de processos: %d\n", N);

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
        processo[i].state[i] = 0;
        processo[i].nodeRound = 1;
    }

    printf("\n\n");

    /*----- vamos escalonar os eventos iniciais -----*/
    for (i = 0; i < N; i++)
    {
        schedule(test, 30.0, i);
        schedule(test, 60.0, i);
        //   schedule(test, 100.0, i);
    }

    // // falha no tempo 31 o processo 1
    schedule(fault, 29.0, 0);
    // schedule(fault, 29.0, 3);
    // schedule(fault, 29.0, 6);
    //  schedule(fault, 29.0, 5);
    //  for (i = 1; i < N; i++) {
    //      schedule(recovery, 330.0, i);
    //  }

    /*----- agora vem o loop principal do simulador -----*/

    while (time() < 400.0)
    {
        cause(&event, &token);
        switch (event)
        {
        case test:
            printf("Processo %d\n", token);
            processo[token].nodeRound++;
            if (status(processo[token].id) != 0)
            {
                printf("[%6.1f]" ANSI_COLOR_RED " FALHEI! \n\n" ANSI_COLOR_RESET, time());
                break; // processo falho não testa!
            }

            for (int s = 1; s <= clusters; s++)
            {
                nodesAux = cis(token, s);

                tests++;

                if (status(processo[nodesAux->nodes[0]].id) == 0)
                {
                    printf("o processo %d testou o processo %d " ANSI_COLOR_GREEN "CORRETO  " ANSI_COLOR_RESET " no tempo %5.1f\n", token, nodesAux->nodes[0], time());

                    if (processo[token].state[nodesAux->nodes[0]] % 2 != 0)
                    {
                        processo[token].state[nodesAux->nodes[0]]++;
                    }

                    for (int m = (nodesAux->nodes[0] + 1) % N; m != token; m++, m %= N)
                    {
                        if (processo[nodesAux->nodes[0]].state[m] > processo[token].state[m])
                        {
                            processo[token].state[m] = processo[nodesAux->nodes[0]].state[m];
                        }
                    }
                }
                else
                {
                    printf("o processo %d testou o processo %d" ANSI_COLOR_RED " INCORRETO " ANSI_COLOR_RESET "no tempo %5.1f\n", token, nodesAux->nodes[0], time());

                    if (processo[token].state[nodesAux->nodes[0]] == -1)
                    {
                        processo[token].state[nodesAux->nodes[0]] = 1;
                    }

                    if ((processo[token].state[nodesAux->nodes[0]] % 2) != 1)
                    {
                        processo[token].state[nodesAux->nodes[0]]++;
                    }
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
                                    printf("o processo %d testou o processo %d " ANSI_COLOR_GREEN "CORRETO  " ANSI_COLOR_RESET " no tempo %5.1f\n", token, alvo, time());

                                    if (processo[token].state[alvo] % 2 != 0)
                                    {
                                        processo[token].state[alvo]++;
                                    }
                                }
                                else
                                {
                                    printf("o processo %d testou o processo %d " ANSI_COLOR_RED "INCORRETO" ANSI_COLOR_RESET " no tempo %5.1f\n", token, alvo, time());

                                    if (processo[token].state[alvo] % 2 != 1)
                                    {
                                        processo[token].state[alvo]++;
                                    }
                                }
                            }
                            else
                            {
                                if ((processo[token].state[aux] % 2) == 0)
                                {
                                    break;
                                }
                            }
                        }

                        set_free(nodes);
                    }
                }

                set_free(nodesAux);
            }

            printf("\n");

            int detectNextRound = 1;
            for (i = 0; i < N; i++)
            {
                if (processo[i].nodeRound == round)
                {
                    detectNextRound = 0;
                    break;
                }
            }

            if (detectNextRound == 1)
            {
                int detectLatency = 1;
                printf("Vetor states na rodada de testes %d:\n", round);
                for (i = 0; i < N; i++)
                {
                    printf("Processo %d :", i);

                    for (int j = 0; j < N; j++)
                    {
                        printf("%d ", processo[i].state[j]);

                        if (processo[i].state[j] == -1)
                        {
                            detectLatency = 0;
                        }
                    }

                    printf("\n");
                    if (detectLatency == 1)
                    {
                        printf("Latência: %d\n", round + 1);
                    }
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
            printf("o processo %d falhou no tempo %5.1f\n", token, time());
            break;
        case recovery:
            release(processo[token].id, token);
            printf("o processo %d recuperou no tempo %5.1f\n", token, time());
            // schedule(test, 30.0, token);
            break;
        } /* end switch */
    }     /* end while */
} /* end tempo.c */
