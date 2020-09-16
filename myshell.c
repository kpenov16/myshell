#include<stdio.h>

int main(int argc, char * args[]){
    char  *  names [] = {"Bob", "John"};
    size_t size = sizeof(names)/sizeof(char *);
    printf("char * names []  is of size: %zu\n", size);

}
