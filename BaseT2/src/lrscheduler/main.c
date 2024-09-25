#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

int global_time = 0;

Process* select_next_process(Queue *high_queue, Queue *low_queue) {
    if (!is_empty(high_queue)) {
		// Si hay procesos en la cola High, los ejecutamos primero
        return dequeue(high_queue); 
    } else if (!is_empty(low_queue)) {
		// Si no hay procesos en la cola High, ejecutamos los de la Low
        return dequeue(low_queue); 
    }
	// No hay procesos para ejecutar
    return NULL;
}

void write_output_file(char *filename, Process **process_list, int process_count) {
    FILE *file = fopen(filename, "w");

    // Escribir los encabezados del archivo CSV
    fprintf(file, "nombre_proceso,pid,interrupciones,turnaround,response,waiting,suma_deadline\n");

    // Recorrer cada proceso y escribir sus estadísticas
    for (int i = 0; i < process_count; i++) {
        Process *process = process_list[i];
        
        // Calcular estadísticas para cada proceso
        int interrupciones = process->interrupciones; // Ajusta según tu lógica
        int turnaround = process->last_cpu_tick - process->initialTime; // Turnaround time
        int response = process->response_time; // Tiempo de respuesta (el primer tick de CPU)
        int waiting = process->waiting_time; // Tiempo de espera
        int suma_deadline = process->suma_deadline; // Tiempo ejecutado después del deadline

        // Escribir las estadísticas del proceso
        fprintf(file, "%s,%d,%d,%d,%d,%d,%d\n",
                process->name, process->pid, interrupciones, turnaround, response, waiting, suma_deadline);
    }

    // Cerrar el archivo
    fclose(file);
}


void update_process_state(Process *running_process, Queue *low_queue, Queue *high_queue, int quantum, int global_time) {
    // Si el proceso ha consumido todo su quantum o ha terminado la ráfaga
    if (global_time - running_process->last_cpu_tick >= running_process->burstTime) {
        // Reducir el número de ráfagas restantes
        running_process->numBursts--; 
        if (running_process->numBursts == 0) {
            // Si no quedan ráfagas, el proceso ha terminado
            running_process->state = FINISHED; 
        } else {
            // El proceso aún tiene ráfagas, ver si debe moverse a WAITING o continuar ejecutando
            running_process->state = WAITING;
            enqueue(low_queue, running_process);  // Moverlo a la cola baja si está en WAITING
        }
        // Actualizar el último tick de CPU
        running_process->last_cpu_tick = global_time; 
    } else if (global_time - running_process->last_cpu_tick >= quantum) {
        // Si el proceso ha consumido todo su quantum, pasarlo a la cola de baja prioridad
        enqueue(low_queue, running_process);
        running_process->state = READY;

        // Reiniciar el quantum al de la cola low
        running_process->quantum = quantum;
    }
}



int main(int argc, char const *argv[])
{
    // Leer argumentos
    char *file_name = (char *)argv[1];
    char *output_file = (char *)argv[2]; // output.csv
    int16_t quantum = (int16_t)atoi(argv[3]);

    // Leer el archivo de entrada
    InputFile *input_file = read_file(file_name);

    // Crear las colas
    Queue *high_queue = create_queue(2 * quantum);
    Queue *low_queue = create_queue(quantum);

    // Crear lista de procesos
    Process **process_list = (Process **)malloc(input_file->len * sizeof(Process *));
    for (int i = 0; i < input_file->len; ++i) {
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

    Process *running_process = NULL; // Proceso en ejecución

    // Simular ticks del scheduler
    while (true) {
        // Actualizar procesos que hayan terminado IO_WAIT y pasarlos a READY
        change_process_state(low_queue);
        change_process_state(high_queue);

        // Si hay un proceso en ejecución, verificar su estado y actualizar
        if (running_process != NULL && running_process->state == RUNNING) {
            update_process_state(running_process, low_queue, high_queue, quantum, global_time);

            // Si el proceso consumió todo su quantum y sigue activo, reingresarlo a la cola
            if (running_process->state == READY) {
                // Se reingresa a la cola baja si terminó el quantum
                enqueue(low_queue, running_process); 
                // Ya no está ejecutándose
                running_process = NULL;  
            } else if (running_process->state == FINISHED) {
                 // El proceso ha finalizado
                running_process = NULL; 
            }
        }

        // Encolar los procesos que inician en este tick
        enqueue_for_first_time(process_list, input_file->len, high_queue, global_time);

        // Promover procesos de la cola baja a la alta si cumplen la condición
        promote_process(low_queue, high_queue, global_time);

        // Si no hay un proceso corriendo, seleccionar uno nuevo
        if (running_process == NULL || running_process->state == FINISHED) {
            running_process = select_next_process(high_queue, low_queue);
            if (running_process != NULL) {
                running_process->state = RUNNING;
                running_process->last_cpu_tick = global_time;
            }
        }

        // Terminar cuando todos los procesos hayan finalizado
        bool all_finished = true;
        for (int i = 0; i < input_file->len; i++) {
            if (process_list[i]->state != FINISHED) {
                all_finished = false;
                break;
            }
        }
        if (all_finished) break;
        // Incrementar el tiempo global
        global_time++; 
    }

    // Guardar las estadísticas en el archivo de salida
    write_output_file(output_file, process_list, input_file->len);

    // Liberar memoria
    input_file_destroy(input_file);
    free(process_list);
}

