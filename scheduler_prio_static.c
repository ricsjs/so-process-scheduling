#include <stdio.h>
#include <stdlib.h>  // Inclua esta linha para a função rand()
#include "queue.h" // contém funções úteis para filas
#include "proc.h"  // possui as funções dos processos
#include "stats.h" // possui as funções de estatísticas 
#include "utils.h" // possui funções úteis 

// Utilizando as variáveis globais definidas no 'main'
extern struct queue *ready;    // fila de aptos de alta prioridade
extern struct queue *ready2;   // fila de aptos de baixa prioridade
extern struct queue *blocked;  // fila de bloqueados
extern struct queue *finished; // fila de finalizados
// NOTE: essa fila de finalizados é utilizada apenas para as estatísticas finais

// variável global que indica o tempo máximo que um processo pode executar ao todo
extern int MAX_TIME;

void assign_queue(struct proc *process) {
    int threshold = 0.20 * MAX_TIME;  // 20% de MAX_TIME
    if (process->remaining_time <= threshold) {
        enqueue(ready, process);  // Adiciona à fila de alta prioridade
        count_ready_in(process);
    } else {
        enqueue(ready2, process);  // Adiciona à fila de baixa prioridade
        count_ready2_in(process);
    }
}

struct proc *scheduler(struct proc *current) {
    struct proc *selected = NULL;

    // Tratamento do processo atual
    if (current != NULL) {
        switch (current->state) {
            case READY:
                assign_queue(current);  // Aloca o processo na fila correta
                break;
            case BLOCKED:
                enqueue(blocked, current);
                count_blocked_in(current);
                break;
            case FINISHED:
                enqueue(finished, current);
                count_finished_in(current);
                return NULL;  // Processo finalizado não deve retornar para as filas de prontos
            default:
                printf("@@ ERRO no estado de saída do processo %d\n", current->pid);
        }
    }

    // Seleciona um novo processo para executar com base na probabilidade
    int random_val = rand() % 100;  // Gera um valor aleatório entre 0 e 99
    if (isempty(ready) && isempty(ready2)) {
        return NULL;  // Nenhum processo disponível para execução
    }

    if (isempty(ready2) || (random_val < 80 && !isempty(ready))) {
        // 80% de chance de pegar da fila `ready`
        if (!isempty(ready)) {
            selected = dequeue(ready);
            count_ready_out(selected);
        }
    } else {
        // 20% de chance de pegar da fila `ready2`
        if (!isempty(ready2)) {
            selected = dequeue(ready2);
            count_ready2_out(selected);
        }
    }

    if (selected != NULL) {
        // Alterando o estado do processo selecionado para executando
        selected->state = RUNNING;
        count_running_in(selected);
    }

    return selected;
}
