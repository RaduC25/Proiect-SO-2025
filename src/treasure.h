#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#ifndef TREASURE_H
#define TREASURE_H
#define USER_NAME_SIZE 30
#define CLUE_SIZE 100
#define BUFFER_SIZE 1000
#define ARGS_NUMBER 6
#define MIN_TREASURE_SIZE 62

#define HUNTS_PATH "treasure hunts"
#define HUNT_DIR_PATH "treasure hunts/hunt:"
#define TREASURE_FILE_NAME "/treasure.txt"
#define LOGS_FILE_NAME "/logs.txt"
#define LOGS_PATH "logs"


const char arguments[6][20]={"add","list","remove_treasure","remove_hunt","view","help"};
char static treasure_path[100]="",logs_path[100]="";

typedef struct{
    unsigned  treasureID;
    char user_name[USER_NAME_SIZE];
    float longitude,latitude;
    char clue_text[CLUE_SIZE];
    int value;
}Treasure; 

#endif