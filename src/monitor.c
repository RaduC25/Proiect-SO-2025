#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_CMD_LEN 256

const char monitor_commands[3][20]={"list_hunts","list_treasures","view_treasure"};

sig_atomic_t received_sigusr1 = 0;
sig_atomic_t received_sigusr2 = 0;

void handle_sigusr1(int sig) {
    received_sigusr1 = 1;
}

void handle_sigusr2(int sig) {
    received_sigusr2 = 1;
}

void list_hunts();

void list_treasures(const char* hunt_id);

void view_treasure(const char* hunt_id, const char* treasure_id);

void choose_command(const char* cmd) {
    // if (strcmp(cmd, "list_hunts") == 0) {
    //     list_hunts();
    // } else if (strncmp(cmd, "list_treasures", 14) == 0) {
    //     const char* hunt_id = cmd + 15;
    //     list_treasures(hunt_id);
    // } else if (strncmp(cmd, "view_treasure", 13) == 0) {
    //     const char* rest = cmd + 14;
    //     char hunt_id[64], treasure_id[64];
    //     sscanf(rest, "%s %s", hunt_id, treasure_id);
    //     view_treasure(hunt_id, treasure_id);
    // } else {
    //     printf("[monitor] Unknown command received: %s\n", cmd);
    // }
}

int main() {
    struct sigaction sa1 = {0};
    sa1.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa1, NULL);

    struct sigaction sa2 = {0};
    sa2.sa_handler = handle_sigusr2;
    sigaction(SIGUSR2, &sa2, NULL);

    printf("[monitor] Ready and waiting for commands...\n");

    char stdin_buffer[MAX_CMD_LEN]="";

    while (1) {
        pause(); // Wait for a signal

        if (received_sigusr2) {
            printf("[monitor] Stop signal received. Exiting...\n");
            usleep(500000); // Sleep 0.5 sec before exiting
            break;
        }

        if (received_sigusr1) {
            received_sigusr1 = 0;
            int size = read(STDIN_FILENO, stdin_buffer, sizeof(stdin_buffer)-1);
            if(size>0){
                stdin_buffer[size]='\0';
                printf("%s\n",stdin_buffer);
                choose_command(stdin_buffer);
            }
        }
    }

    return 0;
}
