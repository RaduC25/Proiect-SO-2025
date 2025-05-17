#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../directories_utils/directories_utils.h"
#include "write_logs.h"

char static current_time[BUFFER__SIZE]="";
char static file_path[BUFFER__SIZE*2]="";

void write_log_file_path(char* hunt_id){
    sprintf(file_path,"%s%s%s", HUNT_DIR_PATH, hunt_id, LOGS_FILE_NAME);
}

void create_logged_hunt_symlink(char* hunt_id) {

    write_log_file_path(hunt_id);
    // Check if the file exists
    if (access(file_path, F_OK) != 0) {
        perror("Logs file doesn't exists");
        return;
    }
    char abs_logged_hunt_path[500];
    if (!realpath(file_path, abs_logged_hunt_path)) {
        perror("realpath");
        return;
    }

    // making the symlink
    char symlink_name[500];
    sprintf(symlink_name, "%s/logged_hunt-%s", LOGS_PATH, hunt_id);

    unlink(symlink_name);// if there was an old symlink, it will be deleted
    if (symlink(abs_logged_hunt_path, symlink_name) != 0) {
        perror("symlink");
    } else {
        printf("Created symlink: %s -> %s\n", symlink_name, abs_logged_hunt_path);
    }
}
//writes the view operation in the log file

void write_view_log(char* hunt_id,char*treasure_id){
    char buffer[BUFFER__SIZE]="";
    write_log_file_path(hunt_id);
    current_datetime();
    int size=sprintf(buffer,"treasure: %s, from hunt: %s was printed -- %s\n",treasure_id,hunt_id,current_time);
    int f=open(file_path,O_WRONLY, S_IWGRP);
    if(f<0){
        perror("failed to open logs.txt");
        return;
    }
    lseek(f,0,SEEK_END);
    if(write(f,buffer,size)!=size){
        perror("error in writing into a file");
    }
    close(f);
}

//writes the remove operation in the log file
void write_remove_log(char* hunt_id,char*treasure_id){
    char buffer[BUFFER__SIZE]="";
    write_log_file_path(hunt_id);
    current_datetime();
    int size=sprintf(buffer,"treasure: %s, from hunt: %s was deleted -- %s\n",treasure_id,hunt_id,current_time);
    int f=open(file_path,O_WRONLY, S_IWGRP);
    if(f<0){
        perror("failed to open logs.txt");
        return;
    }
    lseek(f,0,SEEK_END);
    if(write(f,buffer,size)!=size){
        perror("error in writing into a file");
    }
    close(f);
}
//writes the add opeartion in the log file
void write_add_log(char* hunt_id, unsigned treasure_id, char* username){
    char buffer[BUFFER__SIZE]="";
    write_log_file_path(hunt_id);
    current_datetime();
    int size=sprintf(buffer,"added hunt with id: %u and username: %s -- %s\n",treasure_id,username,current_time);
    int file=open(file_path,O_WRONLY,S_IWGRP);
    if(file<0){
        perror("failed to open logs.txt");
        return;
    }
    lseek(file,0,SEEK_END);
    if(write(file,buffer,size)!=size){
        perror("error in writing into a file");
        return;
    }
    close(file);
}

//prints the current time in the log file
void current_datetime() {
    time_t now;
    struct tm *timeinfo;

    time(&now);  // get current time
    timeinfo = localtime(&now);  

    // the format is Day-Month-Year - Hour-Minutes-Seconds
    strftime(current_time, sizeof(current_time), "Date: %d-%m-%Y - Time: %H:%M:%S", timeinfo);
}