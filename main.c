#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int argc; // global variable to count the arguments inserted by user

char* reserved_commands[4] = {
        "ls",       // array of strings for built in commands
        "cd",       // in the shell to be executed
        "rm",       // in the parent mode
        "exit"
};

// prompting the user for input and returning the line as a string
// using dynamic memory allocation for any size of input
// checking for empty input

char * modified_getline(){
    int readbytes;
    size_t size = 16;
    char *inputstring;
    puts("please enter a string");
    inputstring = (char*) malloc(size);
    readbytes = getline(&inputstring, &size, stdin);

    if (readbytes == -1)
        return NULL;
    else
        return inputstring;
}

// parsing the input string into an array of strings
// using the well-known C function strtok_r by space and \n delimeters
// terminating the array by a NULL pointer
// assigning the global variable argc to the arguments count

char** get_arguments (char* inputstring){
    char* token;
    char** strings = malloc(64);
    int location = 0, bufsize = 64;
    while ((token = strtok_r(inputstring, " ""\n", &inputstring))) {
        strings[location] = token;
        location++;
        if (location >= bufsize) {
            bufsize += 64;
            strings = realloc(strings, bufsize * sizeof(char*));
            if (!strings) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    strings[location] = NULL;
    argc = location;
    return strings;
}

// OS Stuff
// main function of execution and dealing with parent and child

int execute(char** strings) {
    pid_t pid, waitpid;

    // before forking we check for reserved words to execute
    // builtin commands of our shell in the parent mode

    if (strings[0] == NULL)     // checking for null input and returning if there
        return 0;

    else if (strcmp(strings[0], reserved_commands[3]) == 0)     // if input was exit we exit successfully
        exit(EXIT_SUCCESS);

    else if (strcmp(strings[0], reserved_commands[1]) == 0){    // if input was cd we change the directory
            chdir(strings[1]);
            return 0;
    }

    else if (strcmp(strings[0], reserved_commands[2]) == 0){    // if input was rm we remove the file if
        remove(strings[1]);                             // there was any of the same name
        return 0;
    }


        pid = fork();       // here we fork to generate a child

        if (!pid) {         // in child mode we execute other programs
            if (execvp(strings[0], strings) == -1)
                perror("myshell");

            if (strcmp(strings[0], reserved_commands[0]) == 0){     // checking for ls family commands to execute in child mode
                execvp(strings[0],strings);
            }

        } else if (pid > 0) {       // parent mode checking if there is '&' to wait or not
            if (strcmp(strings[argc - 1], "&") == 0)
                return 0;
            else
                waitpid = wait(NULL);       // there is '&' so we wait for child to terminate
        } else
            perror("myshell");          // error while forking
}

int main() {

    while (1){          // infinite loop until we recieve exit call
        char* x = modified_getline();   // prompting line input
        char** strings = get_arguments(x);      // storing arguments in an array of strings
        execute(strings);       // execution phase
    }
    return 0;
}