#include "headers.h"
#include "allFunctions.h"
#include "parsing.h"

// All variables defined in other files that are needed here
extern char CommandPrompt[];
extern char HomeDir[];
extern char CurrentDir[];
extern char Input[];
extern char HomeFolder[];
extern int NumBGP;
extern ProcInfo *BGProcesses[];

// Defined new variables that are needed
char PrevDir[BASE_LEN];
char DirRN[BASE_LEN];

void cd(char *Input[])
{
    // If we have more than 1 argument passed for cd, it is invalid
    if (Input[2] != NULL)
    {
        printf("Error - cd: too many arguments\n");
        return;
    }

    char *Argument = Input[1];
    int Error = 0;

    // Storing the current directory as the previous directory before we change it (for the "cd -" command)
    strcpy(PrevDir, CurrentDir);
    getcwd(CurrentDir, 1000);

    // If we havent been given any arguments for cd, we change  to the home directory
    if (Argument == NULL)
    {
        chdir(HomeDir);
    }

    // If we have been given ~ as the argument, then we change to the home directory
    else if (!strcmp(Argument, "~"))
    {
        chdir(HomeDir);
    }

    // If we have been given - as the argument, then we change to the previous directory
    else if (!strcmp(Argument, "-"))
    {
        printf("%s\n", PrevDir);
        chdir(PrevDir);
    }
    else
    {
        // If the argument is relative to the home directory, we make the path absolute
        if (Argument[0] == '~')
        {
            chdir(HomeDir);
            Argument += 2;
            Error = chdir(Argument);
            if (Error != 0)
                chdir(CurrentDir);
        }

        // If the argument is .. while we are in the home directory, we print the absolute path of the parent directory
        else if (!strcmp(Argument, "..") && !strcmp(CurrentDir, HomeDir))
        {
            chdir(Argument);
            getcwd(DirRN, 1000);
            printf("%s\n", DirRN);
        }
        else
            Error = chdir(Argument);
    }

    // If the directory we tried to change into gives an error, we reset the value of the current directory and previous directory
    if (Error != 0)
    {
        if (errno == ENOTDIR)
            printf("Error - cd: Not a directory\n");
        else
            printf("Error - cd: No such file or directory\n");
        strcpy(CurrentDir, PrevDir);
    }

    return;
}

void pwd()
{
    // Get and print the the absolute path of the current directory
    char CurrDir[BASE_LEN];
    getcwd(CurrDir, 1000);
    printf("%s\n", CurrDir);
    return;
}

void echo(char *Input[])
{
    // Print to the terminal the parsed version of whatever is passed as an argument to echo
    for (int a = 1; Input[a] != NULL; a++)
    {
        if (a == 1)
            printf("%s", Input[a]);
        else
            printf(" %s", Input[a]);
    }

    // To ensure that a blank newline isnt printed when no argument is passed to echo
    if (Input[1] != NULL)
        printf("\n");
    return;
}

