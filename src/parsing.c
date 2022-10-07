#include "headers.h"
#include "allFunctions.h"
#include "parsing.h"

// All variables defined in other files that are needed here
extern char CommandPrompt[];
extern char HomeDir[];
extern char CurrentDir[];
extern char Input[];
extern double TimeFlag;

// Defined new variables that are needed
int InputFile;
int OutputFile;
int OutputAppend;
struct termios input;

// Taken from file given to us for detecting key presses
void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &input) 1 atexit(disableRawMode);
    struct termios raw = input;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)
}

// Taken from file given to us for detecting key presses
void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &input)
}

int LargestCommonSubsequence(char NameHolder[][BASE_LEN], int NumFiles, int shortest)
{
    int i, BreakFlag = 0, count = 0;

    // Run the loop from the first character to the length of the shortest given string
    for (i = 1; i <= shortest; i++)
    {
        // check each file against the first one
        for (count = 1; count < NumFiles; count++)
        {
            if (!strncmp(NameHolder[0], NameHolder[count], i))
                continue;
            else
            {
                BreakFlag = 1;
                i--;
                break;
            }
        }
        if (BreakFlag)
            break;
    }
    return i;
}

void CompleteInput()
{
    struct stat FileInfo;
    struct dirent **DirFiles;

    // To store all the characters given after the space
    char PostSpace[BASE_LEN];

    // To store all the characters given before the space
    char PreSpace[BASE_LEN];

    int SpaceFlag = 0, SpacePos = -1, i, CountPost = 0, CountPre = 0;

    for (i = strlen(Input) - 1; i >= 0; i--)
    {
        // space has ASCII value 32, thus if we have been given a space character in the input, we store the position of the last one
        if ((int)Input[i] == 32)
        {
            SpaceFlag = 1;
            SpacePos = i;
            break;
        }
    }

    // We copy the string after the space to PostSpace
    // If we havent been given space, we copy the entire string to it
    for (i = SpacePos + 1; i < strlen(Input); i++)
        PostSpace[CountPost++] = Input[i];
    PostSpace[CountPost] = '\0';

    // We copy the string before the space to PreSpace
    // If we havent been given space, we copy the nothing to it
    for (i = 0; i <= SpacePos; i++)
        PreSpace[CountPre++] = Input[i];
    PreSpace[CountPre] = '\0';

    // If we have no characters after the spacebar (but have a spacebar)
    if (CountPost == 0)
    {
        int NumFiles = scandir(".", &DirFiles, NULL, NULL);

        // If the only files/directories we have are "." and ".."
        if (NumFiles == 2)
            return;

        // If we have ".", ".." and 1 other file or directory, we autofill the input to that
        else if (NumFiles == 3)
        {
            for (i = 0; i < NumFiles; i++)
            {
                if (strcmp(DirFiles[i]->d_name, "..") && strcmp(DirFiles[i]->d_name, "."))
                {
                    // Checking whether the directory/file is a directory or a file
                    stat(DirFiles[i]->d_name, &FileInfo);
                    strcpy(PostSpace, DirFiles[i]->d_name);
                    if (S_ISDIR(FileInfo.st_mode))
                        strcat(PostSpace, "/");
                    else if (S_ISREG(FileInfo.st_mode))
                        strcat(PostSpace, " ");
                }
            }
            strcat(Input, PostSpace);
        }

        // If we have more than 1 file/directory besides "." and ".."
        else
        {
            // Array to store all the file and directory names
            char NamesToPass[NumFiles - 2][BASE_LEN];

            // Set the shortest length to an arbitrary large number
            int shortest = 1000;
            int count = 0;
            printf("\n");
            for (i = 0; i < NumFiles; i++)
            {
                if (strcmp(DirFiles[i]->d_name, "..") && strcmp(DirFiles[i]->d_name, "."))
                {
                    // Checking whether the directory/file is a directory or a file
                    stat(DirFiles[i]->d_name, &FileInfo);
                    if (S_ISDIR(FileInfo.st_mode))
                        printf("%s/\n", DirFiles[i]->d_name);
                    else if (S_ISREG(FileInfo.st_mode))
                        printf("%s \n", DirFiles[i]->d_name);
                    shortest = strlen(DirFiles[i]->d_name) < shortest ? strlen(DirFiles[i]->d_name) : shortest;
                    strcpy(NamesToPass[count++], DirFiles[i]->d_name);
                }
            }

            // Find the length of the sequence common to all the strings
            int NumCommon = LargestCommonSubsequence(NamesToPass, NumFiles - 2, shortest);
            if (NumCommon > 0)
            {
                int x;
                char Copy[NumCommon + 1];
                for (x = 0; x < NumCommon; x++)
                    Copy[x] = NamesToPass[0][x];
                Copy[x] = '\0';
                strcat(Input, Copy);
            }
        }
    }

    // If we been given characters after the space, or we have been given no space
    else if (CountPost > 0)
    {
        int NumFiles = scandir(".", &DirFiles, NULL, NULL);

        // If the only files/directories we have are "." and ".."
        if (NumFiles == 2)
            return;

        // If we have ".", ".." and 1 other file or directory, we autofill the input to that if it matches the characers we have been given after the space
        else if (NumFiles == 3)
        {
            for (i = 0; i < NumFiles; i++)
            {
                if (strcmp(DirFiles[i]->d_name, "..") && strcmp(DirFiles[i]->d_name, ".") && !strncmp(PostSpace, DirFiles[i]->d_name, strlen(PostSpace)))
                {
                    // Checking whether the directory/file is a directory or a file
                    stat(DirFiles[i]->d_name, &FileInfo);
                    strcpy(PostSpace, DirFiles[i]->d_name);
                    if (S_ISDIR(FileInfo.st_mode))
                        strcat(PostSpace, "/");
                    else if (S_ISREG(FileInfo.st_mode))
                        strcat(PostSpace, " ");
                }
            }
            // If we have been given a space
            if (SpacePos != -1)
            {
                strcat(PreSpace, PostSpace);
                strcpy(Input, PreSpace);
            }

            // If we havent been given a space
            else
                strcpy(Input, PostSpace);
        }

        // If we have more than 1 file/directory besides "." and ".."
        else
        {
            int CorrectComplete = 0;
            for (i = 0; i < NumFiles; i++)
            {
                // Checking how many of the files or directories match the string we have been given after the space
                if (strcmp(DirFiles[i]->d_name, "..") && strcmp(DirFiles[i]->d_name, ".") && !strncmp(PostSpace, DirFiles[i]->d_name, strlen(PostSpace)))
                    CorrectComplete++;
            }

            // If we have no directories/files that match the input string
            if (CorrectComplete == 0)
                return;

            // If we have only 1 directory/file that matches the given input, we autofill it to that
            else if (CorrectComplete == 1)
            {
                for (i = 0; i < NumFiles; i++)
                {
                    if (strcmp(DirFiles[i]->d_name, "..") && strcmp(DirFiles[i]->d_name, ".") && !strncmp(PostSpace, DirFiles[i]->d_name, strlen(PostSpace)))
                    {
                        // Checking whether the directory/file is a directory or a file
                        stat(DirFiles[i]->d_name, &FileInfo);
                        strcpy(PostSpace, DirFiles[i]->d_name);
                        if (S_ISDIR(FileInfo.st_mode))
                            strcat(PostSpace, "/");
                        else if (S_ISREG(FileInfo.st_mode))
                            strcat(PostSpace, " ");
                    }
                }
                // If we have been given a space
                if (SpacePos != -1)
                {
                    strcat(PreSpace, PostSpace);
                    strcpy(Input, PreSpace);
                }

                // If we havent been given a space
                else
                    strcpy(Input, PostSpace);
            }

            // If we have more than one directories or files that match the given input after the space
            else if (CorrectComplete > 1)
            {
                // Array to store all the file and directory names
                char NamesToPass[CorrectComplete][BASE_LEN];

                // Set the shortest length to an arbitrary large number
                int shortest = 1000;
                int count = 0;
                printf("\n");
                for (i = 0; i < NumFiles; i++)
                {

                    if (strcmp(DirFiles[i]->d_name, "..") && strcmp(DirFiles[i]->d_name, ".") && !strncmp(PostSpace, DirFiles[i]->d_name, strlen(PostSpace)))
                    {
                        // Checking whether the directory/file is a directory or a file
                        stat(DirFiles[i]->d_name, &FileInfo);
                        if (S_ISDIR(FileInfo.st_mode))
                            printf("%s/\n", DirFiles[i]->d_name);
                        else if (S_ISREG(FileInfo.st_mode))
                            printf("%s \n", DirFiles[i]->d_name);
                        shortest = strlen(DirFiles[i]->d_name) < shortest ? strlen(DirFiles[i]->d_name) : shortest;
                        strcpy(NamesToPass[count++], DirFiles[i]->d_name);
                    }
                }

                // Find the length of the sequence common to all the strings
                int NumCommon = LargestCommonSubsequence(NamesToPass, CorrectComplete, shortest);
                if (NumCommon > 0)
                {
                    int x;
                    char Copy[NumCommon + 1];
                    for (x = 0; x < NumCommon; x++)
                        Copy[x] = NamesToPass[0][x];
                    Copy[x] = '\0';
                    if (SpacePos == -1)
                        strcpy(Input, Copy);
                    else
                    {
                        strcat(PreSpace, Copy);
                        strcpy(Input, PreSpace);
                    }
                }
            }
        }
    }
}

