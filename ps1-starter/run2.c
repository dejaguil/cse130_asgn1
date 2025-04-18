#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv)
{
    /* There should at least be 3 arguments in addition to run2: 2 for the first command, and the rest for the second command */
    if (argc < 4) {
        fprintf(stderr, "Usage: %s cmd1 cmd1_arg cmd2 [cmd2_args ..]\n", argv[0]);
        return 1;
    }

    int status;

    pid_t first_child = fork();
    // First child
    if (first_child == 0) {
        execlp(argv[1], argv[1], argv[2], NULL);
        perror("execlp failed");
        exit(1);
    }

    pid_t second_child = fork();

    if (second_child == 0) {
        // Second child
        execvp(argv[3], &argv[3]);
        perror("execvp failed");
        exit(1);
    }

    // Wait for both children to finish
    for (int i = 0; i < 2; i++) {
        pid_t done = wait(&status);
        if (done > 0) {
            printf("exited=%d exitstatus=%d\n", WIFEXITED(status), WEXITSTATUS(status));
        }
    }

    return 0;
}
