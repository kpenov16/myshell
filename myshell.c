#include<stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char * args[]){
    char * shared = "shared string of chars";
    int pid = fork();
    if(pid != 0){
        printf("#########====parent=====#########\n");
        printf("shared string in parent is: %s\n", shared);
        printf("parent is about to wait for the child\n");
        int r = waitpid(pid, NULL, 0);
        printf("waitpid returned: %d\n", r);
        printf("shared string in parent is still: %s\n", shared);
        printf("#########====parent=====#########\n");
        exit(0);
    }else{
        printf("#########====child=====#########\n");
        printf("shared string in child is: %s\n", shared);
        printf("Child is about to exit with 0\n");
        printf("#########====child=====#########\n");
        exit(0);
    }    
}

    /*for(size_t i = 0; i < argc; i++ ){
        printf("%s\n", args[i]);
    }*/

    /*char  *  names [] = {"Bob", "John"};
    size_t size = sizeof(names)/sizeof(char *);
    printf("char * names []  is of size: %zu\n", size);
    for(size_t i = 0; i < size; i++){
        printf("name %zu in names is %s\n", i, names[i]);        
    }*/
