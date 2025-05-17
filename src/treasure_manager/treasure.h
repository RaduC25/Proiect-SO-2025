#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include "../directories_utils/directories_utils.h"


#ifndef TREASURE_H
#define TREASURE_H

#define USER_NAME_SIZE 30
#define CLUE_SIZE 100
#define BUFFER_SIZE 1000
#define ARGS_NUMBER 7
#define MIN_TREASURE_SIZE 62
#define FILE_PATH_SIZE 100


const char arguments_cmd[ARGS_NUMBER][20]={"add","list","remove_treasure","remove_hunt","view","help","all_hunts"};
char static treasure_path[100]="",logs_path[100]="";

typedef struct{
    unsigned  treasureID;
    char user_name[USER_NAME_SIZE+1];
    float longitude,latitude;
    char clue_text[CLUE_SIZE];
    int value;
}Treasure; 

#endif