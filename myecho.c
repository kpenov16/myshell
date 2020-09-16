#include<stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char * args[]){
    printf("I am myecho\n");
    for(size_t i = 0; i < argc; i++ ){
        if(i>0)
            printf("%s\n", args[i]);
    }
    exit(0);
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
