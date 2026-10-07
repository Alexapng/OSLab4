#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *end = NULL;
    long n = strtol(argv[1], &end, 10);

    if (*argv[1] == '\0' || *end != '\0' || n < 0) {
        fprintf(stderr, "n must be a non-negative integer.\n");
        return EXIT_FAILURE;
    }

    /*
     * Be careful: the number of processes grows as 2^n.
     * The lab asks to test n = 3 and n = 5.
     */
    for (long i = 0; i < n; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            return EXIT_FAILURE;
        }

        sleep(5);
    }

    printf("Finished: PID=%d, PPID=%d\n", getpid(), getppid());
    return EXIT_SUCCESS;
}
