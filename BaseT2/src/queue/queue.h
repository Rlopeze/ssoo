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

bool is_empty(Queue *queue);