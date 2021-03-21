//
//  main.c
//  SimpleShell
//
//  Created by Rohan Sachdeva on 17/01/21.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

char **split_command(char *command, char **ptr_array);
void cmd_execute(char ** cmd_parameters);
int ptr_array_size = 50;

int main(int argc, const char * argv[]) {
    printf("------- Hello World! Welcome to my Simple Shell -------\n\n\n\n");
    
    char exit_cmd[]="exit";
    
    while (1) {
        char *command_input=NULL;          /* stores the address of memory block allocated by getline */
        size_t byte_size = 0;              /* stores no. of bytes allocated by getline */
        ssize_t char_count;
        char **ptr_array = malloc(sizeof(char *) * ptr_array_size);  /* array of 50 char pointers*/
        if (!ptr_array) {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
              }
        
        printf("$");
        
        /* Reading command input. getline will dynamically allocate the memory block and assign command_input to point at it  */
        char_count = getline(&command_input, &byte_size, stdin);
        if(char_count == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
            
        }
        
        if(command_input[char_count-1] == '\n') {   /* Remove trailing newline character*/
                    command_input[char_count-1] = '\0';
        }
        
        if(!strcmp(command_input, exit_cmd))
            break;
        
        ptr_array = split_command(command_input,ptr_array);  /* Split input command into array of strings */
        cmd_execute(ptr_array);
        
        free(command_input);
        free(ptr_array);
      
        
    }
    return 0;
}

char **split_command(char *command,char **ptr_array){
    
    char *str_ptr;
    int ptr_array_index = 0;
    str_ptr = strtok(command, " ");  /* splitting input command by whitespace and returning first string */

    
    while(str_ptr != NULL) {

        ptr_array[ptr_array_index] = str_ptr;
        str_ptr = strtok(NULL, " ");        /* continue splitting from the end of the previous string */
        ptr_array_index ++;
        
        if(ptr_array_index == ptr_array_size){ /* check if ptr array is full then reallocate memory */
            ptr_array_size += ptr_array_size;
            ptr_array = realloc(ptr_array, ptr_array_size * sizeof(char*));
            
            if (!ptr_array) {
                    fprintf(stderr, "error: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                  }
        }
    }
    ptr_array[ptr_array_index] = NULL;    /* storing NULL at the last index to mark end of array */
    return ptr_array;
}

void cmd_execute(char **cmd_parameters){
    
      if(*cmd_parameters == NULL){
          return;
        }
    if (strcmp(cmd_parameters[0], "cd") == 0) {
        if(chdir(cmd_parameters[1]) == -1)
        {
            fprintf(stderr, "Error: %s\n", strerror(errno));
        }
        return;
    }

         pid_t  pid = fork();
         int status;

         if (pid < 0) {                                              /* fork a child process */
             fprintf(stderr, "error: %s\n", strerror(errno));
             exit(EXIT_FAILURE);
         }
         else if (pid == 0) {                                        /* child process: execute the command  */
             printf("%s",*cmd_parameters);
              if (execv(*cmd_parameters, cmd_parameters) < 0) {
                  fprintf(stderr, "error: %s\n", strerror(errno));
                  exit(EXIT_FAILURE);
              }
         }
         else {                                  /* parent wait for child completion    */
             while (wait(&status) != pid);
         }
}