void ls(char *Input[])
{
    int NumArgs, NumFlags = 0, NumRest, NumBlocks = 0, HiddenFile, NewlineCount, NumFiles;
    int i, a = 0, l = 0, BeenHere = 0;
    char DirPath[BASE_LEN] = "", FilePath[BASE_LEN] = "";

    // Count the number of arguments passed with ls
    for (NumArgs = 0; Input[NumArgs] != NULL; NumArgs++)
        ;

    // Check what flags have been given, if any
    for (i = 1; i < NumArgs; i++)
    {
        if (Input[i][0] == '-')
        {
            NumFlags++;
            if (strchr(Input[i], 'a') != NULL)
                a++;
            if (strchr(Input[i], 'l') != NULL)
                l++;
        }
    }

    // The remaining arguments are either files or directories (-1 as the 0th argument is the word ls itself)
    NumRest = NumArgs - NumFlags - 1;

    // The number of new lines that are to be printed is set to the number of files/directories
    NewlineCount = NumRest;

    // If the number of files/directories is 0, then the ls information required is of the current directory
    if (!NumRest)
        Input[0] = ".";

    for (i = 0; i < NumArgs; i++)
    {
        // If the current word isnt a flag or the word ls itself (therefore is a file or directory)
        if (Input[i][0] == '-' || !strcmp(Input[i], "ls"))
            continue;
        else
        {
            // If the directory given is ~, then we replace it with the absolute path of the home directory
            if (!strcmp(Input[i], "~"))
                strcpy(DirPath, HomeDir);

            // If the dirctory given is relative to ~, we replace it with the absolute path
            else if (Input[i][0] == '~')
            {
                strcpy(DirPath, HomeDir);
                strcat(DirPath, ++Input[i]);
            }
            else
                strcpy(DirPath, Input[i]);

            DIR *DirInfo;
            DirInfo = opendir(DirPath);

            // If the path given is not a directory, it can be one of 2 things. either its a file or its an invalid directory
            if (DirInfo == NULL)
            {
                // If the given path is a file
                if (errno == ENOTDIR)
                {
                    struct stat FileInfo;
                    if (stat(DirPath, &FileInfo))
                    {
                        printf("Error - ls: File Error\n");
                        closedir(DirInfo);
                        return;
                    }

                    // If the -l flag hasnt been given, we just print the names of the files
                    if (!l)
                    {
                        if (FileInfo.st_mode & S_IEXEC)
                            printf("\033[0;31m");
                        printf("%s\n", DirPath);
                        printf("\033[0m");
                        // BeenHere is a count that tells us the number of files that have been passed as arguments for ls
                        BeenHere++;
                        continue;
                    }

                    // If the -l flag has been given
                    else
                    {
                        Permissions(&FileInfo);
                        OtherFileDetails(FileInfo);
                        if (FileInfo.st_mode & S_IEXEC)
                            printf("\033[0;31m");
                        printf("%s\n", DirPath);
                        printf("\033[0m");
                        BeenHere++;
                        continue;
                    }
                }

                // If the given path is an invalid path
                else
                    printf("Error - ls: No such file or directory\n");

                closedir(DirInfo);
                return;
            }

            closedir(DirInfo);
            // If the number of files/directories and newline count is > 1 and there is atleast 1 file given as an argument for ls
            if (NumRest > 1 && NewlineCount > 1 && BeenHere > 0)
            {
                printf("\n");
                NewlineCount -= BeenHere;
            }

            // If the number of files/directories is greater than 1, we print the the name of the directories
            if (NumRest > 1)
                printf("%s:\n", DirPath);

            struct dirent **DirFiles;
            NumFiles = scandir(DirPath, &DirFiles, NULL, NULL);
            // Scan all the files in the directory given by the path and store them in DirFiles with the number in Numfiles

            // If the number of files in the directory is negative, there is an error
            if (NumFiles < 0)
            {
                printf("Error - ls: Directory errorr\n");
                return;
            }

            // We calculate the total number of 512B blocks allocated if the -l flag is given
            if (l)
            {
                for (int i = 0; i < NumFiles; i++)
                {
                    struct stat FileInfo;
                    strcpy(FilePath, DirPath);
                    strcat(FilePath, "/");
                    strcat(FilePath, DirFiles[i]->d_name);
                    // Using the name of the file/directory we make its path absolute and check if its valid
                    if (stat(FilePath, &FileInfo))
                    {
                        printf("Error - ls: File Error\n");
                        return;
                    }
                    NumBlocks += FileInfo.st_blocks;
                }
                NumBlocks /= 2;
            }

            NumFiles = scandir(DirPath, &DirFiles, NULL, alphasort);
            // Scan all the files in the directory given by the path and store them in DirFiles with the number in Numfiles
            // We also implement alphasort to ensure the files are in alphabtical order

            // If the number of files in the directory is negative, there is an error
            if (NumFiles < 0)
            {
                printf("Error - ls: Directory errorr\n");
                return;
            }

            // If the -l flag is gien we print the number of 512B blocks allocated
            if (l)
                printf("total %d\n", NumBlocks);

            for (int i = 0; i < NumFiles; i++)
            {
                struct stat FileInfo;
                strcpy(FilePath, DirPath);
                strcat(FilePath, "/");
                strcat(FilePath, DirFiles[i]->d_name);
                // Using the name of the file/directory we make its path absolute and check if its valid
                if (stat(FilePath, &FileInfo))
                {
                    printf("Error - ls: File Error\n");
                    return;
                }

                // If the -l flag isnt given we just print the names of all the files and directories
                if (!l)
                {
                    HiddenFile = DirFiles[i]->d_name[0] == '.' ? 1 : 0;

                    // We print the names of the hidden files only if we're given the -a flag
                    if (a && HiddenFile || !HiddenFile)
                    {
                        // If the argument is a directory, print is name in green
                        if (S_ISDIR(FileInfo.st_mode))
                            printf("\033[0;32m");

                        // If the argument is executable, print its name in red
                        else if (FileInfo.st_mode & S_IEXEC)
                            printf("\033[0;31m");

                        // If the argument is a regular file, print its name in the default color (white)
                        printf("%s\n", DirFiles[i]->d_name);

                        // Set the color of printing back to the default (in case it was changed)
                        printf("\033[0m");
                    }
                }

                // If we are given the -l flag, we print some the details for the files/directories
                else
                {
                    // We print the details of the hidden files only if we're given the -a flag
                    HiddenFile = DirFiles[i]->d_name[0] == '.' ? 1 : 0;
                    if (a && HiddenFile || !HiddenFile)
                    {
                        Permissions(&FileInfo);
                        OtherFileDetails(FileInfo);
                        // If the argument is a directory, print is name in green
                        if (S_ISDIR(FileInfo.st_mode))
                            printf("\033[0;32m");

                        // If the argument is executable, print its name in red
                        else if (FileInfo.st_mode & S_IEXEC)
                            printf("\033[0;31m");

                        // If the argument is a regular file, print its name in the default color (white)
                        printf("%s\n", DirFiles[i]->d_name);

                        // Set the color of printing back to the default (in case it was changed)
                        printf("\033[0m");
                    }
                }
            }

            if (NumRest > 1 && NewlineCount > 1)
            {
                printf("\n");
                NewlineCount--;
            }

            //  Free the memory that was allocated by scandir
            for (int i = 0; i < NumFiles; i++)
                free(DirFiles[i]);
            free(DirFiles);
        }
    }
    return;
}