void NewFgets()
{
    setbuf(stdout, NULL);
    char temp;
    int CurrChar = 0;
    // Enable raw mode to autocomplete if tab key is detected
    enableRawMode();

    while (read(STDIN_FILENO, &temp, 1))
    {
        // If we detect a control key (tab, enter, backspace, ctrl+d, etc.)
        if (iscntrl(temp))
        {
            // If the key detected a tab
            if (temp == 9)
            {
                Input[CurrChar] = '\0';
                CompleteInput();
                CurrChar = strlen(Input);
                PrintCommandPrompt(0, CommandPrompt, HomeDir);
                strcat(CommandPrompt, Input);
                printf("\r%s", CommandPrompt);
            }

            // If the key detected is enter
            else if (temp == 10)
            {
                Input[CurrChar++] = temp;
                printf("\n");
                break;
            }

            // If the key detected is a backspace
            else if (temp == 127)
            {
                if (CurrChar != 0)
                {
                    CurrChar--;
                    Input[CurrChar] = '\0';
                    printf("\b \b");
                }
                continue;
            }

            // If the key detected is Ctrl+D
            else if (temp == 4)
                ExitFunction();
        }

        // If no control key is detected (therefore normal characters)
        else
        {
            Input[CurrChar++] = temp;
            printf("%c", temp);
        }
    }
    disableRawMode();
    // The function then returns to the main.c file after having been autocmpleted where it is then executed
}

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

        if (strchr(ProperInput, '|') != NULL)
            PipedProcess(ProperInput);
        else
        {
            // If the command contains an ampersand, its a background process
            if (strchr(ProperInput, '&') != NULL)
                CheckBGP(ProperInput);

            // If the command doesnt contain an ampersand, its a foreground process
            else
                ExecuteInputCommand(ProperInput, 0);
        }
    }
    return;
}

