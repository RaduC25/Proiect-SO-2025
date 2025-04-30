#include <stdio.h>
#include <stdlib.h>
#include "treasure_hub.h"


void start_monitor(){
    if(monitor_running){
        write(STDOUT_FILENO,"monitor already running\n",25);
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
    char buffer[50]="";
    sprintf(buffer,"[monitor] has started with PID: %d\n",monitor_pid);
    write(STDOUT_FILENO,buffer,strlen(buffer));
}

void stop_monitor(){
    if(monitor_running==0){
        write(STDOUT_FILENO,"[monitor] is not running\n",26);
        return;
    }
    kill(monitor_pid,SIGTERM);
    monitor_running = 0;
    write(STDOUT_FILENO,"[monitor] has been stopped\n",28);
}
void exit_program(){
    if(monitor_running){
        write(STDOUT_FILENO,"monitor is still running\n",26);
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
        write(STDOUT_FILENO,"[monitor] is not running\n",26);
        return;
    }
    if(command_nr<1 || command_nr>3){
        write(STDOUT_FILENO,"command can't be send\n",23);
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
    write(STDOUT_FILENO,"The program has started\n\nThe aveilable commands are:\n\n- start_monitor\n\n- list_hunts: list the hunts and the total number of treasures in each\n\n- list_treasures: show the information about all treasures in a hunt\n\n- view_treasure: show the information about a treasure in hunt\n\n- stop_monitor\n\n- exit\n\nWrite command:\n",316);
    char buffer[100]="";
    int command_number=0;
    int size = read(STDIN_FILENO,buffer,99);
    if(size<=0){
        write(STDOUT_FILENO,"reading error\n",15);
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
        write(STDOUT_FILENO,"reading error\n",15);
        }
        else{
            buffer[size]='\0';
        }
    }
    if(command_number==5){
        write(STDOUT_FILENO,"The program has been stopped\n",30);
    }
    return 0;
}