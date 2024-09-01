#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include "../input_manager/manager.h"
#include "command_handlers.h"

Information process_list[100];
int process_count = 0;

void handle_sigchld(int sig)
{
  int status;
  pid_t pid;
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
  {
    for (int i = 0; i < process_count; i++)
    {
      if (process_list[i].pid == pid)
      {
        if (WIFEXITED(status))
        {
          process_list[i].exit_code = WEXITSTATUS(status);
        }
        else
        {
          process_list[i].exit_code = -1;
        }
        break;
      }
    }
  }
}
int main()
{
  // source: https://cboard.cprogramming.com/linux-programming/100269-reaping-zombies-sigaction.html
  struct sigaction sa;
  sa.sa_handler = &handle_sigchld;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &sa, 0);

  while (1)
  {
    printf("shell> ");
    char **tokens = read_user_input();
    if (tokens[0] == NULL)
    {
      free_user_input(tokens);
      continue;
    }

    if (strcmp(tokens[0], "hello") == 0)
    {
      handle_hello_command();
    }
    else if (strcmp(tokens[0], "sum") == 0 && tokens[1] != NULL && tokens[2] != NULL)
    {
      handle_sum_command(tokens[1], tokens[2]);
    }
    else if (strcmp(tokens[0], "is_prime") == 0 && tokens[1] != NULL)
    {
      handle_is_prime_command(tokens[1]);
    }
    else if (strcmp(tokens[0], "lrexec") == 0 && tokens[1] != NULL)
    {
      handle_lrexec_command(&tokens[1], process_list, &process_count);
    }
    else if (strcmp(tokens[0], "lrlist") == 0)
    {
      handle_lrlist_command(process_list, process_count);
    }
    else if (strcmp(tokens[0], "exit") == 0)
    {
      free_user_input(tokens);
      break;
    }
    else
    {
      printf("Comando no reconocido o formato incorrecto.\n");
    }

    free_user_input(tokens);
  }

  return 0;
}
