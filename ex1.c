#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

static void print_process(const char *name, clock_t start)
{
    clock_t end = clock();
    double time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;

    printf("%-10s | PID = %-6d | PPID = %-6d | Time = %.3f ms\n",
           name, getpid(), getppid(), time_ms);
    fflush(stdout);
}

int main(void)
{
    pid_t child1, child2;

    child1 = fork();
    clock_t after_first_fork = clock();

    if (child1 < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child1 == 0) {
        print_process("Child 1", after_first_fork);
        _exit(EXIT_SUCCESS);
    }

    child2 = fork();
    clock_t after_second_fork = clock();

    if (child2 < 0) {
        perror("fork");
        waitpid(child1, NULL, 0);
        return EXIT_FAILURE;
    }

    if (child2 == 0) {
        print_process("Child 2", after_second_fork);
        _exit(EXIT_SUCCESS);
    }

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    print_process("Main", after_first_fork);

    return EXIT_SUCCESS;
}
