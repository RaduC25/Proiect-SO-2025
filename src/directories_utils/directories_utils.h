#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/fcntl.h>
#include <unistd.h>

#ifndef DIRECTORIES_UTILS_H
#define DIRECTORIES_UTILS_H

#define LOGS_PATH "logs"
#define HUNTS_PATH "treasure hunts"
#define TREASURE_FILE_NAME "/treasure.txt"
#define LOGS_FILE_NAME "/logs.txt"
#define HUNT_DIR_PATH "treasure hunts/hunt:"
#define USERS_PATH "resources/users.txt"

DIR* open_current_dir();

void create_treasure_hunts_dir();

void create_logs_dir();

int make_hunt_dir(char* hunt_id);

#endif