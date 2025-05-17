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

void create_treasure_path(char* hunt_id);

void create_logs_path(char* hunt_id);

int search_hunt_id(char* hunt_id);

unsigned get_next_treasure_id(char* hunt_id);

unsigned get_total_hunts(char* hunt_id);

void modify_total_hunts(char* hunt_id, unsigned total);

int check_data(Treasure* t);

int check_user_name(char* user_name);

void add_username(char* user_name);

void write_data(Treasure t,char* hunt_id);

void add_hunt(char* hunt_id);

void list_hunt(char* hunt_id);

int find_tresure_in_file(char* file_path, char* treasure_id);

void view_treasure(char* hunt_id,char*treasure_id);

void remove_hunt(char* hunt_id);

void remove_treasure(char* hunt_id,char*treasure_id);

void print_all_hunts();

int check_argument(char* s,int arg_count);

void help();

void appeal_function(char** argv,int argc);

#endif