void pinfo(char *Input[])
{
    char ProcessPath[BASE_LEN];
    FILE *ProcessInfo;
    char ProcessDetails[BASE_LEN];
    char ProcessExecDetails[BASE_LEN];
    char *ProcessDetailsParsed[BASE_LEN] = {NULL};
    char ExecutablePath[BASE_LEN] = {0};
    int PID, ConsolePID = tcgetpgrp(STDOUT_FILENO);
    // We get the PID of the terminal
    int a = 0, i = 0, FGflag = 0;

    // If no argument has been passed for pinfo it means we print the process information of the curent process
    if (Input[1] == NULL)
        PID = getpid();

    // If we have been given an argument then we convert it to int as store it as the PID for the process whose information we print
    else
        PID = atoi(Input[1]);

    // We get group PID
    int GrpPID = getpgid(PID);

    // If the PID for the terminal and the group is the same, then its a foreground process
    if (GrpPID == ConsolePID)
        FGflag = 1;

    // Setting the path to read from proc/pid/stat file
    sprintf(ProcessPath, "/proc/%d/stat", (int)PID);

    // Setting the path to read from proc/pid/exe file
    sprintf(ProcessExecDetails, "/proc/%d/exe", (int)PID);

    // We check if a process has an executable path (if it doesnt its a zombie process)
    int HasExecutePath = readlink(ProcessExecDetails, ExecutablePath, 100);

    // We make the executable path of the file relative to the home directory
    CheckRelative(HomeDir, ExecutablePath);

    // Open the proc/pid/stat file and read its information into a string
    ProcessInfo = fopen(ProcessPath, "r");
    if (ProcessInfo == NULL)
    {
        printf("Error - pinfo: File error\n");
        return;
    }
    fread(ProcessDetails, BIG_LEN, 1, ProcessInfo);
    fclose(ProcessInfo);

    // Split the read information on spaces to get inidividual data points as individual words
    ProcessDetailsParsed[a] = strtok(ProcessDetails, " ");
    while (ProcessDetailsParsed[a] != NULL)
    {
        a++;
        ProcessDetailsParsed[a] = strtok(NULL, " ");
    }

    // The 1st datapoint is the PID, the 3rd is the process status and the 23rd is the memory size

    long long int ProcMem = atoll(ProcessDetailsParsed[22]);
    printf("pid: %s\n", ProcessDetailsParsed[0]);
    if (FGflag)
        strcat(ProcessDetailsParsed[2], "+");
    printf("process status: %s\n", ProcessDetailsParsed[2]);
    printf("memory: %lld B {virtual memory}\n", ProcMem);
    if (HasExecutePath == -1)
        printf("executable path: N/A\n");
    else
        printf("executable path: %s\n", ExecutablePath);
    return;
}

