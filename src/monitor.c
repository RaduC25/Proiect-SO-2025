#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_CMD_LEN 256

const char monitor_commands[3][20]={"list_hunts","list_treasures","view_treasure"};

int received_sigusr1 = 0, received_sigusr2 = 0;

void handle_sigusr1(int sig) {
    received_sigusr1 = 1;
}

void handle_sigusr2(int sig) {
    received_sigusr2 = 1;
}

void list_hunts();

void list_treasures(const char* hunt_id);

void view_treasure(const char* hunt_id, const char* treasure_id);

void choose_command( char* buffer) {
    if (strcmp(buffer, monitor_commands[0]) == 0) {
        list_hunts();
        return;
    }
    char command[20],hunt_id[10],treasure_id[10];
    int r = sscanf(buffer,"%s %s %s",command,hunt_id,treasure_id);
    if(r==2 && strcmp(command,monitor_commands[1])==0){
        list_treasures(hunt_id);            
        return;
    }
    if(r==3 && strcmp(command,monitor_commands[2])==0){
        view_treasure(hunt_id,treasure_id);
        return;
    }
    write(STDOUT_FILENO,"[monitor] The command is unknown\n",34);
}

int main() {
    struct sigaction sa1 = {0};
    sa1.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa1, NULL);

    struct sigaction sa2 = {0};
    sa2.sa_handler = handle_sigusr2;
    sigaction(SIGUSR2, &sa2, NULL);

    write("[monitor] Ready and waiting for commands...\n");

    char stdin_buffer[MAX_CMD_LEN]="";

    while (1) {
        pause(); // Wait for a signal

        if (received_sigusr2) {
            write("[monitor] Stop signal received. Exiting...\n");
            usleep(100000); // Sleep before exiting
            break;
        }

        if (received_sigusr1) {
            received_sigusr1 = 0;
            int size = read(STDIN_FILENO, stdin_buffer, sizeof(stdin_buffer)-1);
            if(size>0){
                stdin_buffer[size]='\0';
                write("%s\n",stdin_buffer);
                choose_command(stdin_buffer);
            }
        }
    }

    return 0;
}
