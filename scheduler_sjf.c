#include <stdio.h>

#include "queue.h" // contem funções uteis para filas
#include "proc.h"  // possui as funções dos processos
#include "stats.h" // possui as funções de estatisticas 
#include "utils.h" // possui funções uteis 

// Utilizando as variáveis globais definidas no 'main'
extern struct queue * ready;    // fila de aptos
extern struct queue * ready2;   // segunda fila de aptos
extern struct queue * blocked;  // fila de bloqueados
extern struct queue * finished; // fila de finalizados
// NOTE: essa fila de finalizados é utilizada apenas para
// as estatisticas finais

// variavel global que indica o tempo maximo que um processo pode executar ao todo
extern int MAX_TIME;

// função para ordenar a fila com base em remaining_time usando a ordenação por inserção
void sort_queue_by_remaining_time(struct queue *q)
{
    if (isempty(q) || q->head == NULL || q->head->next == NULL)
        return; // A fila está vazia ou tem apenas um elemento, não precisa ordenar

    struct proc *sorted = NULL; // Lista ordenada
    struct proc *current = q->head;

    while (current != NULL)
    {
        struct proc *next = current->next;
        
        // Inserir current em sorted de forma ordenada
        if (sorted == NULL || current->remaining_time <= sorted->remaining_time)
        {
            // Inserir no início da lista ordenada
            current->next = sorted;
            sorted = current;
        }
        else
        {
            // Encontrar a posição correta para inserir current
            struct proc *temp = sorted;
            while (temp->next != NULL && temp->next->remaining_time < current->remaining_time)
            {
                temp = temp->next;
            }
            
            // Inserir current após temp
            current->next = temp->next;
            temp->next = current;
        }
        
        current = next;
    }

    // Atualizar a cabeça e a cauda da fila
    q->head = sorted;
    
    // Atualizar o ponteiro da cauda
    if (sorted == NULL)
        q->tail = NULL;
    else
    {
        struct proc *last = sorted;
        while (last->next != NULL)
        {
            last = last->next;
        }
        q->tail = last;
    }
}

struct proc *scheduler(struct proc *current)
{
    struct proc *selected;

    // Tratamento do processo atual
    if (current != NULL)
    {
        switch (current->state)
        {
            case READY:
                enqueue(ready, current);
                count_ready_in(current);
                break;
            case BLOCKED:
                enqueue(blocked, current);
                count_blocked_in(current);
                break;
            case FINISHED:
                enqueue(finished, current);
                count_finished_in(current);
                break;
            default:
                printf("@@ ERRO no estado de saída do processo %d\n", current->pid);
        }
    }

    // Ordena a fila de aptos com base no tempo de processamento
    sort_queue_by_remaining_time(ready);

    // Se a fila de aptos está vazia, não há o que fazer
    if (isempty(ready))
    {
        return NULL;
    }

    // Pegando o primeiro processo da fila de aptos para executar
    selected = dequeue(ready);

    // Realizando as estatísticas para o processo que saiu da fila de aptos
    count_ready_out(selected);

    // Alterando o estado do processo selecionado para executando
    selected->state = RUNNING;

    return selected;
}