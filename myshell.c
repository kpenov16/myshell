#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

char *inputString(FILE* fp, size_t size){
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

int main(int argc, char * args[]){
        printf("# ");
        char * in; //= getchar();
        in = inputString(stdin,10);
        int size = strlen(in);
        char delim[] = " ";
        
        char * cp = (char *) malloc(size + 1); 
        strcpy(cp, in);
        size_t count_words = 0;
        char *ptr1 = strtok(cp, delim);
        for( ; ptr1 != NULL; count_words++){
            ptr1 = strtok(NULL, delim);
        }
        
        //count_words++; //for the terminating NULL 
        //there is '\n' in the end that we are going to override with NULL
        char * ar[count_words];//ls -la NULL
        int i = 0;
        char *ptr = strtok(in, delim);
        for ( ; ptr != NULL; i++){
            ar[i] = (char *) malloc(strlen(ptr) + 1);
            strcpy(ar[i], ptr);            
            ptr = strtok(NULL, delim);
        }
        ar[i] = malloc(sizeof NULL);
        ar[i] = NULL;                //there is '\n' in the end that we are going to override with NULL
        printf("count_words %zu, i %d\n",count_words,i);
        


    int pid = fork();
    if(pid != 0){
        printf("#########====parent=start=====#########\n");
        int r = waitpid(pid, NULL, 0);
        //free
        for(size_t i = 0; i < count_words; i++){
            free(ar[i]);
        }   
        free(in); 
        free(cp);

        printf("waitpid returned: %d\n", r);
        printf("#########====parent=end=====#########\n");
        exit(0);
    }else{
        printf("#########====child=start=====#########\n");
        for(size_t i = 0; i < count_words; i++){
            printf("%s\n",ar[i]);
        }
        execvp(ar[0], ar);
        //char * params[] = { "ls", "-la", NULL };
        //execvp(params[0], params);
        printf("#########====child=end=====#########\n");
        exit(0);
    }    
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
// 