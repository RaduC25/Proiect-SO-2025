#ifndef LOGS_H
#define LOGS_H

#define BUFFER__SIZE 100

void create_logged_hunt_symlink(char* hunt_id);

void write_view_log(char* hunt_id,char*treasure_id);

void write_remove_log(char* hunt_id,char*treasure_id);

void write_add_log(char* hunt_id, unsigned treasure_id, char* username);

void current_datetime();


#endif