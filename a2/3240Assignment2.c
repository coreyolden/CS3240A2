#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


//node struct to be used in the queue
typedef struct _node_t
{
        char *direct;
        struct _node_t *next;
        struct _node_t *previous;
} node_t;

//global variables for the queue
node_t *head = NULL;
node_t *tail = NULL;
node_t *current;
int queuesize = 0;

// int isempty(){          //used to tell if the queue is empty
//     if(head ==NULL){
//         return 0;
//     }
//     return 1;
// }

void enqueue(char *directory){ //sets the directory name at the end of the queue
    node_t *current=malloc(sizeof(node_t));
    int length = strlen(directory)+1;
    current->direct = malloc(length);
    strncpy(current->direct, directory, length);
    
    if (queuesize == 0){
        tail = current;
        head = current;
    }else{
        tail->next = current;
        current->previous = tail;
        tail = current;
    }
    queuesize++;
    
}


char *dequeue(){    // returns the directory name stored in the head and delete the head node pushing head back a node in the queue
    
    current = head;
    char *toreturn=malloc(512); 
    strncpy(toreturn, head->direct, 512);
    
    
    if(queuesize>1){
        head = current->next;
        current->next = NULL;
        head->previous = NULL;
        current->previous=NULL;
    }else{
        head = NULL;
        tail = NULL;
        current->next = NULL;
        current->previous = NULL;
    }

    queuesize--;
    //free up all memory from the deleted node
      free(current->direct);
      free(current->next);
      free(current->previous);
      free(current);
     
    return toreturn;

}

int main(int argc, char **argv) {

    struct stat buf; 
    lstat(argv[1], &buf); //verify arg is actually a directory

    if(! S_ISDIR(buf.st_mode) ){ //if arg isnt a directory then close.
        printf("That is not a directory\n\n");
        return -1;
    }
chdir(argv[1]);//get the current working directory which we will add the relative file to
    char cwd[4096];// buffer for current working directory
    getcwd(cwd, sizeof(cwd));

    char *direc = malloc(512);

    
    
    DIR *directory = opendir(cwd);
    
    struct dirent *entry;
     enqueue(cwd); 

    while(queuesize >0){
        memset(direc, 0, 512); //reset direc
        printf("\n%s\n", head->direct); //print the directory name
        direc = dequeue();  //get the directory to open
       
        if (chdir(direc) == -1){ //this will give an error if the file could not be read and print reason 
            perror("Error: ");
    }else{

        
        if ((directory = opendir(direc)) == NULL) //if file cant open this will print the reason 
        {
            perror("Cannot open .");
           
        }else{

            while ((entry = readdir(directory)) != NULL) //read until there are no more file or folders in the directory
            {

                if (entry->d_type == DT_DIR)    //if type is a directory set it up in the format of a path and enqueue it
                {
                    memset(cwd, 0, 4096);
                    getcwd(cwd, sizeof(cwd));
                    printf("    Directory: %s\n", entry->d_name);
                    int result = strcmp(".", entry->d_name);
                    //don't want to try to go into "." or ".."
                    if(result == 0){continue;}
                     result = strcmp("..", entry->d_name);
                    if (result == 0)
                    {
                        continue;
                    }
                    strcat(cwd, "/");
                    strcat(cwd, entry->d_name);
                    strcat(cwd, "/");
                    enqueue(cwd);
                    memset(cwd, '\0', 512);
                    continue;
                };
                if (entry->d_type == DT_REG) // if type is folder just print name
                {
                    printf("    File: %s\n", entry->d_name);
                    continue;
                };
    }
    closedir(directory);
        }
    }
    printf("---------------------------\n"); // after each file just print a line
    }

    return 0;
}