#ifndef ALLFUNCTIONS_H
#define ALLFUNCTIONS_H

void cd(char *Input[]);
// User defined function to implement cd

void pwd();
// User defined function to implement pwd

void echo(char *Input[]);
// User defined function to implement echo

void ls(char *Input[]);
// User defined function to implement ls

void Permissions(struct stat *File);
// Function to get all the permissions of a file/directory for ls

void OtherFileDetails(struct stat File);
// Function to get other file information for ls

void ForegroundProcess(char *Input[]);
// Function to execute foreground processes

void BackgroundProcess(char *Input[]);
// Function to execute background processes

void InitBackgroundProcesses();
// Function to initialize the array of structs that holds the names and PIDs of background processes

void AddBGP(int PID, char Input[]);
// Function to add the information of a background process to the array of structs

void KillBGP();
// Function to kill all non-terminated background processes

void pinfo(char *Input[]);
// User defined function to get some basic information of any process

void InitDiscover(char *Input[]);
// Function to define what the discover function will do

void Discover(char *TargetDir, char *FileName, int d, int f);
// User defined function to either list all files/directories within a directory or search for a specific file within the directory

void InitHistory(char Input[]);
// Function to initialize the path required to ensure conitinuity of history irrespective of current working directory

void History();
// User defined fucntion to create a file called history.txt that stors that last 20 input commands

handler *InitSigHandler(int signum, handler *HandlerFunction);
// Function to initialize the signal handling required to get the exit information of a background process

void ChildHandler(int sig, siginfo_t *info, void *vp);
// Function that gets and prints the exit information of the background processes

void fg(char *Input[]);
// Function that brings a background process into the foreground

void bg(char *Input[]);
// Function that tells a stopped background process to continue

void jobs(char *Input[]);
// Function that prints all the currently running background processes in alphabetical order

void sig(char *Input[]);
// Function that sends the specified singal to the specified background process

void ExitFunction();
// Function to handle the exit protocol for the shell

#endif