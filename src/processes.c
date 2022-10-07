#include "headers.h"
#include "allFunctions.h"
#include "parsing.h"

// All variables defined in other files that are needed here
extern char CommandPrompt[];
extern char HomeDir[];
extern char CurrentDir[];
extern char Input[];
extern char HomeFolder[];
extern ProcInfo *BGProcesses[];
extern ProcInfo *CurrentFGP;

// Defined new variables that are needed
struct timeval TimeTaken;
double FGPStart, FGPEnd, TimeFlag = 0;
int NumBGP = 0;

void ForegroundProcess(char *Input[])
{
    char Process[BASE_LEN] = "";
    int Error, Status, WaitStatus;

    // Get the time for when we begin the process execution
    gettimeofday(&TimeTaken, NULL);
    FGPStart = TimeTaken.tv_sec * 1000000 + TimeTaken.tv_usec;

    // If the executable command given is relative to ~, we make it absolute
    if (Input[0][0] == '~')
    {
        strcpy(Process, HomeDir);
        strcat(Process, "/");
        Input[0] += 2;
        strcat(Process, Input[0]);
    }
    strcpy(Process, Input[0]);

    int PID = fork();

    // If PID is -1, an error has occurred in the forking
    if (PID == -1)
    {
        printf("Error - Foreground Process: Fork error\n");
        return;
    }

    // If the current process is the child process
    if (PID == 0)
    {
        // Execute the command given, error handling if the command is invalid
        Error = execvp(Process, Input);
        if (Error == -1)
            printf("Error - Foreground Process: Command execution failed\n");
        exit(1);
    }

    // If the current process is the parent process
    else
    {
        strcpy(CurrentFGP->name, Process);
        CurrentFGP->PID = PID;

        // Wait for the child process to terminate
        waitpid(PID, &Status, WUNTRACED);

        // Get the time for when the process execution ends
        gettimeofday(&TimeTaken, NULL);

        strcpy(CurrentFGP->name, "");
        CurrentFGP->PID = -1;

        // Using the 2 times, we calculate how long the process took
        FGPEnd = TimeTaken.tv_sec * 1000000 + TimeTaken.tv_usec;
        TimeFlag = (double)(FGPEnd - FGPStart) / 1000000;
    }
}

void BackgroundProcess(char *Input[])
{
    char Process[BASE_LEN] = "";
    int Error, Status, a = 0;

    // If the executable command given is relative to ~, we make it absolute
    if (Input[0][0] == '~')
    {
        strcpy(Process, HomeDir);
        strcat(Process, "/");
        Input[0] += 2;
        strcat(Process, Input[0]);
    }
    else
        strcpy(Process, Input[0]);

    // If the current number of background processes is the maximum we can handle, we print an error
    if (NumBGP == MaxNumBGP)
    {
        printf("Error - Background Process: Too may processes\n");
        return;
    }

    int PID = fork();

    // If PID is -1, an error has occurred in the forking
    if (PID == -1)
    {
        printf("Error - Background Process: Fork error\n");
        return;
    }

    // If the current process is the child process
    if (PID == 0)
    {
        setpgid(0, 0);
        Error = execvp(Process, Input);
        if (Error == -1)
            printf("Error - Background Process: Command execution failed\n");
        exit(1);
    }

    // If the current process is the parent process
    else
    {
        // Add the name and PID of the process to the array
        AddBGP(PID, Process);
        printf("[%d] %d\n", NumBGP, PID);
    }
    return;
}

