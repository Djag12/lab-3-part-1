#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * Executes the command "cat scores | grep <argument> | sort".
 * The argument is passed from the command line to grep.
 */

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <search_term>\n", argv[0]);
        exit(1);
    }

    int pipefd1[2], pipefd2[2];
    pid_t pid1, pid2;

    // Create first pipe for cat -> grep
    if (pipe(pipefd1) == -1) {
        perror("Pipe1 Failed");
        exit(1);
    }

    // Create second pipe for grep -> sort
    if (pipe(pipefd2) == -1) {
        perror("Pipe2 Failed");
        exit(1);
    }

    pid1 = fork();

    if (pid1 < 0) {
        perror("Fork1 Failed");
        exit(1);
    }

    // First child process (P2) - handles grep <argument>
    if (pid1 == 0) {
        pid2 = fork();

        if (pid2 < 0) {
            perror("Fork2 Failed");
            exit(1);
        }

        // Child's child process (P3) - handles sort
        if (pid2 == 0) {
            // Replace standard input with input part of second pipe
            dup2(pipefd2[0], STDIN_FILENO);

            // Close unused pipes
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[1]);

            // Execute sort
            execlp("sort", "sort", NULL);
            perror("Exec sort failed");
            exit(1);
        }
        else {
            // Replace standard input with input part of first pipe
            dup2(pipefd1[0], STDIN_FILENO);

            // Replace standard output with output part of second pipe
            dup2(pipefd2[1], STDOUT_FILENO);

            // Close unused pipes
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);

            // Execute grep with the argument from the command line
            execlp("grep", "grep", argv[1], NULL);
            perror("Exec grep failed");
            exit(1);
        }
    }
    else {
        // Parent process (P1) - handles cat scores

        // Replace standard output with output part of first pipe
        dup2(pipefd1[1], STDOUT_FILENO);

        // Close unused pipes
        close(pipefd1[0]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        // Execute cat scores
        execlp("cat", "cat", "scores", NULL);
        perror("Exec cat failed");
        exit(1);
    }

    return 0;
}
