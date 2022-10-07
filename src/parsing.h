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

void CheckBGP(char *Input);
// Executes functions when they are background processes

void PipedProcess(char *Input);
// Handles if the input command has pipes

int LargestCommonSubsequence(char NameHolder[][1000], int NumFiles, int shortest);
// Finds the longest sequence of characters common in all the given strings

void enableRawMode();
void disableRawMode();
// Function that will autocomplete the input the user gives based on whatever partially completed word is written when the tab key is clicked

void NewFgets();
// Function that replaces fgets to accept the input string (due to autocomplete)

void CompleteInput();
// Function that handles autocompletion

#define BASE_LEN 1000
#define BIG_LEN 5000
#define MaxNumBGP 1000

#endif