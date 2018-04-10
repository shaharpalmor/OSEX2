#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

#define COMMAND_LENGTH 42
#define FAILURE -1
#define SUCCESS 1


/**
 * this functions gets a string that represents the command and its semi commands, and it cuts it
 * and make an array that would feet the execvp command.
 * @param command is the full string.
 * @param argv is the array that wold be sent in the execvp command.
 */
void makeArgumentsArray(char *command, char *argv[COMMAND_LENGTH]) {
    int i = 0;
    char *split = strtok(command, " ");
    while (split != NULL) {
        if (strcmp(split, "&") != 0) {
            argv[i] = split;
        }
        split = strtok(NULL, " ");
        i++;
    }
    argv[i] = NULL;
}


/**
 * this function checks if at the end of the string there is an &, so that it means that the process shuld
 * run individualy and the father process do not have to wait for it.
 * @param command is the string
 * @return 1 if there is an & 0 if it does not.
 */
int isAmpersent(char *command) {

    char *argv[COMMAND_LENGTH];
    char cmd[COMMAND_LENGTH];
    strcpy(cmd, command);
    char *split = strtok(cmd, " ");
    argv[0] = split;
    split = strtok(NULL, " ");
    int i = 1;

    while (split != NULL) {
        argv[i] = split;
        split = strtok(NULL, " ");
        i++;
    }
    if (strcmp(argv[i - 1], "&") == 0) {
        return 1;// means that the father process should run and not wait for the son
    }else {
        return 0;// means that the father process should not run and wait for the son
    }
}


/**
 * this function executes the command, it does the fork for the sons process, and also continue the process of the
 * father.
 * @param command is the command to execute.
 * @return the pid of the sons process - the result of the fork.
 */
int execCommand(char *command) {
    int j = 0;
    char *argv[COMMAND_LENGTH];// moving the arguments to an array
    pid_t result = fork();
    if (result == 0) { //the sons process
        makeArgumentsArray(command, argv);
        j = execvp(argv[0], argv);
        if (j == -1) {
            fprintf(stderr,"Error in system call");
            printf("\n");
            exit(EXIT_FAILURE);
        }
    } else if (result == -1) {
        printf("Error in system call");
    } else {// father process
        printf("%d\n", result);
        if (isAmpersent(command)) // the father has no need to wait for the sons process
            return result;
        else
            waitpid(result, NULL, 0);
    }
    return result;
}

/**
 * deals with the regular cd command which is  entering the destination in the argv[1]. also deals with the option
 * that argv[1] is minus and saves in the last dir the last dir it was and prints it.
 * @param dir is the dir to go to.
 * @param lastDir is the lat dir it was into.
 * @param flag is if there is minus or not
 * @return failure or success.
 */
int cdComplexCommand(char *dir, char lastDir[COMMAND_LENGTH], int flag){
    char currentDir[COMMAND_LENGTH];
    getcwd(currentDir,COMMAND_LENGTH);
    if(chdir(dir)==FAILURE){
        fprintf(stderr,"Error in system call");
        printf("\n");
        return FAILURE;
    }else{
        if(flag){
            printf("%s\n",dir);
        }
        strcpy(lastDir,currentDir);
        return SUCCESS;
    }
}

/**
 * make the cd command. if there is no argv[1] than it goes to the hone directory.
 * also deals with the option of "~" - going to the home directory
 * also deals with the - option, going to the privious directory.
 * @param command  is the command.
 * @return if succeeded or not.
 */
int cdCommand(char *command, char dirToGo[COMMAND_LENGTH]){
    char *argv[COMMAND_LENGTH];// moving the arguments to an array
    makeArgumentsArray(command, argv);
    // cd does not signify which directory to go to
    if(argv[1] == NULL){
        char currentDir[COMMAND_LENGTH]; //buffer to save the current directory
        getcwd(currentDir,COMMAND_LENGTH); //get the current directory
        if(chdir(getenv("HOME"))!=FAILURE){
            strcpy(dirToGo,currentDir);
            return SUCCESS;
        }else{
            fprintf(stderr,"Error in system call");
            printf("\n");
            return  -1;
        }
    }else{ //argv[1]!=Null
        // cd - prints the last directory we were before
        if((strcmp(argv[1], "-") == 0) &&argv[2] == NULL){
            return cdComplexCommand(dirToGo, dirToGo, 1);
        }else if((strcmp(argv[1], "~") == 0) &&argv[2] == NULL){
            // going to the Home directory with ~
            if(chdir(getenv("HOME"))!=FAILURE){
                // any ways saves the home directory as the last directory we did cd to.
                strcpy(dirToGo,"HOME");
                return SUCCESS;
            }else{
                fprintf(stderr,"Error in system call");
                printf("\n");
                return  -1;
            }
        }else{
            return cdComplexCommand(argv[1],dirToGo,0);
        }
    }
}



int main() {
    char *prompt = "prompt>";
    char line[COMMAND_LENGTH];
    char cmd[COMMAND_LENGTH];
    //char *directoryOld;
    char dirOld[COMMAND_LENGTH];
    pid_t pid; // the process that is currently running.
    char jobsArray[COMMAND_LENGTH][COMMAND_LENGTH];
    int pidArray[COMMAND_LENGTH];
    int i = 0;
    int j;
    line[0] = 0;

    while (1) {
        printf("%s", prompt);
        fgets(line, COMMAND_LENGTH, stdin);
        line[strlen(line) - 1] = '\0';
        strcpy(cmd, line);
        strtok(cmd, " ");
        if(line[0] == 0){
            continue;
        }
        if (strcmp(cmd, "exit") == 0) {
            printf("%d \n", getpid());
            exit(0);
        } else if (strcmp(cmd, "cd") == 0) {
            printf("%d \n", getpid());
            pid = cdCommand(line, dirOld);
        } else if (!strcmp(cmd, "jobs") == 0) {
            pid = execCommand(line);
            pidArray[i] = pid;
            strcpy(jobsArray[i], cmd);
            i++;
        } else if (strcmp(cmd, "jobs") == 0) { // jobs command was asked.
            for (j = 0; j < i; j++) {
                pid_t pidProcess = waitpid(pidArray[j], NULL, WNOHANG);
                if (pidProcess == 0) {
                    printf("%d    %s", pidArray[j], jobsArray[j]);
                    printf("\n");
                }
            }

        } else {
            continue;
        }
    }
    return 0;
}
