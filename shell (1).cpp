/// SHELL PROGRAM 
/// AUTHOR : HANNAH JEFFI
#include <iostream>    // cout, cerr
#include <queue>       // stl queue
#include <sys/types.h> // fork, wait
#include <sys/wait.h>  // wait
#include <unistd.h>    // fork
#include <stdlib.h>    // for exit
#include <string.h>
#include <fcntl.h>
#define MAX_LINE 80 /* The maximum length command */

using namespace std;

/// @brief The method extracts the input from the user input, checks for pipe flags and update it
/// @param input INPUT ARRAY
/// @param args COMMAND ARRAY
/// @param history TO STORE PREVIOUS COMMAND
/// @param argslength LENGTH OF COMMAND ARRAY
/// @param pipeFlag PIPE DENOTING THE POSITION OF PIPE IF PRESENT
/// @return  state - 0 for exit ; state - 1 for execute ; state 2 for continue to loop in case of no commands in history;
int getUserInput(char input[], char *args[], char history[], int *argslength, int *pipeFlag)
{
    cin.getline(input, MAX_LINE);
    if ((strcmp(input, "exit") == 0))
    {
        return 0;
    }
    else
    {
        if (strcmp(input, "!!") == 0)
        {
            if (strcmp(history, " ") == 0)
            {
                cout << "No commands in history." << endl;
                return 2;
            }
            else
            {
                cout << history << endl;
                strcpy(input, history);
            }
        }
    }
    // parse the input by splitting the words
    char *pch;
    int position = 0;
    pch = strtok(input, " ");
    while (pch != nullptr)
    {
        args[position++] = pch;
        pch = strtok(NULL, " ");
        if (strcmp(args[position - 1], "|") == 0)
        {
            *pipeFlag = position - 1;
        }
    }
    args[position] = pch;
    // keep track of previous command
    if (strlen(input) > 0)
    {
        strcpy(history, input);
    }
    *argslength = position;
    return 1;
}

/// @brief The method executes the user for all input except involving pipe command
/// @param args char array contaning the command
/// @param argslength length of the command
void executeCommand(char *args[], int *argslength)
{
    int waitflag = 1;
    if (strcmp(args[*(argslength)-1], "&") == 0)
    {
        waitflag = 0;
        args[*(argslength)-1] = nullptr;
        *argslength--;
    }

    int pid = fork();

    if (pid == 0)
    {
        // REDIRECTION
        if (*argslength > 2)
        {
            if (strcmp(args[*argslength - 2], ">") == 0)
            {
                args[*argslength - 2] = NULL;
                int fileDescOut = open(args[*argslength - 1], O_WRONLY | O_TRUNC | O_CREAT, S_IRWXG | S_IRWXU);
                dup2(fileDescOut, 1);
                close(fileDescOut);
            }
            else if (strcmp(args[*argslength - 2], "<") == 0)
            {
                args[*argslength - 2] = NULL;
                int fileDescIn = open(args[*argslength - 1], O_RDONLY);
                dup2(fileDescIn, 0);
                close(fileDescIn);
            }
        }

        if (execvp(args[0], args) == -1)
        {
            cout << "Command not found" << endl;
        }
        exit(0);
    }
    else if (pid < 0)
    {
        perror("Error during fork");
        exit(EXIT_FAILURE);
    }

    else
    {
        if (waitflag == 1)
        {
            wait(NULL);
        }
    }
}

/// @brief THE METHOD EXECUTES THE COMMAND ON EITHER SIDE OF THE PIPE AND PASS FIRST'S OUTPUT AS INPUT TO ANOTHER USING FILE DESCRIPTOR
/// @param args COMMAND
/// @param argslength LENGTH OF COMMAND
/// @param pipeFlag - PIPE FLAG POSITION
void executePipeCommands(char *args[], int *argslength, int pipeFlag)
{
    int pipe_pid;                  // PID OF FIRST FORK
    int pipe_pid2;                 // PID OF SECOND FORK
    int pipeFD[2];                 // FILE DESCRIPTOR
    char *args2[MAX_LINE / 2 + 1]; // SECOND COMMAND ARRAY
    int argsPipePos = 0;

    //  CHECKING FOR &
    int waitflag = 1;
    if (strcmp(args[*(argslength)-1], "&") == 0)
    {
        waitflag = 0;
        args[*(argslength)-1] = nullptr;
        *argslength--;
    }

    // Splitting the command
    for (int i = pipeFlag + 1; i < *argslength; i++)
    {
        args2[argsPipePos++] = args[i];
        args[i] = NULL;
    }
    args2[argsPipePos] = NULL;
    args[pipeFlag] = NULL;

    // CHECK FOR PIPE ERROR
    if (pipe(pipeFD) < 0)
    {
        perror("Error in creating pipe");
        exit(EXIT_FAILURE);
    }

    // Fork for process;
    pipe_pid = fork();
    if (pipe_pid < 0)
    {
        perror("Error during fork");
        exit(EXIT_FAILURE);
    }
    else if (pipe_pid == 0)
    {
        // WRITE SIDE OF THE PIPE
        close(pipeFD[0]);
        dup2(pipeFD[1], 1);
        close(pipeFD[0]);
        if (execvp(args[0], args) == -1)
        {
            cout << "Command not found2" << endl;
        }
        exit(0);
    }

    // SHELL PROCESS
    else
    {
        // CREATE SECOND PROCESS
        pipe_pid2 = fork();
        if (pipe_pid < 0)
        {
            perror("Error during fork");
            exit(EXIT_FAILURE);
        }
        else if (pipe_pid2 == 0)
        {
            // READ SIDE OF THE PIPE
            close(pipeFD[1]);
            dup2(pipeFD[0], 0);
            close(pipeFD[0]);
            if (execvp(args2[0], args2) == -1)
            {
                cout << "Command not found" << endl;
            }
            exit(1);
        }
        else
        {
            close(pipeFD[0]);
            close(pipeFD[1]);
            waitpid(pipe_pid2, NULL, 0);
            //  wait(NULL);
        }
        if (waitflag == 1)
        {
            waitpid(pipe_pid, NULL, 0);
        }
        return;
    }
}

int main()
{
    char *args[MAX_LINE / 2 + 1]; /* command line arguments */
    int argslength = 0;           // LENGTH OF COMMAND ARRAY
    int should_run = 1;           /* flag to determine when to exit program */
    int pipeFlag = 0;             // FLAG DENOTING THE PRECENSE OF PIPE
    char input[MAX_LINE];         // INPUT ARRAY
    char history[MAX_LINE] = " "; // ARRAY TO STORE HISTORY
    while (should_run)
    {
        argslength = 0;
        pipeFlag = 0;
        printf("osh>");
        fflush(stdout);
        // istream is;
        int state = getUserInput(input, args, history, &argslength, &pipeFlag);
        if (state == 1)
        {
            if (pipeFlag > 0)
            {
                executePipeCommands(args, &argslength, pipeFlag);
            }
            else
            {
                executeCommand(args, &argslength);
            }
        }
        else if (state == 0)
        {
            should_run = 0;
        }
        else
        {
            continue;
        }
    }

    return 0;
}