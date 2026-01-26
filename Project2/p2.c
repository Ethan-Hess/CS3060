/*
 * Name: Ethan Hess
 * Section: 3060-x02
 * Assignment: Program 2 - The fork() and exec() System Calls
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        exit(1);
    }
    if (pid == 0)
    { // Child process
        printf("CHILD started. ");
        if (argc == 1)
        {
            printf("No arguments provided. Terminating child.\n");
            exit(0);
        }
        else if (argc == 2)
        {
            printf("One argument provided. Calling execlp(), never to return (sniff)\n");
            execlp(argv[1], argv[1], (char *)NULL);
            perror("execlp failed");
            exit(1);
        }
        else
        {
            printf("More than one argument provided. Calling execvp(), never to return (sniff)\n");
            execvp(argv[1], &argv[1]);
            perror("execvp failed");
            exit(1);
        }
    }
    else
    { // Parent process
        printf("PARENT started, now waiting for process ID#%d\n", pid);
        waitpid(pid, &status, 0);
        int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        printf("PARENT resumed. Child exit code of %d. Now terminating parent\n", exit_code);
    }
    return 0;
}
