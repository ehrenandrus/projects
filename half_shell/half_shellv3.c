/**
 * @author Ehren Andrus
 * @version 3.1
 * @since 11/8/2022
 * 
 * This is my own simple shell called half_shell.
 * I embrace my nerdom and enjoy the pun.
 * Version 2 supports pipes. and color coding and aliases for ls --color=auto and grep --color=auto
 * Version 3.1 supports >> > < | and aliases for windows and home directories
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <locale.h>
#include <sys/sendfile.h>
#include<sys/stat.h>
#include <fcntl.h>

struct myPointer{
    char** ptr;
    size_t size;
};

struct myIntPointer{
    int **ptr;
    size_t size;
};

const int arrIOLength = 100;
const int arrIOSize = 2;
char *readLine();
struct myPointer parseLine(char*);
int interpretLine(struct myPointer);
int executeCommand(char **, int, int);
int builtInCommands(char **);
char **copyArguments(char **, char **, int, int);
int printCommandLine(char *);
int parseCommands(struct myIntPointer, struct myPointer, int [arrIOLength][arrIOSize], int, int, 
                int, int, int, int, int);
int readWriteFile(char **, int, const char [1]);
int readFileWriteToPipe(char **, int, const char [1]);
struct myPointer makeAliases(struct myPointer, int);
void green();
void white();
void blue();
void resetColor();

int main(int argc, char *argv[]){
    char* buffer = NULL;
    //char** parsedLine = NULL;
    char** clear = NULL;
    int loop = 0;
    size_t sizeD = 1024;
    char* directory = (char*)malloc(sizeof(char) * sizeD);

    struct myPointer parsedLine;
    parsedLine.ptr = NULL;
    parsedLine.size = 0;

    system("clear"); // clear stdout
    //printIntro();
    while(loop == 0){
        
        while(getcwd(directory, sizeD) == NULL){ //stores dynamic working directory in directory pointer and if it fails allocate more space to directory and try again
            sizeD *= 2;
            directory = (char*)realloc(directory, sizeof(char) * sizeD);
        }
        printCommandLine(directory);
        buffer = readLine();
        parsedLine = parseLine(buffer);
        loop = interpretLine(parsedLine);
    }


    //free memory
    free(parsedLine.ptr);
    free(buffer);
    free(directory);
    return(0);
}

//read in line from terminal
char *readLine(){
    char *buffer = NULL;  //initialize buffer to Null
    size_t size = 0;   //size will be dynamically allocated by getline

    if (getline(&buffer, &size, stdin) == -1){  //read input and if input faiLS print error message
        puts("ERROR. Unable to read input.");
    }

    buffer[strcspn(buffer, "\n")] = 0; //get rid of newline character

    return buffer;
}

//parse the line on the spaces
struct myPointer parseLine(char* buffer){
    int lengthOfParsedLine = 4;
    struct myPointer parsedLine;
    parsedLine.ptr = (char**)malloc(sizeof(char*) * lengthOfParsedLine);   //allocate 4 bytes of space to start for pointer
    parsedLine.size = 0;
    const char* delimiter = " ";
    int count = 0;
    
    if(parsedLine.ptr == NULL){ //check for memory allocation error
        puts("There was a memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    parsedLine.ptr[count] = strtok(buffer, delimiter);  //get first token in pointer
    parsedLine.size ++;
    
    while(parsedLine.ptr[count] != NULL){ //while pointer isn't null
        if( count >= lengthOfParsedLine){ // if count is bigger than the length of parsedLine
            parsedLine.ptr = (char**)realloc(parsedLine.ptr, count * 2 * sizeof(char*));    //reallocate more memory for parsedLine
        }
        //make sure pointer has memory eLSe print error message and exit
        if(parsedLine.ptr == NULL){
            puts("There was a memory allocation error\n");
            exit(EXIT_FAILURE);
        }
    
        parsedLine.ptr[++count] = strtok(NULL, delimiter);  //incremenet counter then get next token
        parsedLine.size ++;
           
        if( count >= lengthOfParsedLine){ // if count is bigger than the length of parsedLine
            parsedLine.ptr = (char**)realloc(parsedLine.ptr, count * 2 * sizeof(char*));    //reallocate more memory for parsedLine
        }
        //make sure pointer has memory eLSe print error message and exit
        if(parsedLine.ptr == NULL){
            puts("There was a memory allocation error\n");
            exit(EXIT_FAILURE);
        }
        parsedLine = makeAliases(parsedLine, count);
        count = parsedLine.size - 1;
    }

    return parsedLine;
}

//make aliases from commands
struct myPointer makeAliases(struct myPointer parsedLine, int count){
    if(count > 1){ //if there is more than one argument
        // alias ls command to ls --color=auto and grep --color=auto to show color coded files and grep returns
        if(strcmp(parsedLine.ptr[count - 1], "ls") == 0 && strcmp(parsedLine.ptr[count], "--color=auto") != 0){
            parsedLine.ptr[++count] = parsedLine.ptr[count];
            parsedLine.ptr[count - 1] = "--color=auto";
            parsedLine.size ++;
        }else if(strcmp(parsedLine.ptr[count - 1], "grep") == 0 && strcmp(parsedLine.ptr[count], "--color=auto") != 0){
            parsedLine.ptr[++count] = parsedLine.ptr[count];
            parsedLine.ptr[count - 1] = "--color=auto";
            parsedLine.size ++;
        }
    }else { //if there is only 1 argument (ls can be the only one)
        if(strcmp(parsedLine.ptr[count - 1], "ls") == 0){ //alias ls
            parsedLine.ptr[++count] = parsedLine.ptr[count];
            parsedLine.ptr[count - 1] = "--color=auto";
            parsedLine.size ++;
        }
        if(strcmp(parsedLine.ptr[count - 1], "windows") == 0){ //windows alias to get to my windows files quickly
            parsedLine.ptr[0] = "cd";
            count++;
            parsedLine.ptr[count - 1] = "/mnt/c/Users/easym";
            parsedLine.size++;
        }else if(strcmp(parsedLine.ptr[count - 1], "cd") == 0 && parsedLine.ptr[count] != NULL){
            if(strcmp(parsedLine.ptr[count], "~") == 0){ //alias to get to my linux home quickly
                parsedLine.ptr[count] = "/home/easymoney8000";
            }
        }
    }

    return parsedLine;
}

//find input and output redirects, decide if command is built in or not.
int interpretLine(struct myPointer parsedLine){
    const char* commands[] = {"cd", "exit"};
    const int sizeOfCommands = 2;
    const int doubleRightArrow = 0;
    const int pipeNum = 1;
    const int singleRightArrow = 2;
    const int singleLeftArrow = 3;
    int ioCount = 0;
    int count = 0;
    int index = 0; //refers to arrIO 1st dimension
    int type = 1;  //refers to arrIO 2nd dimension
    int arrIO[arrIOLength][arrIOSize];
    struct myIntPointer ioRedirects;
    size_t ioRedirectsSize = 4;
    ioRedirects.ptr = (int**)malloc(sizeof(int*) * ioRedirectsSize);
    ioRedirects.size = 0;
    if(ioRedirects.ptr == NULL){ //check for malloc error
        puts("Thre was a malloc error\n");
        exit(EXIT_FAILURE);
    }
    
    //check for built in commands
    for( int i = 0; i < sizeOfCommands; i ++){ 
        if(strcmp(parsedLine.ptr[0], commands[i]) == 0){
            builtInCommands(parsedLine.ptr);
            return(0);  // get out of function
        }
    }
    //count and keep track of number of pipes in input and their locations
    while(parsedLine.ptr[count] != NULL){
        if((strcmp(parsedLine.ptr[count], ">>") == 0)){// test for  output redirect append
                if(count >= ioRedirectsSize){ //resize ptr if too small
                    ioRedirectsSize *= 2;
                    ioRedirects.ptr = (int**)realloc(ioRedirects.ptr, sizeof(int**) * ioRedirectsSize);
                }
                if(ioRedirects.ptr == NULL){    //realloc error checking
                    puts("Thre was a realloc error\n");
                    exit(EXIT_FAILURE);
                }
                arrIO[ioCount][index] = count; //save index
                arrIO[ioCount][type] = doubleRightArrow; // 0 = >>
                ioRedirects.ptr[ioCount] = &arrIO[ioCount][index]; //copy index
                ioCount++;
        }else if(strcmp(parsedLine.ptr[count], "|") == 0){ //test for pipes
                if(count >= ioRedirectsSize){
                    ioRedirectsSize *= 2;
                    ioRedirects.ptr = (int**)realloc(ioRedirects.ptr, sizeof(int**) * ioRedirectsSize);
                }
                if(ioRedirects.ptr == NULL){ //realloc error checking
                    puts("Thre was a realloc error\n");
                    exit(EXIT_FAILURE);
                }
                arrIO[ioCount][index] = count; //save index
                arrIO[ioCount][type] = pipeNum; // 1 = |
                ioRedirects.ptr[ioCount] = &arrIO[ioCount][index]; //copy index
                ioCount++;
        }else if(strcmp(parsedLine.ptr[count], ">") == 0){ //test for output redirect overwrite
                if(count >= ioRedirectsSize){
                    ioRedirectsSize *= 2;
                    ioRedirects.ptr = (int**)realloc(ioRedirects.ptr, sizeof(int**) * ioRedirectsSize);
                }
                if(ioRedirects.ptr == NULL){ //realloc error checking
                    puts("Thre was a realloc error\n");
                    exit(EXIT_FAILURE);
                }
                arrIO[ioCount][index] = count; //save index
                arrIO[ioCount][type] = singleRightArrow; // 2 = >
                ioRedirects.ptr[ioCount] = &arrIO[ioCount][index]; //copy index
                ioCount++;
        }else if(strcmp(parsedLine.ptr[count], "<") == 0){ //test for input redirect
                if(count >= ioRedirectsSize){
                    ioRedirectsSize *= 2;
                    ioRedirects.ptr = (int**)realloc(ioRedirects.ptr, sizeof(int**) * ioRedirectsSize);
                }
                if(ioRedirects.ptr == NULL){ //realloc error checking
                    puts("Thre was a realloc error\n");
                    exit(EXIT_FAILURE);
                }
                arrIO[ioCount][index] = count; //save index
                arrIO[ioCount][type] = singleLeftArrow; // 2 = >
                ioRedirects.ptr[ioCount] = &arrIO[ioCount][index]; //copy index
                ioCount++;
            }
        count++;
    }

    //parsed different commands inbetween input and output redirections
    parseCommands(ioRedirects, parsedLine, arrIO, index, type, ioCount, doubleRightArrow, pipeNum, singleRightArrow, singleLeftArrow);
 
    return(0);
}

int executeCommand(char **parsedLine, int in, int out){
    pid_t pid;
    int status;
    //puts("made it to exec");
    pid = fork(); //copy old process and start new child procees
    if (pid == 0){ //child process that will execute command

        if(in != 0){ //if not stdin
            dup2(in, 0); //change stdin to in file descriptor for redirect
            close(in);
        }
        if(out != 1){
            dup2(out, 1); //change stdout to out file descriptor for redirect
            close(out);
        }
        if(execvp(*parsedLine, parsedLine) < 0){
            perror("Error with in execute command function");
        }
    } else if (pid < 0){    // error occured
        perror("Error occured in executeCommand");
    } else{ // parent process gets the pid of the child process
        do{
            pid = waitpid(pid, &status, WUNTRACED); //get info from child process

        }while (!WIFEXITED(status) && !WIFSIGNALED(status));//keep looping until child process has finished
    }

    return(0);
}

int parseCommands(struct myIntPointer ioRedirects, struct myPointer parsedLine, int arrIO[arrIOLength][arrIOSize], int index, int type, 
                int ioCount, int doubleRightArrow, int pipeNum, int singleRightArrow, int singleLeftArrow){
        //stdin and stdout file descriptors
    const char append[] = "a";
    const char writeOver[] = "w";
    int in = 0;
    int out = 1;
    int fd;
    int notJUMP = -1;
    int jumpHappened = notJUMP;
    int saved_stdin = 0;
    int saved_stdout = 1;
    saved_stdin = dup(0); //save stdin
    saved_stdout = dup(1); //save stdout
    size_t newArgsSize = 1024;
    char **newArguments = (char**)malloc(sizeof(char*) * newArgsSize);
    char **savedArguments = NULL;
    char tempArr[50];

    if(ioCount == 0){//if command isn't built in and has no io redirection
         executeCommand(parsedLine.ptr, in, out); 
    } else if(ioCount > 0) { //if io redirection
        
        int startIndex = 0;
        int stopIndex = 0;
        int pipefd[2]; //arrPipesay of file descriptors for in and out
        //get start and end indexes to send as arguments to copyArguments

        for(int i = 0; i <= ioCount; i++){
            IO:
            if(i == 0){ //for arguments before first pipe
                startIndex = 0;
                stopIndex = *ioRedirects.ptr[i] - 1;
            } else if(i > 0 && i <= ioCount - 1) { //for arguments after first pipe but before last pipe
                startIndex = *ioRedirects.ptr[i - 1] + 1;
                stopIndex = *ioRedirects.ptr[i] - 1;
            }else if(i == ioCount){ //for arguments after last pipe && *ioRedirects.ptr[i] == pipeNum
                startIndex = *ioRedirects.ptr[i - 1] + 1;
                stopIndex = parsedLine.size - 1;
            }

            if(arrIO[i][type] == singleLeftArrow){
                newArguments = NULL;
                savedArguments = copyArguments(parsedLine.ptr, newArguments, startIndex, stopIndex);
            }else {
                newArguments = (char**)realloc(newArguments, sizeof(char*) * newArgsSize);
                newArguments = copyArguments(parsedLine.ptr, newArguments, startIndex, stopIndex);
            }

            if(i < ioCount){ 
                //redirect outputhere store in variable. and send it through parseLine function 
                if(pipe(pipefd) == -1) { //create file descriptors for read and write sides of pipe
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
                if(jumpHappened == notJUMP){
                    if(arrIO[i][type] == singleLeftArrow){
                        jumpHappened = singleLeftArrow;
                        i++;
                        goto IO;
                    }
                }   
                if(jumpHappened == notJUMP){
                    /*pipefd[1]is the file descriptor for the write end of pipe. 
                    this will be passed as a parameter so execvp function redirects output to pipefd[1].
                    input for first execvp call will come from stdin. stdin file descriptor is 0. in = 0
                    */
                    executeCommand(newArguments, in, pipefd[1]); 
                    close(pipefd[1]); //child process will write
                    in = pipefd[0]; //assign in to a file descriptor number from pipe read side. this will redirect input for next call to execvp
                }else if(jumpHappened == singleLeftArrow){
                    readFileWriteToPipe(newArguments, pipefd[1], writeOver);
                    close(pipefd[1]); //child process will write 
                    in = pipefd[0]; //assign in to a file descriptor number from pipe read side. this will redirect input for next call to execvp
                    if(pipe(pipefd) == -1) { //create file descriptors for read and write sides of pipe
                        perror("pipe");
                        exit(EXIT_FAILURE);
                    }
                    executeCommand(savedArguments, in, pipefd[1]); //read from previous pipe but write to another
                    close(pipefd[1]); //child process will write 
                    in = pipefd[0]; //assign in to a file descriptor number from pipe read side. this will redirect input for next call to execvp
                    jumpHappened = notJUMP;
                }
                if(arrIO[i][type] == doubleRightArrow){ //if there is a double right arrow set jumpHappened, increment i and jump to postpone execute command
                    jumpHappened = doubleRightArrow;
                    i++;
                    goto IO;
                }else if(arrIO[i][type] == singleRightArrow){ //if there is a single right arrow set jumpHappened, increment i and jumpt to postpone execute command
                    jumpHappened = singleRightArrow;
                    i++;
                    goto IO;
                }else if(arrIO[i][type] == singleLeftArrow){ //if there is a single left arrow set jumpHappened, increment i and jumpt to postpone execute command
                    jumpHappened = singleLeftArrow;
                    i++;
                    goto IO;
                }    
            } else { //final call to executeCommand will come from here
                    if(in != 0 && jumpHappened == notJUMP){
                        dup2(in, 0); //copy stdin into in file descriptor
                        executeCommand(newArguments, in, 1);
                        dup2(saved_stdin, 0); //restore stdin to default
                        dup2(saved_stdout, 1); //restore stdout to default
                    }else if(jumpHappened == doubleRightArrow){
                        readWriteFile(newArguments, in, append);
                        jumpHappened = notJUMP;
                        dup2(saved_stdin, 0); //restore stdin to default
                        dup2(saved_stdout, 1); //restore stdout to default
                    }else if(jumpHappened == singleRightArrow){
                        readWriteFile(newArguments, in, writeOver);
                        jumpHappened = notJUMP;
                        dup2(saved_stdin, 0); //restore stdin to default
                        dup2(saved_stdout, 1); //restore stdout to default
                    }else if(jumpHappened == singleLeftArrow){
                        readFileWriteToPipe(newArguments, pipefd[1], writeOver);
                        close(pipefd[1]); //child process will write
                        in = pipefd[0]; //assign in to a file descriptor number from pipe read side. this will redirect input for next call to execvp
                        executeCommand(savedArguments, in, out);
                        jumpHappened = notJUMP;
                        dup2(saved_stdin, 0); //restore stdin to default
                        dup2(saved_stdout, 1); //restore stdout to default
                    }
            }

              
        }
    }
    newArguments = NULL;
    free(newArguments);
}

