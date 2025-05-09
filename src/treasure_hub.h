#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 100
#define COMMAND_FILE "command_file.txt"
#define COMMANDS_NUMBER 6
const char commands[COMMANDS_NUMBER][20]={"start_monitor","list_hunts","list_treasures","view_treasure","stop_monitor","exit"};

pid_t static monitor_pid = -1;
int static monitor_running=0;
int fd_pipe[2]={0}; // 0 - read end, 1 - write end