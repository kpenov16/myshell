#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>



char * input_string(FILE* fp, size_t size){
//The size is extended by the input with the value of the provisional
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(char)*size);//size is start size
    if(!str)return str;
    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(char)*(size+=16));
            if(!str)return str;
        }
    }
    str[len++]='\0';

    return realloc(str, sizeof(char)*len);
}

size_t get_words_count(size_t lenght_str,char * input_str, char delim[]){
    char * cp = (char *) malloc(lenght_str + 1); 
    strcpy(cp, input_str);
    size_t count_words = 0;
    char *ptr1 = strtok(cp, delim);
    for( ; ptr1 != NULL; count_words++){
        ptr1 = strtok(NULL, delim);
    }
    free(cp);
    return count_words;
}


char ** get_commands_arr(size_t count_words, char * input_str, char delim[]){
    char * in = (char *) malloc(count_words); 
    strcpy(in, input_str);
    
    //count_words++; //for the terminating NULL 
    //there is '\n' in the end that we are going to override with NULL
    char ** ar = malloc((count_words+1) * sizeof(char *));//ls -la NULL
    int i = 0;
    char *ptr = strtok(in, delim);
    size_t c_words = 0;
    for ( ; ptr != NULL; i++){
        ar[i] = (char *) malloc(strlen(ptr) + 1);
        strcpy(ar[i], ptr);            
        ptr = strtok(NULL, delim);
    }
    //ar[i] = malloc(sizeof NULL);
    //ar[i] = NULL;                //there is '\n' in the end that we are going to override with NULL

    free(in);

    return ar; 
    //remember to free the ar's elementer out of this function 
    //- maybe something like free(arr[0]); free(arr[1]); if ar length is 2 
    //or free(arr) 
}

void run(){
    printf("# ");
    //start
    char * in_tmp; 
    in_tmp = input_string(stdin,10);
    int size = strlen(in_tmp);    
    
    char in[size+1];
    strcpy(in, in_tmp);
    in[size] = '\0';
    /*
    char in2[size+1];
    strcpy(in2, in_tmp);
    in[size] = '\0';
    */
    free(in_tmp); 
    //end

    size_t count_commands = get_words_count(size, in, "|");
    //printf("count_commands: %zu",count_commands);
    char ** p_ar;
    //char * in2[count_commands];
    char ** in2[count_commands];
    if(count_commands > 0){
        p_ar = get_commands_arr( count_commands, in, "|" );        
        for(size_t i = 0; i < count_commands; i++){
            int size = strlen(p_ar[i]);
            char delim[] = " ";
            size_t count_words = get_words_count(size, p_ar[i], delim);

            int ii = 0;
            char *ptr = strtok(p_ar[i], delim);
            size_t c_words = 0;
            char ** ar = malloc(count_words * sizeof(char *));;//ls -la NULL
            for ( ; ptr != NULL; ii++){
                ar[ii] = (char *) malloc(strlen(ptr) + 1);
                strcpy(ar[ii], ptr);            
                ptr = strtok(NULL, delim);                
            }
            ar[ii] = malloc(sizeof NULL);
            ar[ii] = NULL;                //there is '\n' in the end that we are going to override with NULL
            printf("count_words %zu, i %d\n",count_words,ii);        
            in2[i] = ar;

            //printf("commands: %s", p_ar[i]);
            free(p_ar[i]);
        }
        //free(p_ar);

        // in2 ser sådan ud:
        //  in2[0] = {"ls", "-la", NULL }; //hvor den string array ligger i heap men in2 pointers to arr liger i stack, måske :)
        //  in2[1] = {"grep", "shel", NULL};
        for(size_t i=0; i < count_commands; i++){            
            for (char ** ptr = in2[i]; *ptr != NULL; ptr++){
                printf("%s\n", *ptr);
                free(*ptr);
            }
        }
    }

    char delim[] = " ";
    size_t count_words = get_words_count(size, in, delim);
    
    size_t pipes = 0;
    
    //count_words++; //for the terminating NULL 
    //there is '\n' in the end that we are going to override with NULL
    char * ar[count_words];//ls -la NULL
    int i = 0;
    char *ptr = strtok(in, delim);
    size_t c_words = 0;
    for ( ; ptr != NULL; i++){
        ar[i] = (char *) malloc(strlen(ptr) + 1);
        strcpy(ar[i], ptr);            
        ptr = strtok(NULL, delim);
        if(ar[i][0] == '|'){
            pipes++;
        }
    }
    ar[i] = malloc(sizeof NULL);
    ar[i] = NULL;                //there is '\n' in the end that we are going to override with NULL
    printf("count_words %zu, i %d\n",count_words,i);

/*
    //count_words++; //for the terminating NULL 
    //there is '\n' in the end that we are going to override with NULL
    char * ar2d[pipes+1][count_words];//ls -la NULL
    int i = 0;
    char *ptr = strtok(in2, delim);
    size_t c_words = 0;
    for ( ; ptr != NULL; i++){
        ar2d[i][0] = (char *) malloc(strlen(ptr) + 1);
        strcpy(ar2d[i][0], ptr);            
        ptr = strtok(NULL, delim);
        if(ar2d[i][0] == '|'){
            pipes++;
        }
    }
    ar2d[i][0] = malloc(sizeof NULL);
    ar2d[i][0] = NULL;                //there is '\n' in the end that we are going to override with NULL
    printf("count_words %zu, i %d\n",count_words,i);
*/


    if(pipes > 0){
        pid_t pid;
        int fd[2];
        pipe(fd);
        pid = fork();

        if(pid == 0){
            printf("#########====pipe=writer=child=start=====#########\n");
            
            // 0  stdin , 1 stdout, 2 err
            dup2(fd[0], 1);
            close(fd[0]);
            close(fd[1]);

            execvp(ar[0], ar);

            fprintf(stderr, "Failed to execute '%s'\n", "firstcmd");
            exit(1);
        }else{
            pid = fork();
            if(pid == 0){
                printf("#########====pipe=reader=child=start=====#########\n");
                // 0  stdin , 1 stdout, 2 err
                dup2(fd[1], 0);
                close(fd[0]);
                close(fd[1]);

                execvp(ar[0], ar);

                fprintf(stderr, "Failed to execute '%s'\n", "firstcmd");
                exit(1);
            }else{
                int status;
                close(fd[0]);
                close(fd[1]);
                waitpid(pid, &status, 0);
            }
        }

    }else{
        int pid = fork();
    
        if(pid == 0){ //in child
            printf("#########====child=start=====#########\n");
            /*for(size_t i = 0; i < count_words; i++){
                printf("%s\n",ar[i]);
            }*/
            execvp(ar[0], ar);
            printf("we should never see this!#########====child=end=====#########\n");
            //char * params[] = { "ls", "-la", NULL };
            //execvp(params[0], params);
            exit(0);
        }else{ //in parent
            printf("#########====parent=start=====#########\n");
            int r = waitpid(pid, NULL, 0);
            //free
            for(size_t i = 0; i < count_words; i++){
                free(ar[i]);
            }        
            printf("waitpid returned: %d\n", r);
            printf("#########====parent=end=====#########\n");

            //run();

            //exit(0);
        }    

    }

}

int main(int argc, char * args[]){
    while (1)
    {
        run();
    }    
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
