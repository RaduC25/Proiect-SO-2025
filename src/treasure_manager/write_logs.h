#ifndef LOGS_H
#define LOGS_H

void create_logged_hunt_symlink(char* hunt_id);

void write_view_log(char* hunt_id,char*treasure_id);

void write_remove_log(char* hunt_id,char*treasure_id);


#endif