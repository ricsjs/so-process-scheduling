#include <stdio.h>

#include "queue.h" // contem funções uteis para filas
#include "proc.h"  // possui as funções dos processos
#include "stats.h" // possui as funções de estatisticas 
#include "utils.h" // possui funções uteis 

// Utilizando as variáveis globais definidas no 'main'
extern struct queue *ready;    // fila de aptos de alta prioridade
extern struct queue *ready2;   // fila de aptos de baixa prioridade
extern struct queue *blocked;  // fila de bloqueados
extern struct queue *finished; // fila de finalizados
// NOTE: essa fila de finalizados é utilizada apenas para as estatisticas finais

// variavel global que indica o tempo maximo que um processo pode executar ao todo
extern int MAX_TIME;
struct proc *scheduler(struct proc *current)
{
    struct proc *selected = NULL;

    /*
     *   Tratando o processo que está atualmente executando
     */
    if (current != NULL)
    {
        printf("Lidando com o processo atual %d com estado%d\n", current->pid, current->state);
        
        // Verificando o estado em que o processo executando está
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

    /*
     *   Estratégia de seleção de um novo processo para executar
     */
    if (isempty(ready))
    {
        if (isempty(ready2))
        {
            return NULL;
        }
        else
        {
            selected = dequeue(ready2);
            count_ready2_out(selected);
        }
    }
    else
    {
        selected = dequeue(ready);
        count_ready_out(selected);
    }

    selected->state = RUNNING;
    count_running_in(selected);

    return selected;
}