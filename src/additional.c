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
extern int NumBGP;
extern ProcInfo *CurrentFGP;
extern int OGShell;

void Permissions(struct stat *File)
{
    printf(S_ISDIR(File->st_mode) ? "d" : "-");
    // Is the object a directory

    printf(File->st_mode & S_IRUSR ? "r" : "-");
    // User read permissions

    printf(File->st_mode & S_IWUSR ? "w" : "-");
    // User write permissions

    printf(File->st_mode & S_IXUSR ? "x" : "-");
    // User execute permissions

    printf(File->st_mode & S_IRGRP ? "r" : "-");
    // Group read permissions

    printf(File->st_mode & S_IWGRP ? "w" : "-");
    // Group write permissions

    printf(File->st_mode & S_IXGRP ? "x" : "-");
    // Group execute permissions

    printf(File->st_mode & S_IROTH ? "r" : "-");
    // Others read permissions

    printf(File->st_mode & S_IWOTH ? "w" : "-");
    // Others write permissions

    printf(File->st_mode & S_IXOTH ? "x" : "-");
    // Others execute permissions

    printf(" ");
    // Space to make it readable

    return;
}

void OtherFileDetails(struct stat File)
{
    char LastModified[BASE_LEN];
    printf("%ld ", File.st_nlink);
    // Number of links in the file/directory

    printf("%s ", getpwuid(File.st_uid)->pw_name);
    // Owner of the file/directory

    printf("%s ", getgrgid(File.st_gid)->gr_name);
    // Group owner of the file/directory

    printf("%ld ", File.st_size);
    // Size of the file/directory in bytes

    strftime(LastModified, sizeof(LastModified), "%b %d %H:%M", localtime(&File.st_mtime));
    printf("%s ", LastModified);
    // Last modified time of the file/directory

    return;
}

void InitDiscover(char *Input[])
{
    int NumArgs, NumFlags = 0, NumRest, TargetDirGiven = 0, FileNameGiven = 0;
    int f = 0, d = 0, i, a;
    char TargetDir[BASE_LEN] = "";
    char FileName[BASE_LEN] = "";

    // Count the number of arguments passed with discover
    for (NumArgs = 0; Input[NumArgs] != NULL; NumArgs++)
        ;

    // Check what flags have been given, if any
    for (i = 1; i < NumArgs; i++)
    {
        if (Input[i][0] == '-')
        {
            NumFlags++;
            if (strchr(Input[i], 'd') != NULL)
                d++;
            if (strchr(Input[i], 'f') != NULL)
                f++;
        }
    }

    // The remaining arguments are either files or directories (-1 as the 0th argument is the word discover itself)
    NumRest = NumArgs - NumFlags - 1;

    for (i = 0; i < NumArgs; i++)
    {
        // If the number of non-flag arguments is 2, that means we have both a directory and a file name
        if (NumRest == 2)
        {
            // If the current object being considered isnt the file name, the word discover itself or a flag, therefore it is a directory
            if (strchr(Input[i], '"') == NULL && Input[i][0] != '-' && i)
            {
                TargetDirGiven = 1;

                // If the direcory given is ~, we replace it with the home directory
                if (!strcmp(Input[i], "~"))
                    strcpy(TargetDir, HomeDir);

                // If the directory given is relative to ~, we make its path absolute
                else if (Input[i][0] == '~')
                {
                    strcpy(TargetDir, HomeDir);
                    strcat(TargetDir, Input[i] + 1);
                }
                else
                    strcpy(TargetDir, Input[i]);
            }

            // If the current object is the filename
            else if (strchr(Input[i], '"') != NULL)
            {
                FileNameGiven = 1;
                strcpy(FileName, Input[i] + 1);
                FileName[strlen(FileName) - 1] = '\0';
            }

            // Once we have set the directory path and the filename, we call discover
            if (TargetDirGiven && FileNameGiven)
            {
                Discover(TargetDir, FileName, d, f);
                break;
            }
        }

        // If the number of non-flag arguments is 1, we have either been given a directory or a file name
        else if (NumRest == 1 && i)
        {
            // If we have been given the directory
            if (strchr(Input[i], '"') == NULL && Input[i][0] != '-')
            {
                TargetDirGiven = 1;

                // If the direcory given is ~, we replace it with the home directory
                if (!strcmp(Input[i], "~"))
                    strcpy(TargetDir, HomeDir);

                // If the directory given is relative to ~, we make its path absolute
                else if (Input[i][0] == '~')
                {
                    strcpy(TargetDir, HomeDir);
                    strcat(TargetDir, Input[i] + 1);
                }
                else
                    strcpy(TargetDir, Input[i]);
            }

            // If we have been given the file name
            else if (strchr(Input[i], '"') != NULL)
            {
                FileNameGiven = 1;

                // No directory path means we set the taget directory as the current directory
                strcpy(TargetDir, ".");
                strcpy(FileName, Input[i] + 1);
                FileName[strlen(FileName) - 1] = '\0';
            }

            if (FileNameGiven)
            {
                Discover(TargetDir, FileName, d, f);
                break;
            }
            else
            {
                Discover(TargetDir, NULL, d, f);
                break;
            }
        }

        // If the number of non-flag arguments is 0, that means we havent been given a directory or a file. so the target directory is the current directory
        else if (NumRest == 0)
        {
            strcpy(TargetDir, ".");
            Discover(TargetDir, NULL, d, f);
            break;
        }
    }
    return;
}

