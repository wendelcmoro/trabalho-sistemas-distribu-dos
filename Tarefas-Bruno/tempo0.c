// Sistemas Distribuídos 2022/2
// Autor: Bruno Augusto Luvizott
// Data da última atualização: 28/11/2022
// Funcionalidade: tempo0.c -> os processos simplesmente contam o tempo
// Tarefa 0: digitar, compilar e executar o programa exemplo, tempo.c

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

// Vamos definir os eventos

#define test 1
#define fault 2
#define recovery 3

// vamos definir um processo, o descritor do processo

typedef struct
{
    int id; // identificador do processo, que é a "entidade" sendo simulada, no SMPL "facility"
    // outras variáveis locais que cada processo mantém são declaradas aqui
} TipoProcesso;

TipoProcesso *processo;

// Corpo principal do programa

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
    smpl(0, "Um exemplo de simulação");
    reset();
    stream(1);

    // Vamos inicializar os processos
    processo = (TipoProcesso *)malloc(sizeof(TipoProcesso) * N);

    for (int i = 0; i < N; i++)
    {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
    }

    // Vamos agora fazer o escalonamento inicial de eventos

    for (int i = 0; i < N; i++)
        schedule(test, 30.0, i);
    schedule(fault, 31.0, 1);
    schedule(recovery, 61.0, 1);

    // Agora vem o loop principal do simulador

    puts("Boa noite, sou o simulador SMPL tempo.c");

    while (time() < 150.0)
    {
        cause(&event, &token);
        switch (event)
        {
        case test:
            if (status(processo[token].id) != 0)
                break; // processo falho não testa
            printf("O processo %d vai testar no tempo %4.1f\n", token, time());
            schedule(test, 30.0, token);
            break;

        case fault:
            r = request(processo[token].id, token, 0);
            printf("O processo %d falhou no tempo %4.1f\n", token, time());
            break;

        case recovery:
            release(processo[token].id, token);
            printf("O processo %d recuperou no tempo %4.1f\n", token, time());
            schedule(test, 30.0, token);
            break;
        }
    }
}
