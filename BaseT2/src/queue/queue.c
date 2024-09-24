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
  
  Node *prev = NULL;
  Node *selected_prev = NULL;
  Node *selected_node = NULL;
  Node *current = queue->head;
  int64_t highest_priority_value = INT64_MIN;

  while (current != NULL)
  {
    Process *process = current->process;

    if (process->state != WAITING)
    {
      int64_t priority_value = (current_time - process->last_cpu_tick) - process->deadline;

      if (priority_value > highest_priority_value ||
          (priority_value == highest_priority_value && process->pid < selected_node->process->pid))
      {
        selected_prev = prev;
        selected_node = current;
        highest_priority_value = priority_value;
      }
    }

    prev = current;
    current = current->next;
  }

  if (selected_node == NULL)
  {
    return NULL;
  }
  if (selected_prev == NULL)
  {
    queue->head = selected_node->next;
  }
  else
  {
    selected_prev->next = selected_node->next;
  }

  if (selected_node == queue->tail)
  {
    queue->tail = selected_prev;
  }

  queue->size--;
  Process *process = selected_node->process;
  free(selected_node);

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