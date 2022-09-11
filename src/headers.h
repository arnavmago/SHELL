#ifndef HEADERS_H
#define HEADERS_H

// All the required inbuilt header files
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>
#include <grp.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/signal.h>

typedef void handler(int signum, siginfo_t *info, void *vp);
// Used for signal handling

typedef struct ProcessInformation
{
    int PID;
    char name[1000];
} ProcInfo;
// data structure to store the names and PIDs the backrounds processes

#endif