//reads from a file and writes to the pipe
int readFileWriteToPipe(char **newArguments, int out, const char filePermission[]){
    FILE *fptrR;
    FILE *fptrW;
    fptrR = fopen(*newArguments, "r"); //open file with read permission
    char ch;
    fptrW = fdopen(out, filePermission); //open write side of pipe with write permission
    while ((ch = fgetc(fptrR)) != EOF) { //write to file until end of file. Do not write EOF to file as char
        fprintf(fptrW, "%c", ch);
    }
    fclose(fptrW);
    fclose(fptrR);
}

//reads from pipe and writes to a file
int readWriteFile(char **newArguments, int in, const char filePermission[]){
    FILE *fptrR;
    FILE *fptrW;
    fptrR = fdopen(in, "r"); //open read side of pipe with read permission
    char ch;
    fptrW = fopen(*newArguments, filePermission); //open file with permission from argument
    while ((ch = fgetc(fptrR)) != EOF) { //write to file until end of file. Do not write EOF to file as char
        fprintf(fptrW, "%c", ch);
    }
    fclose(fptrW);
    fclose(fptrR);
}

//handles built in commands like cd and exit
int builtInCommands(char **parsedLine){
    
    //cd change directory
    if(strcmp(parsedLine[0], "cd") == 0){
        chdir(parsedLine[1]);
        setenv("PWD", "", 1);
    }

    //exit command
    if(strcmp(parsedLine[0], "exit") == 0){
        exit(0);
    }

    return(0);
}

//prints my header to the prompt
int printCommandLine(char *directory){
    //🐢
    green();
    printf("(");
    blue();
    green();
    printf("~half_shell");
    blue();
    printf("~%s", getenv("USER"));
    green();
    printf(")-[");
    white();
    printf("%s", directory);
    green();
    printf("]\n-$ "); //:turtle:
    resetColor();
}

//copies arguments of specified index into another pointer
char **copyArguments(char **parsedLine, char **newArguments, int startIndex, int stopIndex){
    int count = 0;
    //copy arguments between pipes
    for(int i = startIndex; i <= stopIndex; i++){
        if(newArguments== NULL){
            newArguments = (char**)realloc(newArguments, arrIOLength * sizeof(char**));
        }
        newArguments[count] = parsedLine[i];
        count++;
    }
    return newArguments;
}

//green color
void green(){
    printf("\033[1;32m");
}

//blue color
void blue(){
    printf("\033[1;34m");
}

//white color
void white(){
    printf("\033[1;37m");
}

//reset color
void resetColor(){
    printf("\033[0m");
}