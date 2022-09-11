#include "headers.h"
#include "allFunctions.h"
#include "parsing.h"

// All variables defined in other files that are needed here
extern char CommandPrompt[];
extern char HomeDir[];
extern char CurrentDir[];
extern char Input[];
extern double TimeFlag;

void PrintCommandPrompt(int IsHomeDir, char *CommandPrompt, char *HomeDir)
{
    char CurrDir[BASE_LEN];
    char Username[BASE_LEN];
    char SystemName[BASE_LEN];
    struct passwd *UserDetails = getpwuid(getuid());
    strcpy(Username, UserDetails->pw_name);
    // Getting the username

    gethostname(SystemName, 1000);
    // Getting the system name

    getcwd(CurrDir, sizeof(CurrDir));
    // Getting current directory

    // If shell has just been invoked, sets the current directory as the home directory
    if (IsHomeDir)
    {
        getcwd(HomeDir, 1000);
        strcpy(CurrentDir, HomeDir);
    }

    CheckRelative(HomeDir, CurrDir);
    // Checks if the current directory is a sub-directory of the home directory

    // Prints the time taken by a foreground process in the command prompt if the time taken is more than 1 second
    if (TimeFlag < 1)
        sprintf(CommandPrompt, "<%s@%s:%s> ", Username, SystemName, CurrDir);
    else
    {
        char TimeTakenByFGP[1000];
        sprintf(TimeTakenByFGP, "took %ds", (int)TimeFlag);
        sprintf(CommandPrompt, "<%s@%s:%s%s> ", Username, SystemName, CurrDir, TimeTakenByFGP);
        TimeFlag = 0;
    }
    return;
}

void CheckRelative(char *AbsPath1, char *Abspath2)
{
    int Path1Len = strlen(AbsPath1);
    int Path2Len = strlen(Abspath2);
    int Flag = 0;
    char *HomeFolder = basename(AbsPath1);
    // The folder that the shell has been invoked in

    if (!strncmp(AbsPath1, Abspath2, Path1Len))
    {
        char Path[1000] = "~";
        // Initialize  the relative path as ~

        char *Pieces = strtok(Abspath2, "/");
        // Split the current directories absolute path

        while (Pieces != NULL)
        {
            if (Flag)
            {
                strcat(Path, "/");
                strcat(Path, Pieces);
            }
            // Set flag to 1 if we reached the home folder of the shell in the current directories' splitted path
            if (!strcmp(Pieces, HomeFolder))
                Flag = 1;
            Pieces = strtok(NULL, "/");
        }
        strcpy(Abspath2, Path);
        // Copying the relatve path with the ~/ change to the second argument that was passed (which is thr current directory, and the first argument is the home directory)
    }
    return;
}

void UnderstandInput(char *Input)
{
    int Length = strlen(Input);
    int Num = 0, i = 0, a;
    for (a = 0; a < Length; a++)
    {
        // Cleaning up the commands and counting the number of commands we have using the number of semi colons
        if (Input[a] == '\t')
            Input[a] = ' ';
        else if (Input[a] == '\n')
        {
            Input[a] = ';';
            Num++;
        }
        else if (Input[a] == ';')
            Num++;
    }

    char *NumCommands[100] = {NULL};
    // Splitting the commands based on semi-colons (each term is a singluar command)
    NumCommands[i] = strtok(Input, ";");
    while (NumCommands[i] != NULL)
    {
        i++;
        NumCommands[i] = strtok(NULL, ";");
    }

    for (i = 0; i < Num; i++)
    {
        // Seperating each word and arranging them properly with a single space between each word
        char *SeparatedWords = strtok(NumCommands[i], " ");
        char ProperInput[BASE_LEN] = "";
        while (SeparatedWords != NULL)
        {
            strcat(ProperInput, SeparatedWords);
            strcat(ProperInput, " ");
            SeparatedWords = strtok(NULL, " ");
        }
        ProperInput[strlen(ProperInput) - 1] = '\0';

        // If the command contains an ampersand, its a background process
        if (strchr(ProperInput, '&') != NULL)
        {
            int AmpCount = 0;
            char *Background[100] = {NULL};

            for (a = 0; a <= strlen(ProperInput) - 1; a++)
            {
                if (ProperInput[a] == '&')
                    AmpCount++;
            }
            a = 0;

            // Splitting the command on the ampersand to get the actual command to execute
            Background[a] = strtok(ProperInput, "&");
            while (Background[a] != NULL && a < AmpCount)
            {
                a++;
                Background[a] = strtok(NULL, "&");
            }

            // for n ampersands we can have either n (background) commands or n+1 (n background, 1 foreground) commands
            for (a = 0; a < AmpCount; a++)
                ExecuteInputCommand(Background[a], 1);

            // If we have n+1 commands with the (n+1)th one being a foreground process it gets executed, else nothing happens
            if (Background[a] != NULL)
                ExecuteInputCommand(Background[a], 0);
        }

        // If the command doesnt contain an ampersand, its a foreground process
        else
            ExecuteInputCommand(ProperInput, 0);
    }
    return;
}

void ExecuteInputCommand(char *Input, int BG)
{
    char Temp[strlen(Input)];
    strcpy(Temp, Input);

    InitHistory(Temp);

    // Splitting the parsed command into words so we can compare the first word (as that will be the main command to execute, and the rest will be its arguments)
    char *Words[100] = {NULL};
    Words[0] = strtok(Temp, " ");
    for (int a = 1; a < 100; a++)
    {
        Words[a] = strtok(NULL, " ");
        if (Words[a] == NULL)
            break;
    }

    // Comparing the first word in the input command to decide what function is to be called
    if (!strcmp(Words[0], "exit") || !strcmp(Words[0], "quit") || !strcmp(Words[0], "q") || !strcmp(Words[0], "yeet") || !strcmp(Words[0], "leavethechat"))
        ExitFunction();
    else if (!strcmp(Words[0], "cd"))
        cd(Words);
    else if (!strcmp(Words[0], "pwd"))
        pwd();
    else if (!strcmp(Words[0], "echo"))
        echo(Words);
    else if (!strcmp(Words[0], "clear") || !strcmp(Words[0], "c"))
        fputs("\033c", stdout);
    else if (!strcmp(Words[0], "ls"))
        ls(Words);
    else if (!strcmp(Words[0], "pinfo"))
        pinfo(Words);
    else if (!strcmp(Words[0], "discover"))
        InitDiscover(Words);
    else if (!strcmp(Words[0], "history"))
        History();
    else if (!strcmp(Words[0], "fg"))
        fg(Words);
    else if (!strcmp(Words[0], "bg"))
        bg(Words);
    else if (!strcmp(Words[0], "jobs"))
        jobs(Words);
    else if (!strcmp(Words[0], "sig"))
        sig(Words);
    else
    {
        if (BG)
            BackgroundProcess(Words);
        else
            ForegroundProcess(Words);
    }
}
