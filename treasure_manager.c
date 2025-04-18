#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>

#include "treasure.h"

#define BUFFER_SIZE 200
#define ARGS_NUMBER 6
#define MIN_TREASURE_SIZE 62
const char arguments[6][20]={"add","list","remove_treasure","remove_hunt","view","help"};
//...
int search_hunt_id(char* hunt_id){
    char cwd[PATH_MAX];
    if(getcwd(cwd,sizeof(cwd))==NULL){
        perror("getcwd error");
        return 0;
    }
    DIR* d = opendir(cwd);
    if(d==NULL){
        perror("opendir error");
        return 0;
    }
    struct dirent* entry = readdir(d);
    while(entry!=NULL){
        if(entry->d_type == DT_DIR && strncmp(entry->d_name,"hunt:",5)==0 && strcmp(entry->d_name+5,hunt_id)==0){
            closedir(d);
            return 1;
        }
        entry = readdir(d);
    }
    closedir(d);
    return 0;
}

void create_logged_hunt_symlink(char* hunt_id) {
    char cwd[PATH_MAX];
    if(getcwd(cwd,sizeof(cwd))==NULL){
        perror("getcwd error");
        return;
    }
    DIR* d = opendir(cwd);
    if(d==NULL){
        perror("opendir error");
        return;
    }
    struct dirent* entry = readdir(d);
    while(entry!=NULL){
        if (entry->d_type == DT_DIR && strncmp(entry->d_name, "hunt:", 5)==0 && strcmp(entry->d_name+5,hunt_id)==0) {
            char logged_hunt_path[50];
            sprintf(logged_hunt_path, "%s/logs.txt", entry->d_name);

            if (access(logged_hunt_path, F_OK) == 0) {
                char symlink_name[50];
                sprintf(symlink_name, "logged_hunt-%s", hunt_id);

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

unsigned get_next_treasure_id(char* hunt_id) {
    char file_path[100];
    sprintf(file_path, "hunt:%s/logs.txt", hunt_id);

    int fd = open(file_path, O_RDWR);
    if (fd < 0) {
        perror("open logs.txt failed");
        return 0;
    }
    char buffer[20];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        perror("read logs.txt failed");
        close(fd);
        return 0;
    }
    buffer[bytes_read] = '\0';

    unsigned current_id = 0;
    sscanf(buffer, "ID COUNT:%u", &current_id);

    //update the ID
    lseek(fd, 0, SEEK_SET);
    char updated[30];
    int len = sprintf(updated,"ID COUNT:%u\n", current_id + 1);
    if(write(fd, updated, len)!=len){
        perror("error in writing into a file");
    }

    close(fd);
    return current_id;
}


int check_data(Treasure t){
    if(t.clue_text[strlen(t.clue_text)-2]=='\n'){
        t.clue_text[strlen(t.clue_text)-1] = '\0';
    }
    return (-180 <= t.longitude && t.longitude <=180) && (-90<= t.latitude && t.latitude <= 90);
}

int check_user_name(char* user_name){
   int file = open("users.txt",O_RDONLY | O_CREAT,S_IRUSR | S_IRGRP);
   if(file<0){
        perror("failed to open user.txt");
        return 0;
    }
   int size1=0,size2=0;
   char buf[BUFFER_SIZE]="",s[BUFFER_SIZE + 20]="\0",*p;
   while((size1=read(file,buf,BUFFER_SIZE-1))!=0){
        buf[size1]='\0';
        strcat(s,buf);
        strcpy(buf,s+(size1+size2-20));
        p=strtok(buf," ");
        while(p!=NULL){
            if(strcmp(p,user_name)==0){
                close(file);
                return 0;
            }
            p=strtok(NULL," ");
        }
        strcpy(s,buf);
        size2=strlen(s);
   }
   close(file);
   return 1;
}

void add_username(char* user_name){
    if(user_name==NULL){
        printf("user name-ul este incorect\n");
        return;
    }
    int f=open("users.txt",O_WRONLY | O_CREAT, S_IWGRP);
    if(f<0){
        perror("failed to open treasure.txt");
        return;
    }
    lseek(f,0,SEEK_END);
    int size = strlen(user_name);
    user_name[size]=' ';
    if(write(f,user_name,size+1)!=size+1){
        perror("error in writing into a file");
        return;
    }
    close(f);
}

int make_hunt_dir(char* hunt_id){
    char s[20];
    sprintf(s,"hunt:%s",hunt_id);
    if(mkdir(s,0777)!=0){
        return 0;
    }
    char file1[40], file2[40];
    strcpy(file1,s);
    strcat(file1,"/treasure.txt");
    int f1=open(file1,O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if(f1<0){
        perror("failed to open treasure.txt");
        return 0;
    }
    write(f1,"\n",1);
    close(f1);
    strcpy(file2,s);
    strcat(file2,"/logs.txt");
    printf("%s\n",file2);
    int f2=open(file2,O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if(f2<0){
        perror("failed to open logs.txt");
        return 0;
    }
    write(f2,"ID COUNT:0\n",11);
    close(f2);
    return 1;
}

void write_data(Treasure t,char* hunt_id){
    char s[200];
    int size=sprintf(s,"ID:%u\nuser name:%s\nlatitude:%.2f--longitude:%.2f\nclue:%svalue:%d\n\n",t.treasureID,t.user_name,t.latitude,t.longitude,t.clue_text,t.value);
    char file1[40], file2[40];
    sprintf(file1, "hunt:%s/treasure.txt", hunt_id);
    int f1=open(file1,O_WRONLY, S_IWGRP);
    if(f1<0){
        perror("failed to open treasure.txt");
        return;
    }
    lseek(f1,0,SEEK_END);
    if(write(f1,s,size)!=size){
        perror("error in writing into a file");
        return;
    }
    close(f1);
    size=sprintf(s,"added hunt with id: %u and username: %s\n",t.treasureID,t.user_name);
    sprintf(file2, "hunt:%s/logs.txt", hunt_id);
    int f2=open(file2,O_WRONLY,S_IWGRP);
    if(f2<0){
        perror("failed to open logs.txt");
        return;
    }
    lseek(f2,0,SEEK_END);
    if(write(f2,s,size)!=size){
        perror("error in writing into a file");
        return;
    }
    close(f2);
}

void add_hunt(char* hunt_id){
    printf("Please type the treasure data:\n");
    Treasure t;
    if(scanf("%29s %f %f %d",t.user_name,&t.longitude,&t.latitude,&t.value)!=4){
        printf("Wrong data\n");
        return;
    }
    getchar();
    if(fgets(t.clue_text,99,stdin)==NULL){
        printf("Wrong data\n");
        return;
    }
    if(check_data(t)==0){
        printf("Wrong data\n");
        return;
    }
    if(search_hunt_id(hunt_id)){
        if(check_user_name(t.user_name)==0){
            printf("User name already exists\n");
            return;
        }
        else{
            add_username(t.user_name);

        }
    }
    else{
        make_hunt_dir(hunt_id);
        create_logged_hunt_symlink(hunt_id);
        add_username(t.user_name);
    }
    t.treasureID=get_next_treasure_id(hunt_id);
    write_data(t,hunt_id);
    printf("The treasure was succesfully added to hunt %s\n",hunt_id);
}

void list_hunt(char* hunt_id){
    if(search_hunt_id(hunt_id)==0){
        printf("This hunt doesn't exist\n");
        return;
    }
    char file1_path[100],buffer[1000];
    sprintf(file1_path, "hunt:%s/treasure.txt", hunt_id);
    struct stat sbuf;
    stat(file1_path,&sbuf);
    printf("HUNT ID: %s\nTreasure file size: %lld\n",hunt_id,sbuf.st_size);
    int f=open(file1_path,O_RDONLY);
    if(f<0){
        perror("failed to open treasure.txt");
        return;
    }
    int size=0;
    while((size=read(f,buffer,999))!=0){
        buffer[size]='\0';
        printf("%s\n",buffer);
    }
    close(f);
}

void write_view_log(char* hunt_id,char*treasure_id){
    char file_path[100],s[100];
    sprintf(file_path, "hunt:%s/logs.txt", hunt_id);
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

int find_tresure_in_file(char* file_path, char* treasure_id){
    char id[15]="ID:";
    strcat(id,treasure_id);
    int id_len=strlen(id);
    int f=open(file_path,O_RDONLY);
    if(f<0){
        perror("failed to open treasure.txt");
        return -1;
    }
    int size=0;
    char *p,buffer[200];
    int file_pos;
    size=read(f,buffer,15);
    buffer[size]='\0';
    while((p=strstr(buffer,"ID:"))!=NULL && size!=0){
        if(strncmp(p,id,id_len)==0){
            close(f);
            return file_pos + (p-buffer);
        }
        file_pos=lseek(f,MIN_TREASURE_SIZE-(strlen(p))-1,SEEK_CUR);
        size=read(f,buffer,50);
        buffer[size]='\0';
    }
    close(f);
    return -1;
}

void view_treasure(char* hunt_id,char*treasure_id){
    if(search_hunt_id(hunt_id)==0){
        printf("This hunt doesn't exist\n");
        return;
    }
    char file_path[100],buffer[200];
    sprintf(file_path, "hunt:%s/treasure.txt", hunt_id);
    int seek_pos = find_tresure_in_file(file_path,treasure_id);
    if(seek_pos==-1){
        printf("This treasure doesn't exist in this hunt\n");
        return;
    }
    int f= open(file_path,O_RDONLY);
    if(f<0){
        perror("failed to open treasure.txt");
        return;
    }
    lseek(f,seek_pos,SEEK_SET);
    int size = read(f,buffer,199);
    buffer[size]='\0';
    printf("Treasure:\n");
    char *p=strstr(buffer,"\n\n");
    if(p==NULL){
        printf("failed to find hunt\n");
        return;
    }
    buffer[p-buffer]='\0';
    printf("%s\n",buffer);
    close(f);
    write_view_log(hunt_id,treasure_id);
    
}

void remove_hunt(char* hunt_id){
    if(search_hunt_id(hunt_id)==0){
        printf("This hunt doesn't exists\n");
    }
    char s[50];
    sprintf(s,"hunt:%s/treasure.txt",hunt_id);
    if(unlink(s)!=0){
        printf("Error at removing a file\n");
        return;
    }
    sprintf(s,"hunt:%s/logs.txt",hunt_id);
    if(unlink(s)!=0){
        printf("Error at removing a file\n");
        return;
    }
    sprintf(s,"hunt:%s",hunt_id);
    if(rmdir(s)!=0){
        printf("error at removing directory\n");
        return;
    }
    printf("The hunt has been removed\n");
}

void remove_treasure(char* hunt_id,char*treasure_id){
    if(search_hunt_id(hunt_id)==0){
        printf("This hunt doesn't exist\n");
        return;
    }
    char file_path[100],buffer[200];
    sprintf(file_path, "hunt:%s/treasure.txt", hunt_id);
    int seek_pos = find_tresure_in_file(file_path,treasure_id);
    if(seek_pos==-1){
        printf("This treasure doesn't exist in this hunt\n");
        return;
    }
    int f= open(file_path,O_RDONLY);
    if(f<0){
        perror("failed to open treasure.txt");
        return;
    }
    lseek(f,seek_pos,0);

}
int check_argument(char* s,int arg_count){
    if(s==NULL){
        return 0;
    }
    int i;
    for(i=0;i<ARGS_NUMBER;i++){
        if(strcmp(arguments[i],s)==0){
            break;
        }
    }
    if(i==ARGS_NUMBER){
        return 0;
    }
    if((strcmp(s,"remove_treasure")==0 || strcmp(s,"view")==0)){
        if(arg_count!=4){
            printf("Incorrect usage of the arguments\n");
            return 0;
        }
        return 1;
    } 
    if(strcmp(s,"help")==0 && arg_count!=2){
        if(arg_count!=2){
            printf("Incorrect usage of the arguments\n");
            return 0;
        }
        return 1;
    }
    if(arg_count!=3){
        return 0;
    }
    return 1;
}


int main(int argc, char** argv){
    if(argc<2 || argc>4){
        printf("Incorrect usage\n");
        return 1;
    }
    if(check_argument(argv[1],argc)==0){
        printf("Wrong arguments\n");
        return 1;
    }
    if(strcmp(argv[1],"add")==0){
        add_hunt(argv[2]);
    }
    if(strcmp(argv[1],"list")==0){
        list_hunt(argv[2]);
    }
    if(strcmp(argv[1],"view")==0){
        view_treasure(argv[2],argv[3]);
    }
    if(strcmp(argv[1],"remove_hunt")==0){
        remove_hunt(argv[2]);
    }
    return 0;
}