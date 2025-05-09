#include <stdio.h>
#include <stdlib.h>
#include "treasure_hub.h"
#include <sys/wait.h>


void start_monitor(){
    if(monitor_running){
        write(STDOUT_FILENO,"monitor already running\n",25);
        return;
    }

    if( ( monitor_pid=fork() ) < 0){
        perror("fork error");
        exit(1);
    }
    if(monitor_pid==0){
        execlp("./m","./m",NULL);
        perror("execlp failed");
        exit(-1);
    }
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
    int status=0;
    wait(&status);
    if(!(WIFEXITED(status))){
        write(STDOUT_FILENO,"The process hasn't treminated normally.\n",41);
    }
    monitor_running = 0;
    usleep(20000);
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

void send_command(int command_nr,char* buffer){
    if(monitor_running==0){
        write(STDOUT_FILENO,"[monitor] is not running\n",26);
        return;
    }
    if(command_nr<1 || command_nr>3){
        write(STDOUT_FILENO,"command can't be send\n",23);
        return;
    }
    int file = open(COMMAND_FILE,O_CREAT |  O_WRONLY,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH);
    if(file<0){
        perror("Failed to open command file ok ");
        return;
    }
    int size=write(file,buffer,strlen(buffer));
    if(size!=strlen(buffer)){
        perror("error at writing in command file");
        return;
    }
    close(file);
    kill(monitor_pid, SIGUSR1);
}

void help(){
    write(STDOUT_FILENO,"The program has started\n\nThe available commands are:\n\n- start_monitor\n\n- list_hunts: list the hunts and the total number of treasures in each\n\n- list_treasures: show the information about all treasures in a hunt\n\n- view_treasure: show the information about a treasure in hunt\n\n- stop_monitor\n\n- exit\n\nWrite command:\n",316);
}

void do_command(int command_nr,char* buffer){
    switch(command_nr){
        case 0:
            start_monitor();
            break;
        case 1:
            send_command(1,buffer);
            break;
        case 2:
            send_command(2,buffer);
            break;
        case 3:
            send_command(3,buffer);
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
    help();
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
            do_command(command_number,buffer);
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