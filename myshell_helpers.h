#include <stdio.h>

char ** get_commands_arr(size_t count_words, char * input_str, char delim[]);

char * get_input_string(FILE* fp, size_t size);

size_t get_words_count(size_t lenght_str,char * input_str, char delim[]);