void CheckBGP(char *ProperInput)
{
    int AmpCount = 0, a;
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

    return;
}

void PipedProcess(char *ProperInput)
{
    int PipeCount = 0, Error, a, Status;
    char *PipedCommands[100] = {NULL};

    for (a = 0; a <= strlen(ProperInput) - 1; a++)
    {
        if (ProperInput[a] == '|')
            PipeCount++;
    }
    a = 0;

    PipedCommands[a] = strtok(ProperInput, "|");
    while (PipedCommands[a] != NULL)
    {
        a++;
        PipedCommands[a] = strtok(NULL, "|");
    }

    int ReadFrom = STDIN_FILENO;

    for (a = 0; a <= PipeCount; a++)
    {
        int PipeFDs[2];
        Error = pipe(PipeFDs);
        if (Error == -1)
        {
            printf("Error - Piping: Unable to pipe the commands\n");
            return;
        }

        int PID = fork();

        if (PID == -1)
        {
            printf("Error - Piping: Fork error\n");
            return;
        }

        if (PID == 0)
        {
            dup2(ReadFrom, STDIN_FILENO);

            if (a != PipeCount)
                dup2(PipeFDs[1], STDOUT_FILENO);

            close(PipeFDs[0]);

            if (strchr(PipedCommands[a], '&') != NULL)
                CheckBGP(PipedCommands[a]);
            else
                ExecuteInputCommand(PipedCommands[a], 0);

            exit(0);
        }
        else
        {
            waitpid(PID, &Status, WUNTRACED);
            close(PipeFDs[1]);
            ReadFrom = PipeFDs[0];
        }
    }
}

