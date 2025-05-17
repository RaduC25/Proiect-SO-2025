#include <stdio.h>
#include <stdlib.h>
#include "../directories_utils/directories_utils.h"
#include "write_logs.h"



void create_logged_hunt_symlink(char* hunt_id) {
    create_logs_dir();
    DIR* d = opendir(HUNTS_PATH);
    if(d==NULL){
        perror("opendir error");
        return;
    }
    struct dirent* entry = readdir(d);
    while(entry!=NULL){
        
        if (entry->d_type == DT_DIR && strncmp(entry->d_name, "hunt:", 5)==0 && strcmp(entry->d_name+5,hunt_id)==0) {
            char logged_hunt_path[50]="";
            sprintf(logged_hunt_path,"%s%s%s",HUNT_DIR_PATH,hunt_id,LOGS_FILE_NAME);
            struct stat st;
            if (stat(logged_hunt_path, &st) == 0) {
                char symlink_name[50];
                sprintf(symlink_name, "%s/logged_hunt-%s",LOGS_PATH,hunt_id);
                // Try to create symlink (ignore if it already exists)
                if (symlink(logged_hunt_path, symlink_name) != 0) {
                    printf("symlink error\n");
                } else {
                    printf("Created symlink: %s -> %s\n", symlink_name, logged_hunt_path);
                }
            }
        }
        entry = readdir(d);
    }
    closedir(d);
}
//writes the view operation in the log file

void write_view_log(char* hunt_id,char*treasure_id){
    char file_path[100]="",s[100]="";
    strcpy(file_path,HUNT_DIR_PATH);
    strcat(file_path,hunt_id);
    strcat(file_path,LOGS_FILE_NAME);
    int size=sprintf(s,"treasure: %s, from hunt: %s was printed\n",treasure_id,hunt_id);
    int f=open(file_path,O_WRONLY, S_IWGRP);
    if(f<0){
        perror("failed to open logs.txt");
        return;
    }
    lseek(f,0,SEEK_END);
    if(write(f,s,size)!=size){
        perror("error in writing into a file");
    }
    close(f);
}

//writes the remove operation in the log file
void write_remove_log(char* hunt_id,char*treasure_id){
    char file_path[100]="",s[100]="";
    strcpy(file_path,HUNT_DIR_PATH);
    strcat(file_path,hunt_id);
    strcat(file_path,LOGS_FILE_NAME);
    int size=sprintf(s,"treasure: %s, from hunt: %s was deleted\n",treasure_id,hunt_id);
    int f=open(file_path,O_WRONLY, S_IWGRP);
    if(f<0){
        perror("failed to open logs.txt");
        return;
    }
    lseek(f,0,SEEK_END);
    if(write(f,s,size)!=size){
        perror("error in writing into a file");
    }
    close(f);
}

