// Sistemas Distribuídos 2022/2
// Autor: Bruno Augusto Luvizott
// Data da última atualização: 28/11/2022
// Tarefa 3:Cada processo mantém localmente o vetor State[N].
// A entrada do vetor State[j] indica o estado do processo j. O estado de cada processo pode ser: -1 (unknown), 0 (correto) ou 1 (falho).
// Inicialize (para todos os processos) o State[N] com -1 (indicando estado “unknown”) para todos os demais processos e 0 para o próprio processo

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

#define test 1
#define fault 2
#define recovery 3

typedef struct
{
    int id; // identificador do processo, que é a "entidade" sendo simulada, no SMPL "facility"
    int *state;
} TipoProcesso;

int main(int argc, char *argv[])
{

    static int N,    // Número de processos do sistema distribuído
        token,       // indica o processo que esta sendo executado
        event, r, i; //

    static char fa_name[5];

    if (argc != 2)
    {
        puts("Uso correto: tempo <número de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    if (!(N >= 2))
    {
        puts("O número de processos deve ser um inteiro maior ou igual a 2");
        exit(1);
    }

    smpl(0, "VRing");
    reset();
    stream(1);

    printf("Número de processos = %d \n\n", N);

    // inicializar processos
    TipoProcesso *processo = (TipoProcesso *)malloc(sizeof(TipoProcesso) * N);

    for (int i = 0; i < N; i++)
    {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
        processo[i].state = (int *)malloc(sizeof(int) * N);
        memset(processo[i].state, -1, N * sizeof(int));
        processo[i].state[i] = 0;
    }

    for (int i = 0; i < N; i++)
        schedule(test, 30.0, i);
    schedule(fault, 31.0, 1);
    schedule(recovery, 61.0, 1);

    // agora vem o loop principal do simulador
    while (time() < 150.0)
    {
        cause(&event, &token);
        switch (event)
        {
        case test:
        {
            if (status(processo[token].id) != 0)
                break;
            printf("O processo %d vai testar no tempo %4.1f\n", token, time());

            // Teste
            int ntoken, parity;
            for (ntoken = (token + 1) % N; ntoken != token; ntoken++, ntoken %= N)
            {
                parity = status(processo[ntoken].id) == 0 ? 0 : 1;

                if (processo[token].state[ntoken] == -1)
                    processo[token].state[ntoken] = parity;
                else if (processo[token].state[ntoken] % 2 != parity)
                    processo[token].state[ntoken]++;

                printf("O processo %d testou o processo %d %s.\n", token, ntoken, parity == 0 ? "correto" : "falho");

                if (parity == 0)
                    break;
            }

            printf("O vetor de testes do processo %d agora é: [", token);
            for (int i = 0; i < N; i++) // Print do vetor
            {
                if (processo[token].state[i] == -1)
                    printf(" -");
                else
                    printf("%2d", processo[token].state[i]);
            }
            printf(" ]\n");

            schedule(test, 30.0, token);
            break;
        }
        case fault:
        {
            int r = request(processo[token].id, token, 0);
            printf("O processo %d falhou no tempo %4.1f\n", token, time());
            break;
        }
        case recovery:
        {
            release(processo[token].id, token);
            printf("O processo %d recuperou no tempo %4.1f\n", token, time());
            memset(processo[token].state, -1, N * sizeof(int)); // Reinciando o vetor State
            processo[token].state[token] = 0;
            schedule(test, 30.0, token);
            break;
        }
        }
    }

    return 0;
}
