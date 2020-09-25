#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

//we dont know how big is the input and therefore we need a function that can take 
//variable lenght input 
char * input_string(FILE* fp, size_t size){ // https://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length
//The size is extended by the input with the value of the provisional
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
    char * input_tmp = (char *) malloc(lenght_str + 1); 
    strcpy(input_tmp, input_str);
    size_t count_words = 0;
    char *ptr1 = strtok(input_tmp, delim);
    for( ; ptr1 != NULL; count_words++){
        ptr1 = strtok(NULL, delim);
    }
    free(input_tmp);
    return count_words;
}

//this is a general function for tokonization and has no side effects on the input, 
//returns pointer to the tokenized string f.eks.: "This is tokenized" -> {"This", "is", "tokenized"} 
                                            // or "ls -la | grep shell" -> {"ls -la ", "grep shell"}
char ** get_commands_arr(size_t count_words, char * input_str, char delim[]){
    char * in_tmp = (char *) malloc(count_words); //we don't want to set NULL in the original string when we tokenize
    strcpy(in_tmp, input_str);                    //so we make a temp copy of it that we free from heap after that 
    
    //there is '\n' in the end that we are going to override with NULL
    char ** ar = malloc((count_words+1) * sizeof(char *));//ls -la NULL //there is + 1 for the additional NULL element in the end
    int i = 0;
    char *ptr = strtok(in_tmp, delim);  //start tokonization for the given delimiter " " or "|" in our case
    size_t c_words = 0;
    for ( ; ptr != NULL; i++){
        ar[i] = (char *) malloc(strlen(ptr) + 1); //alocate space for the string on heap
        strcpy(ar[i], ptr);                       //copy string to the alocated space
        ptr = strtok(NULL, delim);
    }
    //ar[i] = malloc(sizeof NULL); //**I keep this here to remind me of something :)
    //ar[i] = NULL;                //there is '\n' in the end that we are going to override with NULL
    free(in_tmp);
    return ar; 
    //remember to free the ar's elementer out of this function 
    //- maybe something like free(arr[0]); free(arr[1]); if ar length is 2 
    //or free(arr) 
}

void run(){
    printf("# ");
    
    char * in_tmp; 
    in_tmp = input_string(stdin,10); //get pointer to user input 
    int size = strlen(in_tmp);       
    
    char in[size+1];
    strcpy(in, in_tmp);  //copy str from heap into local var on the stack            
    in[size] = '\0';     //add end of string char in any case ..
    free(in_tmp);        //free the heap alocated space from the original input
   

    size_t count_commands = get_words_count(size, in, "|");  //if pipe in the input we need the number of commands
    char ** in2[count_commands];  //create array to hold pointers to each command, each command will be array of strings with null element in the end

    char ** ar_tmp; // we are going to tokenize each command on a " " now but we don't need the originals - we call them tmp 
    ar_tmp = get_commands_arr( count_commands, in, "|" );        
    for(size_t i = 0; i < count_commands; i++){ //for each command in the array we prepare 2d array - one row per command
        int size = strlen(ar_tmp[i]); //how may chars a command has
        char delim[] = " ";
        size_t count_words = get_words_count(size, ar_tmp[i], delim); //how many words are there in a command

        //tokenize the command strings and populate the 2d array so that can be used for the pipes
        int ii = 0;
        char *ptr = strtok(ar_tmp[i], delim);
        size_t c_words = 0;
        char ** ar = malloc(count_words * sizeof(char *));;//ls -la NULL //we need a pointer for each element of each command
        for ( ; ptr != NULL; ii++){
            ar[ii] = (char *) malloc(strlen(ptr) + 1); //alocate space for the element
            strcpy(ar[ii], ptr);    //copy the element in the next position of the array            
            ptr = strtok(NULL, delim);                
        }
        ar[ii] = malloc(sizeof NULL);
        ar[ii] = NULL;                //there is '\n' in the end that we are going to override with NULL
                                      //arr[ii] looks like this {"ls", "-la", NULL}
        in2[i] = ar;                  //in2  looks like this in2[0] = {"ls", "-la", NULL} in2[0] = {"grep", "shell", NULL} ..
        free(ar_tmp[i]); //free the heap of the temp array
    }

    // in2 ser sådan ud:
    //  in2[0] = {"ls", "-la", NULL }; //hvor den string array ligger i heap men in2 pointers to arr liger i stack, måske :)
    //  in2[1] = {"grep", "shel", NULL};
    for(size_t i=0; i < count_commands; i++){            
        for (char ** ptr = in2[i]; *ptr != NULL; ptr++){
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
            execvp(in2[i][0], in2[i]);
        }else{
            //parent forks the next pipe
        }
    }
    //all pipes are chaneld 
    for(size_t i = 0; i < (count_commands-1)*2; i++ )
        close(fds[i]); //close all pipe ends - this is the parent, all the children had their "copies" of the fds becouse of the parent 
                       //we have closed them in the children, remember, and now we do it in the parent
    int status;
    waitpid(pid, &status, 0); //wait for the last child to finish work

    for(size_t i=0; i < count_commands; i++)            
        for (char ** ptr = in2[i]; *ptr != NULL; ptr++)
            free(*ptr); //free all the additional heap created
    for(size_t i=0; i < count_commands; i++)            
        free(in2[i]); //free all the additional heap created                
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