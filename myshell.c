#include<stdio.h>

int main(int argc, char * args[]){
    for(size_t i = 0; i < argc; i++ ){
        printf("%s\n", args[i]);
    }
}

    /*char  *  names [] = {"Bob", "John"};
    size_t size = sizeof(names)/sizeof(char *);
    printf("char * names []  is of size: %zu\n", size);
    for(size_t i = 0; i < size; i++){
        printf("name %zu in names is %s\n", i, names[i]);        
    }*/
