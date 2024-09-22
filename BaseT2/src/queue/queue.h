#pragma once

#include "../process/process.h"
#include <stdbool.h>

typedef struct Node
{
  Process *process;
  struct Node *next;
} Node;

typedef struct
{
  Node *head;
  Node *tail;
  int quantum;
  int size;
} Queue;

Queue *create_queue(int quantum);

Queue *enqueue(Queue *queue, Process *process);

Process *dequeue(Queue *queue);

Queue *enqueue_for_first_time(Process **process_list, int process_count, Queue *high_queue, int global_time);

bool is_empty(Queue *queue);