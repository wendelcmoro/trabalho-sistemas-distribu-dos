// Sistemas Distribuídos 2022/2
// Autor: Bruno Augusto Luvizott
// Data da última atualização: 28/11/2022
// Tarefa 2: Cada processo correto executa testes até achar outro processo correto. Lembre-se de tratar o caso em que todos os demais processos estão falhos

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

#define test 1
#define fault 2
#define recovery 3

typedef struct
{
    int id; // identificador do processo, que é a "entidade" sendo simulada, no SMPL "facility"
    // outras variáveis locais que cada processo mantém são declaradas aqui
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

    // Inicializar processos
    TipoProcesso *processo = (TipoProcesso *)malloc(sizeof(TipoProcesso) * N);

    for (int i = 0; i < N; i++)
    {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
    }

    for (int i = 0; i < N; i++)
        schedule(test, 30.0, i);
    schedule(fault, 31.0, 1);
    schedule(recovery, 61.0, 1);

    // Agora vem o loop principal do simulador
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
            int ntoken;
            for (ntoken = (token + 1) % N; ntoken != token; ntoken++, ntoken %= N)
            {
                if (status(processo[ntoken].id) == 0)
                {
                    printf("O processo %d testou o processo %d correto.\n", token, ntoken);
                    break;
                }
                else
                    printf("O processo %d testou o processo %d falho.\n", token, ntoken);
            }

            if (status(processo[ntoken].id) == 1)
            {
                printf("Todos os processos estão falhos!\n");
                exit(1);
            }

            schedule(test, 30.0, token);
            break;
        }
        case fault:
        {
            int r = request(processo[token].id, token, 0);
            if (r != 0)
            {
                puts("Não foi possível falhar o processo.");
                exit(1);
            }
            printf("O processo %d falhou no tempo %4.1f\n", token, time());
            break;
        }
        case recovery:
        {
            release(processo[token].id, token);
            printf("O processo %d recuperou no tempo %4.1f\n", token, time());
            schedule(test, 30.0, token);
            break;
        }
        }
    }

    return 0;
}
