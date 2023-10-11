
# Custom UNIX Shell

This project is a C/C++ program designed to function as a shell interface. It accepts user commands and executes each command in a separate process. The implementation supports input and output redirection and pipes for interprocess communication. You can complete this project on any Linux, UNIX, or macOS system.

## Overview

A shell interface provides users with a prompt to enter commands. For example, the prompt "osh>" might appear, and the user can enter a command like "ls -l" to list files. The shell executes these commands by creating separate child processes. By default, the parent process waits for the child to finish. However, commands can also run concurrently in the background by adding an ampersand (&) at the end of the command.

The program's operation involves using UNIX system calls like `fork()`, `exec()`, `wait()`, `dup2()`, and `pipe()`.

## Implementation

The program consists of several parts:

1. **Creating the Child Process**: The main function forks a child process to execute user commands. It parses user input into separate tokens and stores them in an array for execution. It checks for the "&" symbol to determine if the parent process should wait for the child to exit.

2. **History Feature**: The shell provides a history feature that allows a user to execute the most recent command by entering "!!" at the prompt.

3. **Input and Output Redirection**: The shell supports input and output redirection using the ">" and "<" operators. For example, you can redirect output to a file or use a file as input to a command.

4. **Pipes**: The shell allows the output of one command to serve as the input for another using a pipe. For example, you can use the "|" operator to pipe the output of one command into another.

## Executing Commands

For simple command execution without pipes:

```shell
osh> ls -l
```

For pipes:

```shell
osh> ls -l | less
```

## Building and Running

1. Compile the program using your C/C++ compiler. For example, you can use g++:

   ```shell
   g++ -o myshell myshell.cpp
   ```

2. Run the program:

   ```shell
   ./myshell
   ```

## Sample Test Cases

Here are some test cases you can try:

- Execute a command: `osh> ls`
- Execute a command with options: `osh> ls -al`
- Run a command in the background: `osh> sleep 5 &`
- Repeat the last command: `osh> !!`
- Redirect output to a file: `osh> ls > output.txt`
- Redirect input from a file: `osh> sort < input.txt`
- Pipe two commands: `osh> ls -l | less`

## Author

- **Hannah Jeffi** -