void Discover(char *TargetDir, char *FileName, int d, int f)
{
    struct dirent **DirFiles;
    int NumFiles;
    char FilePath[BASE_LEN] = "";
    int FileFound = 0;

    NumFiles = scandir(TargetDir, &DirFiles, NULL, alphasort);
    // Scan all the files in the directory given by the path and store them in DirFiles with the number in Numfiles
    // We also implement alphasort to ensure the files are in alphabtical order

    // If number of files in the directory is negative, there is an error
    if (NumFiles < 0)
    {
        printf("Error - discover: Read error\n");
        return;
    }

    // We print the path of the current directory if we either have the -d flag or neither flag
    if ((d || (!d && !f)) && FileName == NULL)
        printf("%s\n", TargetDir);
    for (int a = 0; a < NumFiles; a++)
    {
        struct stat FileInfo;
        strcpy(FilePath, TargetDir);
        strcat(FilePath, "/");
        strcat(FilePath, DirFiles[a]->d_name);
        // Using the name of the file/directory we make its path absolute and check if its valid

        // If the current directory being considered is "." or ".." (which denotes the current directory and its parent directory), we just continue
        if (!strcmp(DirFiles[a]->d_name, ".") || !strcmp(DirFiles[a]->d_name, ".."))
            continue;

        if (stat(FilePath, &FileInfo))
        {
            printf("Error - discover: File Error\n");
            return;
        }

        // If we havent been passed any file to look for, we have to print all the directories/files to the terminal based on the passed flags
        if (FileName == NULL)
        {
            // If the current object being considered is a file and we have been given the -f flag
            // Or if we havent been given either flag and the current object being considered is a file, we print its path relative to the directory discover was called in
            if (f && S_ISREG(FileInfo.st_mode) || (!d && !f && S_ISREG(FileInfo.st_mode)))
                printf("%s\n", FilePath);

            // If the object being considered is a directory
            if (S_ISDIR(FileInfo.st_mode))
            {
                // If we have been given the -d flag or we have been given neither flag, we print its path relative to the directory discover was called in
                if ((d || (!d && !f)))
                    printf("%s\n", FilePath);

                // We call discover recursively on every directory and their sub-directories except the current directory itself as that would cause an infinite loop
                if (strcmp(basename(FilePath), "."))
                    Discover(FilePath, FileName, d, f);
            }
        }

        // If we have been passed a file to look for within the directory
        else
        {
            // If the object being considered is a file
            if (S_ISREG(FileInfo.st_mode))
            {
                // If the file is the file that we are looking for, print its path relative to the directory discover was called in and return
                if (!strcmp(basename(FilePath), FileName))
                {
                    printf("%s\n", FilePath);
                    FileFound = 1;
                }
            }

            // If the object being considered is a directory, we call discover on that and look within it for the file
            else if (S_ISDIR(FileInfo.st_mode))
                Discover(FilePath, FileName, d, f);
        }
    }

    if (!FileFound && FileName != NULL)
        printf("Couldn't find the file %s\n", FileName);

    // Free the memory that was allocated by scandir
    for (int i = 0; i < NumFiles; i++)
        free(DirFiles[i]);
    free(DirFiles);
    return;
}

