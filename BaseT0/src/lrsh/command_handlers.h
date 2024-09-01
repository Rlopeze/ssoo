#ifndef COMMAND_HANDLERS_H
#define COMMAND_HANDLERS_H

typedef struct
{
  pid_t pid;
  char name[256];
  time_t start_time;
  int exit_code;
} Information;

void handle_hello_command();
void handle_sum_command(char *arg1, char *arg2);
void handle_is_prime_command(char *arg);
void handle_lrexec_command(char **arg, Information *process_list, int *process_count);
void handle_lrlist_command(Information *process_list, int process_count);

#endif
