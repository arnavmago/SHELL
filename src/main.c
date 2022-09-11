#include "headers.h"
#include "allFunctions.h"
#include "parsing.h"

char CommandPrompt[BASE_LEN];
char HomeDir[BASE_LEN];
char CurrentDir[BASE_LEN];
char Input[BASE_LEN];
ProcInfo *BGProcesses[1000];

int main()
{
    InitSigHandler(SIGCHLD, ChildHandler);
    // Setting up the signal handling functionality which will give us the exit information of a background process

    InitBackgroundProcesses();
    //  Initializing the data structure that will hold the names and PIDs of all background processes

    PrintCommandPrompt(1, CommandPrompt, HomeDir);
    // Setting the directory that the shell is invoked in as the home directory

    fputs("\033c", stdout);
    // Clearing the terminal (looks nice)

    while (1)
    {
        printf("\r%s", CommandPrompt);
        fgets(Input, 1000, stdin);
        // Taking in input form the user via the terminal
        if (strcmp(Input, "\n"))
        {
            UnderstandInput(Input);
        }
        PrintCommandPrompt(0, CommandPrompt, HomeDir);
        // Re-evaluating the command prompt in case there need to be any changes (ex - cd, foreground process that took longer than 1 second)
    }
}