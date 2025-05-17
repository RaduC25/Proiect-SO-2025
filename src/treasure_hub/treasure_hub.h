#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

#ifndef TREASURE_HUB_H
#define TREASURE_HUB_H

#define BUFFER_SIZE 100
#define COMMAND_FILE "resources/command_file.txt"
#define CALCULATE_SCORE_SCRIPT "./bin/calculate_score.sh"
#define MONITOR_BIN "./bin/monitor"
#define TREASURE_HUB_BIN "./bin/treasure_hub"
#define TREASURE_MANAGER_BIN "./bin/treasure_manager"
#define COMMANDS_NUMBER 8


void write_from_monitor();

void start_monitor();

void stop_monitor();

void exit_program();

void read_command();

void do_command(int command_nr,char* buffer);

int check_command(char* buffer);

void send_command(int command_nr,char* buffer);

void help();

#endif
