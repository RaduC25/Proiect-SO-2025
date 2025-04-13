#ifndef TREASURE_H
#define TREASURE_H
#define USER_NAME_SIZE 30
#define CLUE_SIZE 100

static unsigned totalHunts;

typedef struct{
    unsigned  treasureID;
    char user_name[USER_NAME_SIZE];
    float longitude,latitude;
    char clue_text[CLUE_SIZE];
    int value;
}Treasure; 

#endif