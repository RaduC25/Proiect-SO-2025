#include <stdio.h>
#include <stdlib.h>
#include "treasure_hub.h"
#include <sys/wait.h>

//the command list for the hub
const char commands[COMMANDS_NUMBER][20]={"start_monitor","list_hunts","list_treasures","view_treasure","calculate_score","stop_monitor","exit","help",};
pid_t static monitor_pid = -1;
int static monitor_running=0;
int pipe_th2m[2]={0};

//the function that writes from monitor to stdout
void write_from_monitor(){
    usleep(10000);
    char buffer[BUFFER_SIZE*10]="";
    int size = read(pipe_th2m[0],buffer,BUFFER_SIZE*10);
    if(size > 0){
        write(STDOUT_FILENO,buffer,size);
    }
}
// the function that starts monitor process
void start_monitor(){
    if(monitor_running){
        write(STDOUT_FILENO,"monitor already running\n",25);
        return;
    }
    if(pipe(pipe_th2m)<0){
        perror("Eroare la crearea pipe-ului");
    }

    if( ( monitor_pid=fork() ) < 0){
        perror("fork error");
        exit(1);
    }
    if(monitor_pid==0){
        close(pipe_th2m[0]);
        dup2(pipe_th2m[1],STDOUT_FILENO);
        close(pipe_th2m[1]);
        execlp(MONITOR_BIN,MONITOR_BIN,NULL);
        perror("execlp faileddd");
        exit(-1);
    }
    monitor_running=1;
    char buffer[50]="";
    sprintf(buffer,"[monitor] has started with PID: %d\n",monitor_pid);
    write(STDOUT_FILENO,buffer,strlen(buffer));
    write_from_monitor();

}
// the function checks if the command is valid
int check_command(char* buffer){
    for(int i=0;i<COMMANDS_NUMBER;i++){
        if(strncmp(commands[i],buffer,strlen(commands[i]))==0){
            return i;
        }
    }
    return -1;
}

//the function sends the command to monitor
void send_command(int command_nr,char* buffer){
    if(monitor_running==0){
        write(STDOUT_FILENO,"[monitor] is not running\n",26);
        return;
    }
    if(command_nr<1 || command_nr>4){
        write(STDOUT_FILENO,"command can't be send\n",23);
        return;
    }
    int file = open(COMMAND_FILE,O_CREAT |  O_WRONLY | O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH);
    if(file<0){
        perror("Failed to open command file ok ");
        return;
    }
    int size=write(file,buffer,strlen(buffer));
    if(size!=(int)
    strlen(buffer)){
        perror("error at writing in command file");
        return;
    }
    close(file);
    kill(monitor_pid, SIGUSR1);
    write_from_monitor();
}

//the function sends a signal that stops the process
void stop_monitor(){
    if(monitor_running==0){
        write(STDOUT_FILENO,"[monitor] is not running\n",26);
        return;
    }
    kill(monitor_pid,SIGUSR2);
    write_from_monitor();
    int status=0;
    wait(&status);
    if(!(WIFEXITED(status))){
        write(STDOUT_FILENO,"The process hasn't treminated normally.\n",41);
    }
    monitor_running = 0;
    usleep(20000);
    write(STDOUT_FILENO,"[monitor] has been stopped\n",28);
}

//the function checks if the monitor can be stopped
void exit_program(){
    if(monitor_running){
        write(STDOUT_FILENO,"[monitor] is still running\nTo exit, stop the monitor first\n>",61);
        return;
    }
    
}

void help(){
    write(STDOUT_FILENO,"The program has started\n\nThe available commands are:\n\n- start_monitor\n\n- list_hunts: list the hunts and the total number of treasures in each\n\n- list_treasures: show the information about all treasures in a hunt\n\n- view_treasure: show the information about a treasure in hunt\n\n- calculate_score: calculates all users score from a specified hunt\n\n- stop_monitor\n\n- exit\n\n- help\n\nWrite command:\n>",395);
}

//the function choose what command to be send to monitor, via command_file.txt, using SIGUSR1 signal
void do_command(int command_nr,char* buffer){
    if(command_nr < 0 && command_nr >= COMMANDS_NUMBER){
        return;
    }

    if(command_nr==0 || command_nr > 4 ){
        switch(command_nr){
        case 0:
            start_monitor();
            break;
        case 5:
            stop_monitor();
            break;
        case 6:
            exit_program();
            break;
        case 7: 
            help();
            break;
        }
    }
    else{
        send_command(command_nr,buffer);
    }
}
// the function read the commands from stdin and terminates the process only if the exit command is received and the monitor is stopped
void read_command(){
    char buffer[100]="";
    int command_number=0;
    int size = read(STDIN_FILENO,buffer,99);
    if(size<=0){
        write(STDOUT_FILENO,"reading error\n",15);
    }
    else{
        buffer[size]='\0';
    }
    while((command_number =check_command(buffer))!=6 || monitor_running!=0){
        if(command_number==-1){
            write(STDOUT_FILENO,"Unknown command: ",18);
            write(STDOUT_FILENO,buffer,strlen(buffer));
            write(STDOUT_FILENO,"\n>",3);
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
}
int main(){
    help();

    read_command();
    return 0;
}