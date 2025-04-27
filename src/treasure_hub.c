#include <stdio.h>
#include <stdlib.h>
#include "treasure_hub.h"


void start_monitor(){
    if(monitor_running){
        printf("monitor already running\n");
        return;
    }

    if (pipe(fd_pipe) == -1) {
        perror("[hub] pipe failed");
        exit(1);
    }

    if( ( monitor_pid=fork() ) < 0){
        perror("fork error");
        exit(1);
    }
    if(monitor_pid==0){
        close(fd_pipe[1]);
        dup2(fd_pipe[0],STDIN_FILENO);
        execlp("./m","./m",NULL);
        perror("execlp failed");
        exit(-1);
    }
    close(fd_pipe[0]);
    monitor_running=1;
    printf("[monitor] has started with PID:%d \n",monitor_pid);
}

void stop_monitor(){
    if(monitor_running==0){
        printf("[monitor] is not running\n");
        return;
    }
    kill(monitor_pid,SIGTERM);
    monitor_running = 0;
    printf("[monitor] has been stopped\n");
}
void exit_program(){
    if(monitor_running){
        printf("monitor is still running\n");
        return;
    }
    
}
int check_command(char* buffer){
    for(int i=0;i<COMMANDS_NUMBER;i++){
        if(strncmp(commands[i],buffer,strlen(commands[i]))==0){
            return i;
        }
    }
    return -1;
}

void send_command(int command_nr){
    if(monitor_running==0){
        printf("[monitor] is not running\n");
        return;
    }
    if(command_nr<1 || command_nr>3){
        printf("command can't be send\n");
        return;
    }
    int size=write(fd_pipe[1],commands[command_nr],strlen(commands[command_nr]));
    if(size!=strlen(commands[command_nr])){
        perror("error at writing");
        return;
    }
    kill(monitor_pid, SIGUSR1);
}

void do_command(int command_nr){
    switch(command_nr){
        case 0:
            start_monitor();
            break;
        case 1:
            send_command(1);
            break;
        case 2:
            send_command(2);
            break;
        case 3:
            send_command(3);
            break;
        case 4:
            stop_monitor();
            break;
        case 5:
            exit_program();
            break;
    }
}
int main(){
    printf("Write command:\n");
    char buffer[100]="";
    int command_number=0;
    int size = read(STDIN_FILENO,buffer,99);
    if(size<=0){
        printf("reading error\n");
    }
    else{
        buffer[size]='\0';
    }
    while((command_number =check_command(buffer))!=5 || monitor_running!=0){
        if(command_number==-1){
            printf("Unknown command: %s\n",buffer);    
        }
        else{
            do_command(command_number);
        }
        size=read(STDIN_FILENO,buffer,99);
        if(size<=0){
        printf("reading error\n");
        }
        else{
            buffer[size]='\0';
        }
    }
    if(command_number==5){
        printf("The program has been stopped\n");
    }
    return 0;
}