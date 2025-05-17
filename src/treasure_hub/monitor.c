#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "treasure_hub.h"

//the available commands for the monitor
const char monitor_commands[4][20]={"list_hunts","list_treasures","view_treasure","calculate_score"};
int received_sigusr1 = 0, received_sigusr2 = 0;
struct sigaction sa1 = {0}, sa2 ={0};
int pipe_m2cs[2]={0};
int pipe_m2tm[2]={0};


//the functions for handling SIGUSR1 and SIGUSR2
void handle_sigusr1() {
    received_sigusr1 = 1;
}

void handle_sigusr2() {
    received_sigusr2 = 1;
}
//the monitor writes the data from the treasure manager pipe to the treasure hub pipe
void write_result_from_pipe(){
    char buffer[BUFFER_SIZE*10]="";
    int size = read(pipe_m2tm[0],buffer,BUFFER_SIZE*10);
    if(size > 0){
        write(STDOUT_FILENO,buffer,size);
    }
}
//the monitor writes the data from the calculate score pipe to the treasure hub pipe
void write_result_from_cs_pipe(){
    char buffer[BUFFER_SIZE*5]="";
    int size = read(pipe_m2cs[0],buffer,BUFFER_SIZE*5-1);
    if(size > 0){
        write(STDOUT_FILENO,buffer,size);
    }
}

void list_hunts(){
    int pid=-1;
    if((pid=fork())<0){
        perror("fork error");
        exit(1);
    }
    if(pid==0){
        close(pipe_m2tm[0]);
        dup2(pipe_m2tm[1],STDOUT_FILENO);
        close(pipe_m2tm[1]);
        execlp(TREASURE_MANAGER_BIN,TREASURE_MANAGER_BIN,"all_hunts",NULL);
        perror("execlp");
        exit(2);
    }
    int status=0;
    if(waitpid(pid,&status,0)<0){
        perror("waitpid error");
    }
    if(!(WIFEXITED(status))){
        write(STDOUT_FILENO,"The process hasn't treminated normally.\n",41);
    }
    write_result_from_pipe();
    write(STDOUT_FILENO,"[monitor] The list_hunts command has been executed\n>",53);
}

void list_treasures(const char* hunt_id){
    int pid=-1;
    if((pid=fork())<0){
        perror("fork error");
        exit(1);
    }
    if(pid==0){
        close(pipe_m2tm[0]);
        dup2(pipe_m2tm[1],STDOUT_FILENO);
        execlp(TREASURE_MANAGER_BIN,TREASURE_MANAGER_BIN,"list",hunt_id,NULL);
        perror("execlp");
        exit(2);
    }
    int status=0;
    if(waitpid(pid,&status,0)<0){
        perror("waitpid error");
    }
    if(!(WIFEXITED(status))){
        write(STDOUT_FILENO,"The process hasn't treminated normally.\n",41);
    }
    write_result_from_pipe();
    write(STDOUT_FILENO,"[monitor] The list_treasures command has been executed\n>",57);
}

void view_treasure(const char* hunt_id, const char* treasure_id){
    int pid=-1;
    if((pid=fork())<0){
        perror("fork error");
        exit(1);
    }
    if(pid==0){
        close(pipe_m2tm[0]);
        dup2(pipe_m2tm[1],STDOUT_FILENO);
        execlp(TREASURE_MANAGER_BIN,TREASURE_MANAGER_BIN,"view",hunt_id,treasure_id,NULL);
        perror("execlp");
        exit(2);
    }
    int status=0;
    if(waitpid(pid,&status,0)<0){
        perror("waitpid error");
    }
    if(!(WIFEXITED(status))){
        write(STDOUT_FILENO,"The process hasn't treminated normally.\n",41);
    }
    write_result_from_pipe();
    write(STDOUT_FILENO,"\n[monitor] The view_treasure command has been executed\n>",57);
}

void calculate_score(const char* hunt_id){
    int pid=-1;
    if((pid=fork())<0){
        perror("fork error");
        exit(1);
    }
    if(pid==0){
        close(pipe_m2cs[0]);
        dup2(pipe_m2cs[1],STDOUT_FILENO);
        execlp(CALCULATE_SCORE_SCRIPT,CALCULATE_SCORE_SCRIPT,hunt_id,NULL);
        perror("execlp");
        exit(2);
    }
    int status=0;
    if(waitpid(pid,&status,0)<0){
        perror("waitpid error");
    }
    write_result_from_cs_pipe();
    write(STDOUT_FILENO,"\n[monitor] The calculate_score command has been executed\n>",59);
}
//the function choose a valid command to start the treasure manager process
void choose_command( char* buffer) {
    if (strncmp(buffer, monitor_commands[0],strlen(monitor_commands[0])) == 0) {
        list_hunts();
        return;
    }
    char command[20]="",hunt_id[10]="",treasure_id[10]="";
    int r = sscanf(buffer,"%s %s %s",command,hunt_id,treasure_id);
    if(strcmp(command,monitor_commands[1])==0){
        if(r==2){
            list_treasures(hunt_id);            
        }
        else{
            write(STDOUT_FILENO,"[monitor] The list_treasures command isn't used properly\n>",59);
        }
        return;
    }
    if(strcmp(command,monitor_commands[2])==0){
        if(r==3){
            view_treasure(hunt_id,treasure_id);            
        }
        else{
            write(STDOUT_FILENO,"[monitor] The view_treasure command isn't used properly\n>",58);
        }
        return;
    }
    if(strcmp(command,monitor_commands[3])==0){
        if(r==2){
            calculate_score(hunt_id);            
        }
        else{
            write(STDOUT_FILENO,"[monitor] The calculate_score command isn't used properly\n>",60);
        }
        return;
    }
    write(STDOUT_FILENO,"[monitor] The command is unknown\n",34);
}

void read_file(){
    int file = open(COMMAND_FILE,O_RDONLY,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH);
    if(file<0){
        perror("Failed to open command file");
        return;
    }
    char buffer[BUFFER_SIZE];
    int size=read(file,buffer,99);
    if(size<1){
        perror("error at reading from command file");
        return;
    }
    choose_command(buffer);
}
//the monitor loop that waits for a signal 

void monitor_loop(){
    while (1) {
        pause(); // wait for a signal

        if (received_sigusr2) {
            close(pipe_m2tm[1]);
            close(pipe_m2cs[1]);
            close(pipe_m2tm[0]);
            close(pipe_m2cs[0]);
            write(STDOUT_FILENO,"[monitor] Stop signal received. Exiting...\n",44);
            usleep(100000); // Sleep before exiting
            exit(2);
        }

        if (received_sigusr1) {
            received_sigusr1 = 0;
            read_file();
        }
    }
}
//initiating signals and creating pipes
void init_signals(){
    sa1.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa1, NULL);

    sa2.sa_handler = handle_sigusr2;
    sigaction(SIGUSR2, &sa2, NULL);

    if(pipe(pipe_m2tm)<0){
        perror("Eroare la crearea pipe-ului");
    }
    if(pipe(pipe_m2cs)<0){
        perror("Eroare la crearea pipe-ului");
    }

}

int main() {
    write(STDOUT_FILENO,"[monitor] Ready and waiting for commands...\n>",46);

    init_signals();

    monitor_loop();

    return 0;
}
