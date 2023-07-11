# Trabalho de Sistemas Distribuídos - Implementação do vCube

O trabalho foi escrito em **C**, utilizando como base o arquivo **tempo.c** disponibilizado pelo professor. Utilizamos a biblioteca **simpl.h**, também disponibilizada pelo professor, juntamente do arquivo **cisj.c**. Adicionalmente, criamos uma biblioteca auxiliar para manusear pilhas utilizando estrutura de dados com alocação dinâmica com arquivo de nome **stack.c**. O arquivo principal do programa é o **vcube.c**, nele que imprimimos todos os testes, além de agendamentos e eventos de teste. O trabalho contém um **makefile**, já configurado para a compilação.

## Alunos

* Wendel Caio Moro (GRR20182641 wcm18)
* Bruno Augusto Luvizott (GRR20180112 bal18)

## Arquivos

### vcube.c

Arquivo principal do trabalho. O programa executa uma simulação do vcube, indicando em seu início a quantidade de processos e a quantidade de clusters. O programa espera como entrada o **número de processos**, o número **máximo de intervalos**, a **quantidade de falhas** a serem agendadas e a **quantidade de recuperações** a serem agendadas. Os números de falhas são feitos a partir da função **rand()**, então processos aleatórios devem ser agendados para falharem.

A entrada **máximo de intervalos** limita quantos intervalos de testes por processo serão executados caso ainda não seja detectado o último evento.

Arquivo:

```vcube.c.txt(link aqui)
```

o programa pode ser executado da seguinte maneira:

```
./vcube <num_processos> <num_eventos> <num_falhas> <num_recuperações>
```

### stack.c

Criamos uma biblioteca de pilha, a biblioteca a princípio é para para nos auxiliar ao agendar recuperações de processos, para poder apenas agendar recuperações de processos que tenham realmente falhado e garantir que não seja agendado eventos de recuperaçõs em processos que já estão corretos ou já se recuperaram. A biblioteca contém um cabeçalho **stack.h** para poder ser utilizado no programa principal.

Arquivo:

```
stack.c.txt(link aqui)
stack.h.txt(link aqui)
```

### cisj.c

Arquivo que calcula a função **c(i,s)** do vCube. O arquivo em si é idêntico ao que nos foi disponibilizado, porém retiramos a função **main()**  e incluímos um arquivo de cabeçalho **cisj.h** para poder ser utilizado no programa principal.

Arquivo:

```
cisj.c.txt(link aqui)
cisj.h.txt(link aqui)
```

### simpl.c

Arquivo de biblioteca de simulação que nos foi disponibilizado. O arquivo em si não teve alterações do original, e vem acompanhado de um cabeçalho **simpl.h**.

Arquivo:

```
simpl.c.txt(link aqui)
simpl.h.txt(link aqui)
```

### rand.c

Biblioteca de geração de valores aleatórios que nos foi disponibilizado, a princípio não utilizamos, mas mesmo assim está incluso na compilação do makefile.

Arquivo:

```rand.c.txt(link aqui)
```

### makefile

Arquivo para compilação do nosso programa principal. Gera o executável **vcube**.

Arquivo:

```makefile.txt(link aqui)
```

## Logs de execução

* log para 4 processos, 0 falhas e 0 recuperações: saida-4-0-0.txt(link aqui)
* log para 4 processos, 2 falhas e 1 recuperação: saida-4-2-1.txt(link aqui)
* log para 4 processos, 3 falhas e 3 recuperações: saida-4-3-3.txt(link aqui)
* log para 6 processos, 0 falhas e 0 recuperações: saida-6-0-0.txt(link aqui)
* log para 6 processos, 1 falha e 0 recuperações: saida-6-1-0.txt(link aqui)
* log para 6 processos, 2 falhas e 2 recuperações: saida-6-2-2.txt(link aqui)
* log para 8 processos, 0 falhas e 0 recuperações: saida-8-0-0.txt(link aqui)
* log para 8 processos, 1 falha e 0 recuperações: saida-8-1-0.txt(link aqui)
* log para 8 processos, 2 falhas e 1 recuperação: saida-8-2-1.txt(link aqui)


Período original em que o projeto foi feito: 1° Sem. 2023(Ajuste de Calendário Acadêmico do 2° Sem. 2022)
