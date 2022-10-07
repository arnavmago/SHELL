# Readme file for SHELL

## Makefile Information -

Running the commands **make** and then **./shell** in the directory contaning the files will clear the terminal and begin the shell, as is indicated by the command prompt that will print on the terminal.
The directory the shell was invoked in is set as the home directory and is henceforth represented as **~**.

### Cosemetic commands -

The commands **exit**, **quit**, **yeet** and **leavethechat** end the running program, clear everything on the terminal and take you back to the original terminal.
The command **clear** (or **c**) clear everything on the terminal. (does not quit the program).

**All commands are accepted in lowercase only.**

### Input Parsing -

First we handle the tabs and the semi colons, then we handle the uneven spacing. After we even out the spacing, we check for pipes and ampersands. We then execute each command (the commands having been split on semi colons with every entity being split being an individual command).
If the command has pipes, we tokenize on the **|** symbol and handle it to ensure that the output of the ith command is taken as the input for the (i+1)th command.
**I have assumed the user will input no invalid formats (For example, a null value before or after a pipe).**
If the command has an ampersand then we take it as multiple commands with the commands before the ampersand being background processes.

### User defined commands -

The commands that have been defined by me are -

#### cd -

We read the arguments, if they are relative we make their paths absolute and then use the command chdir to change the directory to the directory passed as an argument
**Use - cd <directory name>**

#### pwd -

Prints the absolute path current directory
**Use - pwd**

#### echo -

Prints whatever was passed after the command echo onto the terminal
**Use - echo <string to be echoed>**

#### ls -

First gets the number of flag and non-flag arguments, handles files and directories differently, prints directories in green, executables in red and regular files in white
prints the same content as bash for the -l and -a flags.
Directories printed in green, executable files in red, regular files in white.
**Use - ls <directory names> <file names> <flags>**

#### pinfo -

Gives the process information of the current runnning process if no argument is passed. If the PID of a process is passed as an argument, prints the info of that process.
**Use - pinfo <process ID>**

#### Discover -

Recursively prints all the files and directories for the directory it is called on, if a filename has been passed as an argument then it prints the path of that file relative to the current directory.
**Use - discover <file name> <directory name> <flags>**

#### History -

Prints the 10 most recently executed commands to the terminal from oldest to newest.
The history.txt file is made in the home directory of the computer to ensure continuity of the commands even when the shell is invoked from a diffrent directory.
**Use - history**

#### jobs - 

Prints all the running or stopped background processes in alphabetical order of their names and their process statuses.
**Use - jobs**

#### sig - 

Sends the signal number (passed as an argument) to the job corresponding to the job number which is passed as an argument.
**Use - sig <signal number> <job number>**

#### fg - 

Takes the number of a process in the background and brings it to the foregoround.
**Use - fg <job number>**

#### bg - 

Takes the job number of a process in the background and if its a stopped process, contniues it.
**Use - bg <job number>**

#### Exit Function - 

Kills all background processes, cleard the terminal and exits the shell.

### Signal Handling - 

#### Ctrl + C - 

Interrupts the currently running foreground process and kills it (doesn't stop, directly terminates).

#### Ctrl + Z - 

Stops the curently running foreground process and changes it into a background process. (As it is stopped and not terminated, it is also added to our array of background processes with its process status being stopped).

#### Ctrl + D - 

Exits the currently running shell and goes to the original terminal.

### Autocomplete

Custom wrote a function to take input from the terminal, if the tab key is hit, then upon evaluating the input given so far, the shell autocompletes the input to the most commmon point. After the autocompletion, the cursor is put at the end of the input to allow the user to continue typing after the tab key. If there is nothing to autocomplete the input to, it remains unchanged. 