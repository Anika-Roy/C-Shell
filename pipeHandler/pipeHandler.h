#ifndef PIPE_H_
#define PIPE_H_

#include "../headers.h"

int pipe_separated_commands_populator(char* token,struct PipeSeparatedCommands pipe_separated_commands[]);
void get_input_output_fds(struct PipeSeparatedCommands pipe_separated_commands[],int* input_fd,int* output_fd,int num_pipes);

#endif