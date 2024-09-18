#include <stdlib.h>
#include "queue.h"

Queue *create_queue(int quantum)
{
  Queue *q = (Queue *)malloc(sizeof(Queue));
  q->head = NULL;
  q->tail = NULL;
  q->quantum = quantum;
  return q;
}

Queue *enqueue(Queue *queue, Process *process)
{
  Node *node = (Node *)malloc(sizeof(Node));
  node->process = process;
  node->next = NULL;
  if (queue->tail == NULL)
  {
    queue->head = node;
    queue->tail = node;
  }
  else
  {
    queue->tail->next = node;
    queue->tail = node;
  }
  queue->size++;
  return queue;
}

Process *dequeue(Queue *queue)
{
  if (queue->head == NULL)
  {
    return NULL;
  }
  Node *node = queue->head;
  queue->head = queue->head->next;
  if (queue->head == NULL)
  {
    queue->tail = NULL;
  }
  queue->size--;
  Process *process = node->process;
  free(node);
  return process;
}

bool is_empty(Queue *queue)
{
  return queue->size == 0;
}