void History(char *Input[])
{
    char CommandsExecuted[BASE_LEN];
    char ReverseBuffer[10][BASE_LEN];
    char HistFilePath[BASE_LEN] = "";
    strcpy(HistFilePath, getenv("HOME"));
    strcat(HistFilePath, "/history.txt");
    // We set the path of the history.txt file so that it can be accessed from any other directory as well

    FILE *HistoryFile;
    HistoryFile = fopen(HistFilePath, "a+");
    // Open the file and see if its valid

    if (HistoryFile == NULL)
    {
        printf("Error - history: File open error\n");
        return;
    }

    // We then read the 10 most recent commands into a buffer as while printing we have to print oldest to newest but we read newest to oldest
    for (int i = 0; fgets(CommandsExecuted, 1000, HistoryFile) != NULL && i < 10; i++)
    {
        strcpy(ReverseBuffer[9 - i], CommandsExecuted);
    }

    // After reading the commands into to the buffer in reverse order, we then print it normally
    // Which leads to it printing the commands oldest to newest, which is what we wanted
    for (int i = 0; i < 10; i++)
        printf("%s", ReverseBuffer[i]);
}

void jobs(char *Input[])
{
    FILE *ProcessInfo;
    int a, i, s, r;

    int NumArgs, NumFlags;

    for (NumArgs = 0; Input[NumArgs] != NULL; NumArgs++)
        ;

    for (i = 1; i < NumArgs; i++)
    {
        if (Input[i][0] == '-')
        {
            NumFlags++;
            if (strchr(Input[i], 's') != NULL)
                s++;
            if (strchr(Input[i], 'r') != NULL)
                r++;
        }
    }

    for (a = 0; a < NumBGP; a++)
    {
        char ProcessPath[BASE_LEN];
        char ProcessDetails[BIG_LEN];
        char *ProcessDetailsParsed[BASE_LEN] = {NULL};
        i = 0;
        if (BGProcesses[a]->PID != -1)
        {
            sprintf(ProcessPath, "/proc/%d/stat", (int)BGProcesses[a]->PID);

            ProcessInfo = fopen(ProcessPath, "r");

            if (ProcessInfo == NULL)
            {
                printf("Error - jobs: File error\n");
                return;
            }
            fread(ProcessDetails, BIG_LEN, 1, ProcessInfo);
            fclose(ProcessInfo);

            ProcessDetailsParsed[i] = strtok(ProcessDetails, " ");
            while (ProcessDetailsParsed[i] != NULL)
            {
                i++;
                ProcessDetailsParsed[i] = strtok(NULL, " ");
            }

            if (!strcmp(ProcessDetailsParsed[2], "R") && (r || (!r && !s)))
                printf("[%d] Running %s [%d]\n", a + 1, BGProcesses[a]->name, BGProcesses[a]->PID);
            else if (s || (!r && !s))
                printf("[%d] Stopped %s [%d]\n", a + 1, BGProcesses[a]->name, BGProcesses[a]->PID);
        }
    }
}

void sig(char *Input[])
{
    int SigPass = atoi(Input[2]);
    int ProcessPID = BGProcesses[atoi(Input[1]) - 1]->PID;

    if (atoi(Input[1]) > NumBGP)
    {
        printf("Error - sig: Process doesn't exist\n");
        return;
    }

    kill(ProcessPID, SigPass);
    return;
}

void ExitFunction()
{
    KillBGP();
    fputs("\033c", stdout);
    exit(0);
}
