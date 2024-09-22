#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

int global_time = 0;

int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	char *file_name = (char *)argv[1];

	// TODO: change to argv[3], argv[2] will be the output.csv
	int16_t quantum = (int16_t)atoi(argv[2]);
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
				atoi(input_file->lines[i][6]));
	}

	// TODO: Implement the logic of the scheduler
	while (true)
	{
		// update process state
		change_process_state(low_queue);
		change_process_state(high_queue);

		enqueue_for_first_time(process_list, input_file->len, high_queue, global_time);
		promote_process(low_queue, high_queue, global_time);

		global_time++;
	}

	input_file_destroy(input_file);
}