void ChildHandler(int Sig, siginfo_t *Info, void *Pointer)
{
    int Status, PID, Check = 0, ExitStatus;
    char buffer[BIG_LEN];

    printf("came here too\n");

    // Look for any background process that just ended
    while ((PID = waitpid(-1, &Status, WNOHANG | WUNTRACED)) > 0)
    {
        for (int i = 0; i < NumBGP; i++)
        {
            // Compare the PID of the process that ended to the processes stored in the array till we find which it was
            if (BGProcesses[i]->PID == PID)
            {
                // If we find the process, we check its exit status
                Check = 1;
                ExitStatus = WIFEXITED(Status);
                if (ExitStatus == 0)
                    sprintf(buffer, "\n%s with pid [%d] exited abnormally\n", BGProcesses[i]->name, BGProcesses[i]->PID);
                else
                    sprintf(buffer, "\n%s with pid [%d] exited normally\n", BGProcesses[i]->name, BGProcesses[i]->PID);
                write(1, buffer, strlen(buffer));

                // We also check if it has terminated or if it has just been stopped
                if (!WIFSTOPPED(Status))
                    BGProcesses[i]->PID = -1;
                break;
            }
        }
    }

    // if we were able to find the process in the array, after checking its exit status we re-print the command prompt (normally you would have to hit enter to get the prompt to re-print, this looks nicer)
    if (Check)
    {
        PrintCommandPrompt(0, CommandPrompt, HomeDir);
        write(1, "\r", strlen("\r"));
        write(1, CommandPrompt, strlen(CommandPrompt));
    }
}

void fg(char *Input[])
{
    int Status;
    int CurrentProcess = getpid();

    // If we have been given more than 1 argument (excluding the word fg itself), it is an error
    if (Input[2] != NULL)
    {
        printf("Error - fg: Too many arguments\n");
        return;
    }

    // If the job number given is invalid
    if (atoi(Input[1]) > NumBGP)
    {
        printf("Error - fg: Process doesn't exist\n");
        return;
    }

    // Convert the job number argument to integer and subtract 1 to account for 0-indexing
    int ProcessNum = atoi(Input[1]) - 1;

    int ProcessPID = BGProcesses[ProcessNum]->PID;

    if (ProcessPID == -1)
    {
        printf("Error - fg: invalid process\n");
        return;
    }

    char ProcessName[BASE_LEN];
    strcpy(ProcessName, BGProcesses[ProcessNum]->name);

    // Ignoring the signals that are sent when trying to read or write from terminal
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    // Set the process group ID the same as stdout
    int Error = tcsetpgrp(STDIN_FILENO, getpgid(ProcessPID));

    if (Error)
    {
        // If we have an error, we set the signal handlers back to default
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        printf("Error - fg: Process foregrounding failed\n");
        return;
    }

    // Send the process the signal to continue execution
    Error = kill(ProcessPID, SIGCONT);

    if (Error)
    {
        // If we have an error, we set the signal handlers back to default
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        printf("Error - fg: Process foregrounding failed\n");
        return;
    }

    // Setting the values for the current foreground process in the event we encounter ctrl+Z
    CurrentFGP->PID = ProcessPID;
    strcpy(CurrentFGP->name, ProcessName);

    // Wait for the process to finish execution as it is now a foreground process
    waitpid(ProcessPID, &Status, WUNTRACED);

    // Set the vslues back to default
    strcpy(CurrentFGP->name, "");
    CurrentFGP->PID = -1;

    Error = tcsetpgrp(STDIN_FILENO, CurrentProcess);
    if (Error)
    {
        printf("Error - fg: Fatal command foregrounding failure\n");
        ExitFunction();
    }

    // If we have no errors, we set the signal handlers back to default
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);

    if (WIFEXITED(Status))
    {
        BGProcesses[ProcessNum]->PID = -1;
        strcpy(BGProcesses[ProcessNum]->name, "");
    }
    return;
}

void bg(char *Input[])
{
    // If we have been given more than 1 argument (excluding the word bg itself), it is an error
    if (Input[2] != NULL)
    {
        printf("Error - fg: Too many arguments\n");
        return;
    }

    // If the job number given is invalid
    if (atoi(Input[1]) > NumBGP)
    {
        printf("Error - fg: Process doesn't exist\n");
        return;
    }

    int ProcessPID = BGProcesses[atoi(Input[1]) - 1]->PID;

    if (ProcessPID == -1)
    {
        printf("Error - bg: invalid process\n");
        return;
    }

    // We send the the background process the signal to continue execution
    int Error = kill(ProcessPID, SIGCONT);

    if (Error == -1)
    {
        printf("Error - bg: Continuation of background process error\n");
    }
    return;
}