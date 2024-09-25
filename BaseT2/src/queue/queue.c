#include <stdlib.h>
#include "queue.h"
#include <limits.h>
#include <stdio.h>  // FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc

Queue *create_queue(int quantum)
{
  Queue *q = (Queue *)malloc(sizeof(Queue));
  q->head = NULL;
  q->tail = NULL;
  q->quantum = quantum;
  q->size = 0;
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

Process *dequeue_specific_process(Queue *queue, Process *target_process)
{
  if (queue->head == NULL)
  {
    return NULL;
  }
  Node *current = queue->head;
  Node *previous = NULL;

  while (current != NULL)
  {
    if (current->process == target_process)
    {
      if (previous == NULL)
      {
        queue->head = current->next;
        if (queue->head == NULL)
        {
          queue->tail = NULL;
        }
      }
      else
      {
        previous->next = current->next;

        if (current->next == NULL)
        {
          queue->tail = previous;
        }
      }

      queue->size--;

      Process *process = current->process;

      free(current);

      return process;
    }
    previous = current;
    current = current->next;
  }

  return NULL;
}

Queue *enqueue_for_first_time(Process **process_list, int process_count, Queue *high_queue, int global_time)
{
  for (int i = 0; i < process_count; i++)
  {
    if (process_list[i]->initialTime == global_time)
    {
      process_list[i]->quantum = high_queue->quantum;
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
      high_queue->size++;
      // TODO: verify if this is correct
      process->quantum = high_queue->quantum;
      enqueue(high_queue, process);
      dequeue_specific_process(low_queue, process);

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
    if (process->state == READY)
    {
      process->waiting_time++;
    }
    
    if (process->state == WAITING)
    {
      process->ioWaitTimeLeft--;
      process->waiting_time++;
      if (process->ioWaitTimeLeft == 0)
      {
        process->state = READY;
        process->ioWaitTimeLeft = process->ioWaitTime;
      }
    }
    
    current = current->next;
  }
}

Process *select_process(Queue *queue, int global_time)
{
  Node *current = queue->head;
  Process *selected_process = NULL;
  int max_priority = INT_MIN;

  while (current != NULL)
  {
    Process *process = current->process;
    if (process->state == READY)
    {
      int priority_value = (global_time - process->last_cpu_tick) - process->deadline;

      if (priority_value > max_priority)
      {
        max_priority = priority_value;
        selected_process = process;
      }
      else if (priority_value == max_priority && process->pid < selected_process->pid)
      {
        selected_process = process;
      }
    }

    current = current->next;
  }

  return selected_process;
}

bool is_empty(Queue *queue)
{
  return queue->size == 0;
}