void InitHistory(char Input[])
{
    FILE *TempHistory, *HistoryFile;
    char CommandsExecuted[BASE_LEN];
    char TempPath[BASE_LEN] = "";
    char HistFilePath[BASE_LEN] = "";
    char CompareDuplicate[BASE_LEN];
    int Error;

    // Set the path of a temp file as "temp.txt" in the home directory
    strcpy(TempPath, getenv("HOME"));
    strcat(TempPath, "/temp.txt");

    // Set the path of a temp file as "temp.txt" in the home directory
    strcpy(HistFilePath, getenv("HOME"));
    strcat(HistFilePath, "/history.txt");

    // Open both the temp and the history file
    TempHistory = fopen(TempPath, "w");
    HistoryFile = fopen(HistFilePath, "a+");

    if (TempHistory == NULL || HistoryFile == NULL)
    {
        printf("Error - history: File open error\n");
        return;
    }

    // Copy the most recent command executed into the temp file
    fputs(Input, TempHistory);
    fputs("\n", TempHistory);

    for (int i = 0; fgets(CommandsExecuted, 100, HistoryFile) != NULL && i < 19; i++)
    {
        if (!strcmp(CommandsExecuted, "\n"))
            continue;
        if (!i)
        {
            // We compare the most recent cammand with the one before it, and if they are the same we dont copy it into the file
            strcpy(CompareDuplicate, CommandsExecuted);
            CompareDuplicate[strlen(CompareDuplicate) - 1] = '\0';
            if (strcmp(CompareDuplicate, Input))
                fputs(CommandsExecuted, TempHistory);
        }
        else
            fputs(CommandsExecuted, TempHistory);
    }

    fclose(TempHistory);
    fclose(HistoryFile);

    // We then delete the file which contained the history till now (as we have copied all the required information into the temp file)
    // We then rename the temp file as the history file

    Error = remove(HistFilePath);
    if (Error)
    {
        printf("Error - history: File deletion error\n");
        return;
    }
    Error = rename(TempPath, HistFilePath);
    if (Error)
    {
        printf("Error - history: File renaming error\n");
        return;
    }
}

void InitBackgroundProcesses()
{
    // Initializing the array that will hold the names and PIDs of all the background processes
    // We set all the PIDs as -1 initially to show that the current index hasnt been used to store any information
    for (int i = 0; i < MaxNumBGP; i++)
    {
        BGProcesses[i] = (ProcInfo *)malloc(sizeof(ProcInfo));
        strcpy(BGProcesses[i]->name, "");
        BGProcesses[i]->PID = -1;
    }
}

void AddBGP(int PID, char Input[])
{
    // Adds the information of the background process at the first available location in the array (if there is space for more)
    if (NumBGP < MaxNumBGP)
    {
        printf("added\n");
        strcpy(BGProcesses[NumBGP]->name, Input);
        BGProcesses[NumBGP]->PID = PID;
        NumBGP++;
    }
    else
        printf("Error - Background Process: Too many processes\n");
    return;
}

void KillBGP()
{
    // Kills all the non - terminated background processes before exiting the shell to ensure we dont get any errors
    for (int i = 0; i < 10; i++)
    {
        if (BGProcesses[i]->PID != -1)
            kill(BGProcesses[i]->PID, SIGKILL);
    }
}

// Taken from online resource
handler *InitSigHandler(int SigInfo, handler *SigHandler)
{
    int Error;
    struct sigaction ResultantAction, InputAction;

    memset(&ResultantAction, 0, sizeof(struct sigaction));
    ResultantAction.sa_sigaction = SigHandler;
    sigemptyset(&ResultantAction.sa_mask);

    ResultantAction.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SigInfo, &ResultantAction, &InputAction);

    Error = sigaction(SigInfo, &ResultantAction, &InputAction);

    if (Error == -1)
        write(1, "Error - Signal Handler: Initialization error\n", strlen("Error - Signal Handler: Initialization error\n"));
    return (InputAction.sa_sigaction);
}

void ControlC(int Sig, siginfo_t *Info, void *Pointer)
{
    // Since Ctrl+C just exits the current foreground process, we just return
    return;
}

void ControlZ(int Sig, siginfo_t *Info, void *Pointer)
{
    printf("reached here\n");

    int PID = getpid();
    // Compare the PID of the current process to the PID of the shell itself, if not equal then return
    if (PID != OGShell)
        return;

    // CurrentFGP holds the information about the running foreground process, so if its -1 that means there is no current foreground process
    if (CurrentFGP->PID == -1)
        return;

    printf("reached here\n");

    // Set the Group PID of the current process to 0 to make it a background process
    setpgid(CurrentFGP->PID, 0);

    // Send the SIGTSTP signal to stop the process
    int Error = kill(CurrentFGP->PID, SIGTSTP);
    if (Error == -1)
    {
        printf("Error - Ctrl+Z: Couldn't stop the process\n");
        return;
    }

    // Add the process to the array that holds the information about all the background processes as it is now a background process
    AddBGP(CurrentFGP->PID, CurrentFGP->name);
    return;
}