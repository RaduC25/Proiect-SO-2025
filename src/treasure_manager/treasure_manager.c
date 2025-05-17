#include <stdio.h>
#include <stdlib.h>
#include "treasure.h"
#include "write_logs.h"


void create_treasure_path(char* hunt_id){
    if(hunt_id==NULL){
        strcpy(treasure_path,"");
        return;
    }
    sprintf(treasure_path,"%s%s%s",HUNT_DIR_PATH,hunt_id,TREASURE_FILE_NAME);
}

void create_logs_path(char* hunt_id){
    if(hunt_id==NULL){
        strcpy(logs_path,"");
        return;
    }
    strcpy(logs_path,"");
    sprintf(logs_path,"%s%s%s",HUNT_DIR_PATH,hunt_id,LOGS_FILE_NAME);
}


//search if the hunt exists
int search_hunt_id(char* hunt_id){
    DIR* d = opendir(HUNTS_PATH);
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


//returns the next treasure id in a specified hunt
unsigned get_next_treasure_id(char* hunt_id) {
    char file_path[100];
    create_logs_path(hunt_id);
    strcpy(file_path,logs_path);
    // strcpy(file_path,HUNT_DIR_PATH);
    // strcat(file_path,hunt_id);
    // strcat(file_path,LOGS_FILE_NAME);
    int fd = open(file_path, O_RDWR);
    if (fd < 0) {
        perror("open logs.txt failed");
        return 0;
    }
    char buffer[20];
    int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
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

unsigned get_total_hunts(char* hunt_id){
    char file_path[FILE_PATH_SIZE];
    create_logs_path(hunt_id);
    strcpy(file_path,logs_path);
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("open logs.txt failed");
        return 0;
    }
    char buffer[BUFFER_SIZE/10];
    int size = read(fd,buffer,BUFFER_SIZE/10 - 1);
    if (size <= 0) {
        perror("read logs.txt failed");
        close(fd);
        return 0;
    }
    buffer[size]='\0';
    char *p=strstr(buffer,"TOTAL TREASURES:");
    if(p==NULL){
        perror("strstr");
        close(fd);
        return 0;
    }
    unsigned current_hunts;
    sscanf(p,"TOTAL TREASURES:%u",&current_hunts);
    return current_hunts;
}

void modify_total_hunts(char* hunt_id, unsigned total){
    char file_path[FILE_PATH_SIZE];
    create_logs_path(hunt_id);
    strcpy(file_path,logs_path);
    int fd = open(file_path, O_RDWR);
    if (fd < 0) {
        perror("open logs.txt failed");
        return;
    }
    char buffer[BUFFER_SIZE/10];
    int size = read(fd,buffer,BUFFER_SIZE/10 - 1);
    if (size <= 0) {
        perror("read logs.txt failed");
        close(fd);
        return;
    }
    buffer[size]='\0';

    unsigned current_id = 0;
    sscanf(buffer, "ID COUNT:%u", &current_id);
    sprintf(buffer,"ID COUNT:%u\nTOTAL TREASURES:%u\n",current_id,total);
    lseek(fd,0,SEEK_SET);
    if(write(fd,buffer,strlen(buffer)-1)!=strlen(buffer)-1){
        perror("write logs.txt failed");
        close(fd);
        return;
    }
    close(fd);
}

int check_data(Treasure t){
    if(t.clue_text[strlen(t.clue_text)-2]=='\n'){
        t.clue_text[strlen(t.clue_text)-1] = '\0';
    }
    return (-180 <= t.longitude && t.longitude <=180) && (-90<= t.latitude && t.latitude <= 90);
}
// the function checks if the user name is in the users file, in the resources folder

int check_user_name(char* user_name){
   int file = open(USERS_PATH,O_RDONLY | O_CREAT,S_IRUSR | S_IRGRP);
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
//the function add a new username in the users file, in the resources folder

void add_username(char* user_name){
    if(user_name==NULL){
        printf("user name-ul este incorect\n");
        return;
    }
    int f=open(USERS_PATH,O_WRONLY | O_CREAT, S_IWGRP);
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



//writes data into the treasure file and into the log file
void write_data(Treasure t,char* hunt_id){
    char s[200];
    int size=sprintf(s,"ID:%u\nuser name:%s\nlatitude:%.2f--longitude:%.2f\nclue:%svalue:%d\n\n",t.treasureID,t.user_name,t.latitude,t.longitude,t.clue_text,t.value);
    char file1[40], file2[40];
    create_treasure_path(hunt_id);
    strcpy(file1,treasure_path);
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
    create_logs_path(hunt_id);
    strcpy(file2,logs_path);
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
//ads a new hunt, if the username is new
void add_hunt(char* hunt_id){
    printf("Please type the treasure data:\nUser name: ");
    Treasure t={0};
    int size=0;
    if(scanf("%30s",t.user_name)!=1){
        printf("Wrong data\n");
        return;
    }
    printf("\nLongitude: ");
    if(scanf("%f",&t.longitude)!=1){
        printf("Wrong data\n");
        return;
    }
    printf("\nLatitude: ");
    if(scanf("%f",&t.latitude)!=1){
        printf("Wrong data\n");
        return;
    }
    printf("\nValue: ");
    if(scanf("%d",&t.value)!=1){
        printf("Wrong data\n");
        return;
    }
    write(STDOUT_FILENO,"\nClue: ",8);
    getchar();
    if((size=read(STDOUT_FILENO,t.clue_text,CLUE_SIZE-1))<1){
        printf("Wrong data\n");
        return;
    }
    if(check_data(t)==0){
        return;
    }
    if(!search_hunt_id(hunt_id)){
        make_hunt_dir(hunt_id);
        create_logged_hunt_symlink(hunt_id);
        add_username(t.user_name);
    }
    if(!check_user_name(t.user_name)){
        add_username(t.user_name);
    }
    modify_total_hunts(hunt_id,get_total_hunts(hunt_id) + 1);
    t.treasureID=get_next_treasure_id(hunt_id);
    write_data(t,hunt_id);
    printf("\nThe treasure, with the ID: %d was succesfully added to hunt %s\n",t.treasureID,hunt_id);
}

//prints the entire hunt
void list_hunt(char* hunt_id){
    if(search_hunt_id(hunt_id)==0){
        printf("This hunt doesn't exist\n");
        return;
    }
    char file1_path[100],buffer[BUFFER_SIZE],file2_path[100];
    create_treasure_path(hunt_id);
    strcpy(file1_path,treasure_path);
    struct stat sbuf;
    stat(file1_path,&sbuf);
    printf("HUNT ID: %s\nTreasure file size: %lld\n",hunt_id,sbuf.st_size);
    create_logs_path(hunt_id);
    strcpy(file2_path,logs_path);
    int file2=open(file2_path,O_RDONLY);
    int size=0,start=0;
    while((size=read(file2,buffer,BUFFER_SIZE-1))!=0){
        if(size<2){
            break;
        }
        start = size-2;
        while(buffer[start]!='\n' && start>=0){
            start--;
        }
    }
    //if(start!=0){
        printf("Last modification: %s\n",buffer + start +1);
    //}
    close(file2);
    int file1=open(file1_path,O_RDONLY);
    if(file1<0){
        perror("failed to open treasure.txt");
        return;
    }
    while((size=read(file1,buffer,999))!=0){
        buffer[size]='\0';
        printf("%s\n",buffer);
    }
    close(file1);
}


//this function returns the file position where the treasure with the given id starts, if the treasure is not in the file, it return -1
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
    create_treasure_path(hunt_id);
    strcpy(file_path,treasure_path);
    int seek_pos = find_tresure_in_file(file_path,treasure_id);//returns the position where the treasure start in the file
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
        return;
    }
    char file_path[50];
    create_treasure_path(hunt_id);
    strcpy(file_path,treasure_path);
    if(unlink(file_path)!=0){
        printf("Error at removing a file 1 \n");
        return;
    }
    create_logs_path(hunt_id);
    strcpy(file_path,logs_path);
    if(unlink(file_path)!=0){
        printf("Error at removing a file 2 \n");
        return;
    }
    char dir_path[50];
    sprintf(dir_path,"%s%s",HUNT_DIR_PATH,hunt_id);
    if(rmdir(dir_path)!=0){
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
    char file_path[100],buffer[1000];
    create_treasure_path(hunt_id);
    strcpy(file_path,treasure_path);
    int seek_pos = find_tresure_in_file(file_path,treasure_id); //returns the position where the treasure start in the file
    if(seek_pos==-1){
        printf("This treasure doesn't exist in this hunt\n");
        return;
    }
    int f= open(file_path,O_RDWR,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP); //the file is open for read and write in the same time
    if(f<0){
        perror("failed to open treasure.txt");
        return;
    }
    lseek(f,seek_pos,SEEK_SET);
    int size = read(f,buffer,99);
    buffer[size]='\0';
    char *p=strstr(buffer,"\n\n");
    if(p==NULL){
        printf("error at deleting a treasure from the file\n");
        return;
    }
    int gap = p - buffer + 2; // this represent the number of characters that are deleted
    off_t read_pos = lseek(f, seek_pos + gap, SEEK_SET);
    off_t write_pos = seek_pos;

    while ((size = read(f, buffer, sizeof(buffer))) > 0) {
        lseek(f, write_pos, SEEK_SET);
        write(f, buffer, size);
        read_pos += size;
        write_pos += size;
        lseek(f, read_pos, SEEK_SET);
    }
    ftruncate(f, write_pos);
    write_remove_log(hunt_id,treasure_id);
    printf("The treasure with the id: %s, from hunt: %s has been deleted\n",treasure_id,hunt_id);
    close(f);
    modify_total_hunts(hunt_id,get_total_hunts(hunt_id) - 1);
}

void print_all_hunts(){
    DIR* d = opendir(HUNTS_PATH);
    if(d==NULL){
        perror("opendir error");
        return;
    }
    struct dirent* entry = readdir(d);
    printf("These are all the current hunts:\n");
    while(entry!=NULL){
        if(entry->d_type == DT_DIR && strncmp(entry->d_name,"hunt:",5)==0){
            get_total_hunts(entry->d_name + 5);
            printf("\n");
            printf("%s -- %u treasures\n",entry->d_name,get_total_hunts(entry->d_name + 5));
        }
        entry = readdir(d);
    }
    
}

//this function checks if the arguments are correct
int check_argument(char* s,int arg_count){
    if(s==NULL){
        return 0;
    }
    int i;
    for(i=0;i<ARGS_NUMBER;i++){
        if(strcmp(arguments_cmd[i],s)==0){
            break;
        }
    }
    if(i==ARGS_NUMBER){
        return 0;
    }
    if((strcmp(s,"remove_treasure")==0 || strcmp(s,"view")==0)){
        if(arg_count!=4){
            return 0;
        }
        return 3;
    } 
    if(strcmp(s,"help")==0 || strcmp(s,"all_hunts")==0){
        if(arg_count!=2){
            return 0;
        }
        return 1;
    }
    if(arg_count!=3){
        return 0;
    }
    return 2;
}

void help(){
    printf("\nThe following arguments could be used:\n");
    printf("\nadd <hunt_id>: Add a new treasure to the specified hunt. If the hunt doesn't exist, it will be created. The treasure data are introduced fromthe standard input.\n");
    printf("\nlist <hunt_id>: List all treasures in the specified hunt.\n");
    printf("\nview <hunt_id> <id>: View details of a specific treasure.\n");
    printf("\nremove_treasure <hunt_id> <id>: Remove a treasure  from a hunt, if the hunt and the treasure exist.\n");
    printf("\nremove_hunt <hunt_id>: Remove an entire hunt, if the hunt exists\n\n");
}

void appeal_function(char** argv,int argc){
    if(strcmp(argv[1],"add")==0  && argc==3){
        add_hunt(argv[2]);
        return;
    }
    if(strcmp(argv[1],"list")==0 && argc==3){
        list_hunt(argv[2]);
        return;
    }
    if(strcmp(argv[1],"remove_hunt")==0 && argc==3){
        remove_hunt(argv[2]);
        return;
    }
    if(strcmp(argv[1],"remove_treasure")==0 && argc==4){
        remove_treasure(argv[2],argv[3]);
        return;
    }
    if(strcmp(argv[1],"view")==0 && argc==4){
        view_treasure(argv[2],argv[3]);
        return;
    }
    if(strcmp(argv[1],"help")==0 && argc==2){
        help();
    }
    if(strcmp(argv[1],"all_hunts")==0 && argc==2){
        print_all_hunts();
    }
}

int main(int argc, char** argv){
    if(argc<2 || argc>4){
        printf("Incorrect usage\nWrong number of arguments\nUse help for more informartions\n");
        return 1;
    }
    int value = check_argument(argv[1],argc);
    if(value==0){
        printf("Wrong arguments\nUse help for more informartions\n");
        return 0;
    }
    appeal_function(argv,argc);
    return 0;
}