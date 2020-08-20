/* authors names: Tarek Ramadan and Hadas Libman
* course name and number: Operating Systems 4302
* assignment name/number: A Simple Shell
* instructor’s name: Dr. Bo Sun
* due date: December 2nd, 2019
* 
* design and implement a simple, interacitve shell program that prompts the user for a command,a
* parses the command, and then execute it with a child process.
* the skeleton for the algorithm to solve the probelm was taken from the book used in the course
* operational requirements: C language, in the solution we are required to use execv() instead of exevp(),
* which means we will have to read the PATH enviornment variable, then search each directory in the PATH
* for the command line file name that appears on the command line */

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

/* from the book */
#define LINE_LEN   80
#define MAX_ARGS  64
#define MAX_ARGS_LEN  16
#define MAX_PATHS  64
#define  MAX_PATH_LEN  96
#define WHITESPACE " .,\t\n"

#define clear() printf("\033[H\033[J") //clearing the shell

/* from the book */
#ifndef NULL
#define NULL
#endif

/* data structure to save commands */
struct command_t
{
    char *name;
    int argc;
    char *argv[MAX_ARGS];
};

/* this function identified by the dir
* argument to see if argv[0] (the file name) appears there
* allocate a new string, place the full path name in it, then reutrn the string
*/
char * lookupPath(char **argv, char **dir)
{
    char *result;
    char pName[MAX_PATH_LEN];
    char *temp = (char *) malloc(MAX_PATH_LEN); 
    memset(temp, 0, MAX_PATH_LEN);  

    //if true then it is absolute path name
    if(*argv[0]=='/')
    {
        result = argv[0];
        return result;
    }
    int j=0;

    //look up path in directories
    for(j=0;j<MAX_PATHS;j++)
    {
        if (dir[j] == NULL)
         continue;
    strcpy(temp, dir[j]); 
    strncat(temp, "/", 1);
    strncat(temp, argv[0], strlen(argv[0])); //to make sure that we return the string in a good format 

    // use access to check if the file is in a dir
    if(access(temp,F_OK)==0){
        result = temp;
        return result;}
    }

    //file name not found in any path
    fprintf(stderr, "%s: command not found \n", argv[0]);
    return NULL;
}

/* argc is the number of "tokens" or words on the command line
* argv[] is an array of strings (pointers to char *)
* the last element in argv[] must be NULL. as we scan the command line from the left, the first token goes
* in argv[0], the second in argv[1] and so on. each time we add a token to argv[], we increment argc.
*/
int parseCommand(char *cLine, struct command_t *cmd)
{
    int argc;
    char **clPtr;
    /* initialization */
    clPtr = &cLine; // cLine is the command line
    argc = 0;
    cmd->argv[argc] = (char *) malloc(MAX_ARGS_LEN);

    // fill argv[]
    while((cmd->argv[argc] = strsep(clPtr,WHITESPACE)) !=NULL) {
        cmd->argv[++argc] = (char *) malloc(MAX_ARGS_LEN);
    }

    // set the command name and argc
    cmd->argc = argc-1;
    cmd->name = (char *) malloc(sizeof(cmd->argv[0]));
    strcpy(cmd->name, cmd->argv[0]);
    return 1;
}

/* this function reads the PATH variable for this environment, then builds an array, dirs[],
* of the directories in PATH */
int parsePath(char *dirs[])
{
    char *pathEnvVar;
    char *thePath;

    int i=0;
    for(i=0; i<MAX_PATHS; i++)
        dirs[i] = NULL;
    pathEnvVar = (char *) getenv("PATH");
    thePath = (char *) malloc(strlen(pathEnvVar) + 1);
    strcpy(thePath, pathEnvVar);

    char* token = strtok(thePath, ":"); //strtok splits a string by some delimiter
    i=0; 
    
    //loops until it reaches the end token while is null 
    while(token != NULL)
    {
        //malloc is a function returns a pointer to the allocated memory, or NULL if the request fails.
        dirs[i] = malloc(strlen(token)+1);
        //check if request fails
        if(dirs[i] == NULL)
            break;
        //parsing the path
        strcpy(dirs[i],token);
        token=strtok(NULL,":");
        i++;
    }
}

/* build the prompt string to have the machine name,
* current directory, or other desired information */
void printPrompt(int x)
{
    if(x==1) //to print welcome message only once at the start
    {
        printf("\n\t***************************");
        printf("\n\t****Welcome To My Shell****\n");
        printf("\t***************************\n");

    }

    printf("\nPlease enter a command or enter exit to terminate");
    char *promptString= getenv("USER"); //get username 
    printf("\n[%s]$",promptString);
    sleep(1);
}

/* read an entire command line into the buffer */
void readCommand(char *buffer) {
    gets(buffer);
}

int main()
{
    int firstscreen=0;
    char *pathv[MAX_PATHS];
    char line[LINE_LEN];
    int child;
    parsePath(pathv); //get directory paths from PATH
    struct command_t command;

    while(1){
        firstscreen++;
        printPrompt(firstscreen);
        readCommand(line); //reads the command line

        if(strcmp(line,"exit")==0){ 
            exit(0);
        }
        if(strlen(line)>0){
            parseCommand(line,&command); //parse the command
            command.name=lookupPath(command.argv,pathv); //get the full path name for the file
        
            if(command.name == NULL){
                printf("Invalid command");
                continue;
            }
            else {
                child=fork();

        //child process execution 
        if(child==0){ 
            execv(command.name,command.argv);
            exit(0);
        }
        //parent process waits for the child to terminate 
        else { 
            wait(NULL);
            }
            }
        }
    }
return 0;
}
