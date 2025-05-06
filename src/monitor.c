#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_CMD_LEN 256

const char monitor_commands[3][20]={"list_hunts","list_treasures","view_treasure"};

int received_sigusr1 = 0, received_sigusr2 = 0;
struct sigaction sa1 = {0}, sa2 ={0};

void handle_sigusr1(int sig) {
    received_sigusr1 = 1;
}

void handle_sigusr2(int sig) {
    received_sigusr2 = 1;
}

void list_hunts(){
    int pid=-1;
    if((pid=fork())<0){
        perror("fork error");
        exit(1);
    }
    if(pid==0){
        execlp("./p1","./p1","all_hunts",NULL);
        perror("execlp");
        exit(2);
    }
    usleep(10000);
    write(STDOUT_FILENO,"[monitor] The list_hunts command has been executed\n",52);
}

void list_treasures(const char* hunt_id){
    int pid=-1;
    if((pid=fork())<0){
        perror("fork error");
        exit(1);
    }
    if(pid==0){
        execlp("./p1","./p1","list",hunt_id,NULL);
        perror("execlp");
        exit(2);
    }
    usleep(10000);
    write(STDOUT_FILENO,"[monitor] The list_treasures command has been executed\n",56);
}

void view_treasure(const char* hunt_id, const char* treasure_id){
    int pid=-1;
    if((pid=fork())<0){
        perror("fork error");
        exit(1);
    }
    if(pid==0){
        execlp("./p1","./p1","view",hunt_id,treasure_id,NULL);
        perror("execlp");
        exit(2);
    }
    usleep(10000);
    write(STDOUT_FILENO,"\n[monitor] The view_treasure command has been executed\n",56);
}

void choose_command( char* buffer) {
    printf("-%s\n",buffer);
    if (strncmp(buffer, monitor_commands[0],strlen(monitor_commands[0])) == 0) {
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

void monitor_loop(){
    char stdin_buffer[200]="";
    while (1) {
        pause(); // Wait for a signal

        if (received_sigusr2) {
            write(STDOUT_FILENO,"[monitor] Stop signal received. Exiting...\n",44);
            usleep(100000); // Sleep before exiting
            break;
        }

        if (received_sigusr1) {
            received_sigusr1 = 0;
            int size = read(STDIN_FILENO, stdin_buffer, sizeof(stdin_buffer)-1);
            if(size>0){
                stdin_buffer[size]='\0';
                choose_command(stdin_buffer);
            }
        }
    }
}

void init_signals(){
    sa1.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa1, NULL);

    sa2.sa_handler = handle_sigusr2;
    sigaction(SIGUSR2, &sa2, NULL);

}

int main() {
    write(STDOUT_FILENO,"[monitor] Ready and waiting for commands...\n",45);

    init_signals();

    monitor_loop();

    return 0;
}
