#include <stdio.h>
#include <stdlib.h>
#include "directories_utils.h"

//returns the current directory
DIR* open_current_dir(){
    char cwd[256];
    if(getcwd(cwd,sizeof(cwd))==NULL){
        perror("getcwd error");
        return NULL;
    }
    DIR* d = opendir(cwd);
    if(d==NULL){
        perror("opendir error");
        return NULL;
    }
    return d;
}

void create_treasure_hunts_dir(){
    DIR* current_dir = open_current_dir();
    if(current_dir==NULL){
        return;
    }
    struct dirent* entry = readdir(current_dir);
    while(entry!=NULL){
        if(entry->d_type == DT_DIR && strcmp(entry->d_name,HUNTS_PATH)==0){
            closedir(current_dir);
            return;
        }
        entry = readdir(current_dir);
    }
    if(current_dir!=NULL){
        closedir(current_dir);
        return;
    }
    if(mkdir(HUNTS_PATH,0777)!=0){
        perror("mkdir");
        return;
    }
    closedir(current_dir);
}

void create_logs_dir(){
    DIR* current_dir = open_current_dir();
    if(current_dir==NULL){
        return;
    }
    struct dirent* entry = readdir(current_dir);
    while(entry!=NULL){
        if(entry->d_type == DT_DIR && strcmp(entry->d_name,LOGS_PATH)==0){
            closedir(current_dir);
            return;
        }
        entry = readdir(current_dir);
    }
    if(mkdir(LOGS_PATH,0777)!=0){
        perror("mkdir");
        return;
    }
}
//the function creates a hunt directory and the 2 files
int make_hunt_dir(char* hunt_id){
    char s[100];
    create_treasure_hunts_dir();
    sprintf(s,"%s%s",HUNT_DIR_PATH,hunt_id);
    if(mkdir(s,0777)!=0){
        return 0;
    }
    char file1[40], file2[40];
    strcpy(file1,s);
    strcat(file1,TREASURE_FILE_NAME);
    int f1=open(file1,O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH);
    if(f1<0){
        perror("failed to open treasure.txt");
        return 0;
    }
    write(f1,"\n",1);
    close(f1);
    strcpy(file2,s);
    strcat(file2,LOGS_FILE_NAME);
    int f2=open(file2,O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH);
    if(f2<0){
        perror("failed to open logs.txt");
        return 0;
    }
    write(f2,"ID COUNT:0\nTOTAL TREASURES:0\n",29);
    close(f2);
    return 1;
}