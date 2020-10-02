#include "myshell_helpers.h"

#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <stdlib.h>

void run(){
    printf("myshell# ");
    
    char * input_str_tmp = get_input_string(stdin,10); //get pointer to user input 
    int    size   = strlen(input_str_tmp);       
    
    char input_str[size+1];
    strcpy(input_str, input_str_tmp);  //copy str from heap into local var on the stack            
    input_str[size] = '\0';     //add end of string char in any case ..
    free(input_str_tmp);        //free the heap alocated space from the original input
   

    size_t count_commands = get_words_count(size, input_str, "|");  //if pipe in the input we need the number of commands
    char ** commands_arr[count_commands];  //create array to hold pointers to each command, each command will be array of strings with null element in the end

    char ** commands_arr_tmp; // we are going to tokenize each command on a " " now but we don't need the originals - we call them tmp 
    commands_arr_tmp = get_commands_arr( count_commands, input_str, "|" );        
    for(size_t i = 0; i < count_commands; i++){ //for each command in the array we prepare 2d array - one row per command
        int size = strlen(commands_arr_tmp[i]); //how may chars a command has
        char delim[] = " ";
        
        size_t count_words = get_words_count(size, commands_arr_tmp[i], delim); //how many words are there in a command
        //tokenize the command strings and populate the 2d array so that can be used for the pipes
        int j = 0;
        char *ptr = strtok(commands_arr_tmp[i], delim);
        //size_t c_words = 0;
        char ** command_parmas_arr = malloc(count_words * sizeof(char *));;//ls -la NULL //we need a pointer for each element of each command
        for ( ; ptr != NULL; j++){
            command_parmas_arr[j] = (char *) malloc(strlen(ptr) + 1); //alocate space for the element
            strcpy(command_parmas_arr[j], ptr);    //copy the element in the next position of the array            
            ptr = strtok(NULL, delim);                
        }
        command_parmas_arr[j] = malloc(sizeof NULL);
        command_parmas_arr[j] = NULL; //if there is '\n' in the end that we are going to override with NULL
                                      //command_parmas_arr looks like this command_parmas_arr[0] = "ls", command_parmas_arr[1] = "-la" ..
                
        commands_arr[i] = command_parmas_arr;  //commands_arr  looks like this commands_arr[0] = {"ls", "-la", NULL} piped_commands_arr[1] = {"grep", "shell", NULL} ..
        free(commands_arr_tmp[i]); //free the heap of the temp array it is NULL tokenized and we cant use it
    }

    //  commands_arr ser sådan ud:
    //  commands_arr[0] = {"ls", "-la", NULL }; //hvor den string array ligger i heap men commands_arr pointers to arr liger i stack, måske :)
    //  commands_arr[1] = {"grep", "shel", NULL};
    for(size_t i=0; i < count_commands; i++){            
        for (char ** ptr = commands_arr[i]; *ptr != NULL; ptr++){
            //printf("%s\n", *ptr);                
        }
    }

    int fds[(count_commands-1)*2]; //generate array to hold file descriptors for each pipe in the command
    for( size_t i = 0; i < count_commands-1; i++ )
        pipe(fds + i*2); //generate pipes and connect them with file descriptors
                         //f.eks.: fds[0] and fds[1] are file decriptors for pipe 1, fds[2] and fds[3] for pipe 2 and so on 
    
    pid_t pid;

    for(size_t i=0, p=0; i < count_commands; i++, p+=2){            
        pid = fork(); //fork off child processes for each child process worker 
        if(pid == 0){ //if in child       0 stdin, 1 stdout, 2 stderr 
            if(i < count_commands-1){ //all other than last
                dup2(fds[p+1], 1);  //p1   p2   p3   p4        //stdout for the last pipe is not re-directed  
                                    //0{1} 2{3} 4{5} 6[7]      //dvs. it is let to come on the screen 
            }//                         ||   ||   ||   |       //but for all the rest the output is wrtten 
            //                          | |  | |  | |  |       //to the input end of the pipe,
            //                         |   | |  | |  ||        //the output end of the pipe is used by
            //                         |    |    |    |        //the next child process to reads its input
            if(i != 0){             //[0]1 {2}3 {4}5 {6}7
                dup2(fds[p-2], 0);  //p1   p2   p3   p4      //stdin for the first pipe is not directed 
                                    //all other than first    //but for all the rest the input is taken 
            }                                                //from the output end of the pipe between the 2 children, 
                                                             //the input end of the pipe is used by 
                                                             //the previous child process to write its output 

            //dup2(fds[p+1], 1); basically says: you my child write to what you think is the stdout 
            //                                   but I have changed it so is the start of the pipe you are writing to 

            //dup2(fds[p-2], 0); basically says: you my child read from what you think is the stdin but I have changed 
            //                                   it so is the end of the pipe you are reading from  
            //** exception is the first and the last child, 
            //   where the first will read from the actual stdin and the last will write to the actual stdout

            for(size_t i = 0; i < (count_commands-1)*2; i++ )
                close(fds[i]); //close all pipe ends, because we duplicated the file descriptors 
                               //after dup2 and before closing them we have two fd pointing to the same end of the pipe
                               //we don't want to write/read by chance in the code so we close them in that way only 
                               //the exec* can reach the pipes reading or writing to stdin or stdout thinking its the standard 
            execvp(commands_arr[i][0], commands_arr[i]);
        }else{
            //parent forks the next pipe
        }
    }
    //all pipes are channeld 
    for(size_t i = 0; i < (count_commands-1)*2; i++ )
        close(fds[i]); //close all pipe ends - this is the parent, all the children had their "copies" of the fds becouse of the parent 
                       //we have closed them in the children, remember, and now we do it in the parent
    int status;
    waitpid(pid, &status, 0); //wait for the last child to finish work

    for(size_t i=0; i < count_commands; i++){            
        for (char ** ptr = commands_arr[i]; *ptr != NULL; ptr++){
            free(*ptr); //free all the additional heap created
        }
    }                               
}

int main(int argc, char * args[]){
    while (1)
        run();        
    exit(0);      
}

// refs:
// https://www.geeksforgeeks.org/exec-family-of-functions-in-c/
// https://stackoverflow.com/questions/58161259/how-to-pass-parameters-and-use-them-using-exec-in-c
// https://stackoverflow.com/questions/33170802/c-does-freeing-an-array-of-pointers-also-free-what-theyre-pointing-to/33170941
// https://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length
// https://stackoverflow.com/questions/6282198/reading-string-from-input-with-space-character
// https://www.techonthenet.com/c_language/standard_library_functions/string_h/strcpy.php
// https://www.codingame.com/playgrounds/14213/how-to-play-with-strings-in-c/string-split 
// https://www.tutorialspoint.com/what-does-dereferencing-a-pointer-mean-in-c-cplusplus 
// https://stackoverflow.com/questions/21022644/how-to-get-the-real-and-total-length-of-char-char-array/21022695
// https://www.tutorialspoint.com/cprogramming/c_input_output.htm
// https://stackoverflow.com/questions/33884291/pipes-dup2-and-exec 
// https://stackoverflow.com/questions/8389033/implementation-of-multiple-pipes-in-c
// https://riptutorial.com/c/example/3250/calling-a-function-from-another-c-file