#include "myshell_helpers.h"

#include <stdio.h>

#include <string.h>
#include <stdlib.h>


//this is a general function for tokonization and has no side effects on the input, 
//returns pointer to the tokenized string f.eks.: "This is tokenized" -> {"This", "is", "tokenized"} 
                                            // or "ls -la | grep shell" -> {"ls -la ", "grep shell"}
char ** get_commands_arr(size_t count_words, char * input_str, char delim[]){
    char * input_str_tmp = (char *) malloc(count_words); //we don't want to set NULL in the original string when we tokenize
    strcpy(input_str_tmp, input_str);                    //so we make a temp copy of it that we free from heap after that 
    
    //there is '\n' in the end that we are going to override with NULL
    char ** str_arr = malloc((count_words+1) * sizeof(char *));//ls -la NULL //there is + 1 for the additional NULL element in the end
    int i = 0;
    char *ptr = strtok(input_str_tmp, delim);  //start tokonization for the given delimiter " " or "|" in our case
    for ( ; ptr != NULL; i++){
        str_arr[i] = (char *) malloc(strlen(ptr) + 1); //alocate space for the string on heap
        strcpy(str_arr[i], ptr);                       //copy string to the alocated space
        ptr = strtok(NULL, delim);
    }
    //str_arr[i] = malloc(sizeof NULL); //**I keep this here to remind me of something :)
    //str_arr[i] = NULL;                //there is '\n' in the end that we are going to override with NULL
    free(input_str_tmp);
    return str_arr; 
    //remember to free the str_arr's elementer out of this function 
    //- maybe something like free(str_arr[0]); free(str_arr[1]); if ar length is 2, or free(str_arr) 
}


//we dont know how big is the input and therefore we need a function that can take 
//variable lenght input 
char * get_input_string(FILE* fp, size_t size){ // https://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(char)*size);//size is start size for the newly created block of memory
    if(!str)return str; //in case size is 0, NULL will be dealocated something like free(NULL) strange :)
    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){ //we read char by char and exit on new line or EOF
        str[len++]=ch; //add current char to char array and count up
        if(len==size){ //when current available space is reached - we need more space 
            str = realloc(str, sizeof(char)*(size+=16)); //we extend the str size with 16 bytes every time we need more space
            if(!str)return str; 
        }
    }
    str[len++]='\0'; //add end of string char at the end of the input and increase with 1
    return realloc(str, sizeof(char)*len); //add an extra byte from len at the end
}

//counts the number of strings separated by the delimiter whitout modifying the input str 
size_t get_words_count(size_t lenght_str,char * input_str, char delim[]){
    char * input_str_tmp = (char *) malloc(lenght_str + 1); 
    strcpy(input_str_tmp, input_str);
    size_t count_words = 0;
    char *ptr1 = strtok(input_str_tmp, delim);
    for( ; ptr1 != NULL; count_words++){
        ptr1 = strtok(NULL, delim);
    }
    free(input_str_tmp);
    return count_words;
}