void ExecuteInputCommand(char *Input, int BG)
{
    char Temp[strlen(Input)];
    char Command[100];
    strcpy(Temp, Input);
    InputFile = 0, OutputFile = 0, OutputAppend = 0;
    char InFileName[BASE_LEN];
    char OutFileName[BASE_LEN];
    int Inputfd, Outputfd;
    int InFileIndex = 0, OutFileIndex = 0;
    int Stdin, Stdout;

    // Putting the command in the history file
    InitHistory(Temp);

    // Splitting the parsed command into words so we can compare the first word (as that will be the main command to execute, and the rest will be its arguments)
    char *Words[100] = {NULL};
    Words[0] = strtok(Temp, " ");
    strcpy(Command, Words[0]);
    for (int a = 1; a < 100; a++)
    {
        // Checking for input file symbol
        if (!strcmp(Words[a - 1], "<"))
        {
            InputFile = 1;
            InFileIndex = a;
        }

        // Checking for output file symbol
        else if (!strcmp(Words[a - 1], ">"))
        {
            OutputFile = 1;
            OutFileIndex = a;
        }

        // Checking for output fule append symbol
        else if (!strcmp(Words[a - 1], ">>"))
        {
            OutputAppend = 1;
            OutFileIndex = a;
        }

        Words[a] = strtok(NULL, " ");
        if (Words[a] == NULL)
            break;
    }

    // if '<' is present
    if (InputFile)
    {
        if (Words[InFileIndex] != NULL)
            strcpy(InFileName, Words[InFileIndex]);
        else
        {
            printf("Error - Redirection: Invalid input file\n");
            return;
        }
    }

    // If '>' or '>>' are present
    if (OutputFile || OutputAppend)
    {
        if (Words[OutFileIndex] != NULL)
            strcpy(OutFileName, Words[OutFileIndex]);
        else
        {
            printf("Error - Redirection: Invalid output file\n");
            return;
        }
    }

    if (InputFile)
    {
        // Create a backup for stdin, which we use to reset it back after execution)
        Stdin = dup(STDIN_FILENO);
        Inputfd = open(InFileName, O_RDONLY);
        if (Inputfd == -1)
        {
            printf("Error - Redirection: Failed to open input file\n");
            close(Inputfd);
            return;
        }

        // Allows us to use Inputfd and STDIN_FILENO as the same, therefore instead of reading from stdin, the command reads from the file
        int Error = dup2(Inputfd, STDIN_FILENO);
        if (Error == -1)
        {
            printf("Error - Redirection: Failed to redirect input to file\n");
            close(Inputfd);
            return;
        }
        close(Inputfd);
    }
    if (OutputFile || OutputAppend)
    {
        // Create a backup for stdin, which we use to reset it back after execution)
        Stdout = dup(STDOUT_FILENO);

        // If '>>' is present, open to append
        if (OutputAppend)
            Outputfd = open(OutFileName, O_CREAT | O_WRONLY | O_APPEND, 0644);

        // If '>' is present, open to write
        else
            Outputfd = open(OutFileName, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (Outputfd == -1)
        {
            printf("Error - Redirection: Failed to open output file\n");
            close(Outputfd);
            return;
        }
        // Allows us to use Inputfd and STDOUT_FILENO as the same, therefore instead of reading from stdout, the command writes to the file
        int Error = dup2(Outputfd, STDOUT_FILENO);
        if (Error == -1)
        {
            printf("Error - Redirection: Failed to redirect output to file\n");
            close(Outputfd);
            return;
        }
        close(Outputfd);
    }

    if (InFileIndex)
    {
        // We set the location of the '<' symbol to null so the the commmand wont read beyond that
        Words[InFileIndex - 1] = NULL;
    }
    if (OutFileIndex)
    {
        // We set the location of the '>' or '>>' symbol to null so the the commmand wont read beyond that
        Words[OutFileIndex - 1] = NULL;
    }

    // Comparing the first word in the input command to decide what function is to be called
    if (!strcmp(Command, "exit") || !strcmp(Command, "quit") || !strcmp(Command, "q") || !strcmp(Command, "yeet") || !strcmp(Command, "leavethechat"))
        ExitFunction();
    else if (!strcmp(Command, "cd"))
        cd(Words);
    else if (!strcmp(Command, "pwd"))
        pwd();
    else if (!strcmp(Command, "echo"))
        echo(Words);
    else if (!strcmp(Command, "clear") || !strcmp(Command, "c"))
        fputs("\033c", stdout);
    else if (!strcmp(Command, "ls"))
        ls(Words);
    else if (!strcmp(Command, "pinfo"))
        pinfo(Words);
    else if (!strcmp(Command, "discover"))
        InitDiscover(Words);
    else if (!strcmp(Command, "history"))
        History();
    else if (!strcmp(Command, "fg"))
        fg(Words);
    else if (!strcmp(Command, "bg"))
        bg(Words);
    else if (!strcmp(Command, "jobs"))
        jobs(Words);
    else if (!strcmp(Command, "sig"))
        sig(Words);
    else
    {
        if (BG)
            BackgroundProcess(Words);
        else
            ForegroundProcess(Words);
    }

    // We reset stdin and stdout back to default
    if (InputFile)
        dup2(Stdin, STDIN_FILENO);
    if (OutputFile || OutputAppend)
        dup2(Stdout, STDOUT_FILENO);

    return;
}
