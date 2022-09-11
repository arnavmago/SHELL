#ifndef PARSING_H
#define PARSING_H

void PrintCommandPrompt(int IsHomeDir, char *CommandPrompt, char *HomeDir);
// Evaluates the command promp that needs to be printed

void CheckRelative(char *AbsPath1, char *Abspath2);
// Checks if the current directory is a sub-directory of the home directory and gives the relative path if it is

void UnderstandInput(char *Input);
// Parse and clean (extra spaces, tabs, semi-colons) the input given by the user

void ExecuteInputCommand(char *Input, int BG);
// Execute functions based on the parsed input

#define BASE_LEN 1000
#define BIG_LEN 5000
#define MaxNumBGP 1000

#endif