# Readme file for SHELL

## Makefile Information -

Running the commands **make** and then **./a** in the directory contaning the files will clear the terminal and begin the shell, as is indicated by the command prompt that will print on the terminal.
The directory the shell was invoked in is set as the home directory and is henceforth represented as **~**.

### Cosemetic commands -

The commands **exit**, **quit**, **yeet** and **leavethechat** end the running program, clear everything on the terminal at that point and take you back to the original terminal.
The command **clear** (or **c**) clear everything on the terminal at that point (does not quit the program).
All commands are accepted in lowercase only.

### User defined commands -

The commands that have been defined by me are -

#### cd -

We read the arguments, if they are relative we make their paths absolute and then use the command chdir to change the directory to the directory passed as an argument

#### pwd -

Prints the absolute path current directory

#### echo -

Prints whatever was passed after the command echo onto the terminal

#### ls -

First gets the number of flag and non-flag arguments, handles files and directories differently, prints directories in green, executables in red and regular files in white
prints the same content as bash for the -l and -a flags

#### pinfo -

Gives the process information of the current runnning process if no argument is passed. If the PID of a process is passed as an argument, prints the info of that process.

#### Discover -

Recursively prints all the files and directories for the directory it is called on, if a filename has been passed as an argument then it prints the path of that file relative to the current directory.

#### History -

Prints the 10 most recently executed commands to the terminal from oldest to newest.
