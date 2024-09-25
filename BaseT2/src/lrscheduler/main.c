#include <stdio.h>  // FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

int global_time = 0;

void write_output_file(char *filename, Process **process_list, int process_count)
{
    FILE *file = fopen(filename, "w");

    fprintf(file, "nombre_proceso,pid,interrupciones,turnaround,response,waiting,suma_deadline\n");

    for (int i = 0; i < process_count; i++)
    {
        Process *process = process_list[i];

        int interrupciones = process->interrupciones;
        int turnaround = process->last_cpu_tick - process->initialTime;
        int response = process->response_time;
        int waiting = process->waiting_time;
        int suma_deadline = process->suma_deadline;

        fprintf(file, "%s,%d,%d,%d,%d,%d,%d\n",
                process->name, process->pid, interrupciones, turnaround, response, waiting, suma_deadline);
    }

    fclose(file);
}

Process *select_next_process(Queue *high_queue, Queue *low_queue, int global_time)
{
    if (!is_empty(high_queue))
    {
        return select_process(high_queue, global_time);
    }
    else if (!is_empty(low_queue))
    {
        return select_process(low_queue, global_time);
    }
    return NULL;
}

Process *update_process_state(Process *running_process, Queue *low_queue, Queue *high_queue, int quantum, int global_time)
{
    if (running_process->actualBurstTime > 0)
    {
        running_process->actualBurstTime--;
        running_process->quantum--;
        if (running_process->quantum < 0)
        {
            running_process->quantum = 0;
        }

        if (running_process->actualBurstTime == 0)
        {
            running_process->numBursts--;
            running_process->actualBurstTime = running_process->burstTime;
            running_process->state = WAITING;
            running_process->ioWaitTimeLeft = running_process->ioWaitTime;
            
        }

        if (running_process->numBursts == 0 && running_process->quantum == 0)
        {
            running_process->state = FINISHED;
            printf("Process %s finished at time %d\n", running_process->name, global_time);
            running_process->last_cpu_tick = global_time;
        }
        else if (running_process->quantum == 0)
        {
            Process *process = dequeue_specific_process(high_queue, running_process);
            if (process != NULL)
            {
                running_process->quantum = low_queue->quantum;
                enqueue(low_queue, running_process);
            }
        }
    }

    return running_process;
}

int main(int argc, char const *argv[])
{
    char *file_name = (char *)argv[1];
    char *output_file = (char *)argv[2];
    int16_t quantum = (int16_t)atoi(argv[3]);

    InputFile *input_file = read_file(file_name);

    Queue *high_queue = create_queue(2 * quantum);
    Queue *low_queue = create_queue(quantum);

    Process **process_list = (Process **)malloc(input_file->len * sizeof(Process *));
    for (int i = 0; i < input_file->len; ++i)
    {
        process_list[i] = process_create(
            input_file->lines[i][0],
            atoi(input_file->lines[i][1]),
            atoi(input_file->lines[i][2]),
            atoi(input_file->lines[i][3]),
            atoi(input_file->lines[i][4]),
            atoi(input_file->lines[i][5]),
            atoi(input_file->lines[i][6]),
            quantum);
    }

    Process *running_process = NULL;

    while (true)
    {
        change_process_state(low_queue);
        change_process_state(high_queue);

        if (running_process != NULL && running_process->state == RUNNING)
        {
            Process *process = update_process_state(running_process, low_queue, high_queue, quantum, global_time);
            if (process->state == WAITING || process->state == FINISHED)
            {
                if (process->state == FINISHED)
                {
                    dequeue_specific_process(high_queue, process);
                    dequeue_specific_process(low_queue, process);
                }
                running_process = select_next_process(high_queue, low_queue, global_time);
                if (running_process != NULL)
                {
                    running_process->state = RUNNING;
                    running_process->last_cpu_tick = global_time;
                }
            }
        }

        enqueue_for_first_time(process_list, input_file->len, high_queue, global_time);

        promote_process(low_queue, high_queue, global_time);

        // Si no hay un proceso corriendo, seleccionar uno nuevo
        if (running_process == NULL || running_process->state == FINISHED)
        {
            running_process = select_next_process(high_queue, low_queue, global_time);
            if (running_process != NULL)
            {
                running_process->state = RUNNING;
                running_process->last_cpu_tick = global_time;
            }
        }

        if (global_time <= 17)
        {
            printf("\nglobal_time: %d\n", global_time);
            printf("high_queue size: %d\n", high_queue->size);
            if (running_process != NULL)
            {
                printf("running_process name: %s\n", running_process->name);
                printf("running_process quantum: %d\n", running_process->quantum);
                printf("running_process->state: %d\n", running_process->state);
                printf("running_process->numbursts: %d\n", running_process->numBursts);
                printf("running_process->actualBurstTime: %d\n", running_process->actualBurstTime);
                printf("running_process->burstimes: %d\n", running_process->burstTime);
                printf("running_process->last_cpu_tick: %d\n", running_process->last_cpu_tick);
            }

            Node *current = high_queue->head;
            while (current != NULL)
            {
                Process *process = current->process;
                printf("high_queue process name: %s\n", process->name);
                printf("high_queue process state: %d\n", process->state);
                printf("high_queue process quantum: %d\n", process->quantum);
                printf("high_queue process numbursts: %d\n", process->numBursts);
                printf("high_queue process actualBurstTime: %d\n", process->actualBurstTime);

                current = current->next;
            }
            
            Node *current2 = low_queue->head;
            while (current2 != NULL)
            {
                Process *process = current2->process;
                printf("low_queue process name: %s\n", process->name);
                printf("low_queue process state: %d\n", process->state);
                printf("low_queue process quantum: %d\n", process->quantum);
                printf("low_queue process numbursts: %d\n", process->numBursts);
                printf("low_queue process actualBurstTime: %d\n", process->actualBurstTime);

                current2 = current2->next;
            }
        }
        if (global_time == 17)
            break;

        bool all_finished = true;
        for (int i = 0; i < input_file->len; i++)
        {
            if (process_list[i]->state != FINISHED)
            {
                all_finished = false;
                break;
            }
        }
        if (all_finished)
            break;
        global_time++;
    }

    write_output_file(output_file, process_list, input_file->len);

    input_file_destroy(input_file);
    free(process_list);
}