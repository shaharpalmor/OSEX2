//#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <malloc.h>
#include <stdlib.h>

#define COMMAND_LENGTH 42


int lsCommand(char *command){
    int i = 0;
    int j = 0;
    char *argv[COMMAND_LENGTH];// moving the arguments to an array
    char *argvCopy[COMMAND_LENGTH];// changing the order of the arguments - swaping
    pid_t result = fork();

    if(result == 0){ //the sons process
        char *split = strtok(command," ");
        split = strtok(NULL," ");
        while(split != NULL)
        {
            if(strcmp(split, "&") != 0)
                    argv[i] = split;
            split = strtok(NULL," ");
            i++;
        }
        i--;

        for (j = 0; j <= i; j++){
            argvCopy[j] = argv[i];
            i--;
        }
        argvCopy[j] = argv[i];

        execvp("ls",argvCopy);

    } else if (result ==-1){
        printf("could not create process for the ls command");
    } else {// father process
        printf("%d\n", result);
        if(isAmpersent(command)) // the father has no need to wait for the sons process
            return result;
        else
           waitpid(result, NULL, 0);
    }
    return result;
}

int isAmpersent(char *command){

    char *argv[COMMAND_LENGTH];
    char cmd[COMMAND_LENGTH];
    strcpy(cmd,command);
    char *split = strtok(cmd," ");
    split = strtok(NULL," ");
    int i = 1;

    while(split != NULL)
    {
        argv[i] = split;
        split = strtok(NULL," ");
        i++;
    }
    if (strcmp(argv[i-1],"&") == 0){
        return 1;// means that the father process should run and not wait for the son
    }
    return 0;// means that the father process should not run and wait for the son

}

int catCommand(char *command){
    int j = 0;
    char *argv[COMMAND_LENGTH];// moving the arguments to an array
    pid_t result = fork();
    if(result == 0){ //the sons process
        makeArgumentsArray(command,argv);
        j = execvp(argv[0],argv);
        if (j == -1){
            fprintf(stderr, "Failed to execute %s\n", argv[0]);
            perror("execvp");
            exit (EXIT_FAILURE);
        }
    } else if (result ==-1){
        printf("could not create process for the ls command");
    } else {// father process
        printf("%d\n", result);
        if(isAmpersent(command)) // the father has no need to wait for the sons process
            return result;
        else
            waitpid(result, NULL, 0);
    }
    return result;
}

int sleepCommand(char *command){
    int j = 0;
    char *argv[COMMAND_LENGTH];// moving the arguments to an array
    pid_t result = fork();

    if(result == 0){ //the sons process
        makeArgumentsArray(command,argv);
        j = execvp(argv[0],argv);
        if (j == -1){
            fprintf(stderr, "Failed to execute %s\n", argv[0]);
            perror("execvp");
            exit (EXIT_FAILURE);
        }
    } else if (result ==-1){
        printf("could not create process for the ls command");
    } else {// father process
        printf("%d\n", result);
        if(isAmpersent(command)) // the father has no need to wait for the sons process
            return result;
        else
            waitpid(result, NULL, 0);
    }
    return result;
}

void makeArgumentsArray(char *command, char *argv[COMMAND_LENGTH]){
    int i = 0;
    char *split = strtok(command," ");
    //split = strtok(NULL," ");
    while(split != NULL)
    {
        if(strcmp(split, "&") != 0) {
            argv[i] = split;
        }
        split = strtok(NULL," ");
        i++;
    }
    argv[i] = NULL;
}

int main() {

    char *prompt = "prompt> ";
    char line[COMMAND_LENGTH];
    char cmd[COMMAND_LENGTH];
    pid_t pid; // the process that is currently running.

    while (1) {

        printf("%s", prompt);
        fgets(line, COMMAND_LENGTH, stdin);
        line[strlen(line)-1]='\0';
        strcpy(cmd,line);
        strtok(cmd, " ");

        if (strcmp(cmd,"ls") == 0){
            pid = lsCommand(line);
        } else if(strcmp(cmd,"cat") == 0){
            pid = catCommand(line);
        } else if(strcmp(cmd,"sleep") == 0){
            pid = catCommand(line);
        } else if(strcmp(cmd,"cd") == 0){

        } else if (strcmp(cmd,"jobs") == 0){

        } else if(strcmp(cmd,"exit") == 0)
            break;
        else
            continue;

    }

    return 0;
}

