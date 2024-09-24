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

Process *dequeue(Queue *queue, int global_time)
{
  if (queue->head == NULL)
  {
    return NULL;
  }

  Node *current = queue->head;
  Node *previous = NULL;
  Node *best_node = queue->head;
  Node *best_previous = NULL;

  while (current != NULL)
  {
    Process *current_process = current->process;
    Process *best_process = best_node->process;

    int current_priority = (global_time - current_process->last_cpu_tick) - current_process->deadline;
    int best_priority = (global_time - best_process->last_cpu_tick) - best_process->deadline;

    if ((current_priority > best_priority) ||
        (current_priority == best_priority && current_process->pid < best_process->pid))
    {
      best_node = current;
      best_previous = previous;
    }

    previous = current;
    current = current->next;
  }
  if (best_previous != NULL)
  {
    best_previous->next = best_node->next;
  }
  else
  {
    queue->head = best_node->next;
  }

  if (best_node->next == NULL)
  {
    queue->tail = best_previous;
  }
  queue->size--;
  Process *process = best_node->process;
  free(best_node);
  return process;
}

Queue *enqueue_for_first_time(Process **process_list, int process_count, Queue *high_queue, int global_time)
{
  for (int i = 0; i < process_count; i++)
  {
    if (process_list[i]->initialTime == global_time)
    {
      high_queue = enqueue(high_queue, process_list[i]);
    }
  }
  return high_queue;
}

void promote_process(Queue *low_queue, Queue *high_queue, int global_time)
{
  Node *prev = NULL;
  Node *current = low_queue->head;

  while (current != NULL)
  {
    Process *process = current->process;

    if (2 * process->deadline < global_time - process->last_cpu_tick)
    {
      if (prev == NULL)
      {
        low_queue->head = current->next;
      }
      else
      {
        prev->next = current->next;
      }
      if (current == low_queue->tail)
      {
        low_queue->tail = prev;
      }

      low_queue->size--;
      enqueue(high_queue, process);

      Node *temp = current;
      current = current->next;
    }
    else
    {
      prev = current;
      current = current->next;
    }
  }
}

void change_process_state(Queue *queue)
{
  Node *current = queue->head;
  while (current != NULL)
  {
    Process *process = current->process;
    if (process->state == WAITING)
    {
      process->ioWaitTimeLeft--;
      if (process->ioWaitTimeLeft == 0)
      {
        process->state = READY;
        process->ioWaitTimeLeft = process->ioWaitTime;
      }
    }
    current = current->next;
  }
}

bool is_empty(Queue *queue)
{
  return queue->size == 0;
}