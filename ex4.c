#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

extern char **environ;

#define MAX_LINE 1024
#define MAX_ARGS 64

static void reap_finished_children(void)
{
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        /* Reap completed background processes. */
    }
}

static void execute_with_path(char *const argv[])
{
    const char *command = argv[0];

    /* If the user supplied a path directly, try it as-is. */
    if (strchr(command, '/') != NULL) {
        execve(command, argv, environ);
        perror("execve");
        _exit(127);
    }

    const char *path_env = getenv("PATH");
    if (path_env == NULL) {
        fprintf(stderr, "PATH is not set\n");
        _exit(127);
    }

    char *path_copy = strdup(path_env);
    if (path_copy == NULL) {
        perror("strdup");
        _exit(127);
    }

    char *saveptr = NULL;
    char *dir = strtok_r(path_copy, ":", &saveptr);

    while (dir != NULL) {
        char full_path[PATH_MAX];

        if (snprintf(full_path, sizeof(full_path), "%s/%s", dir, command)
            < (int)sizeof(full_path)) {
            execve(full_path, argv, environ);
        }

        dir = strtok_r(NULL, ":", &saveptr);
    }

    fprintf(stderr, "%s: command not found\n", command);
    free(path_copy);
    _exit(127);
}

int main(void)
{
    char line[MAX_LINE];

    while (1) {
        reap_finished_children();

        printf("myshell> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            putchar('\n');
            break;
        }

        line[strcspn(line, "\n")] = '\0';

        char *argv[MAX_ARGS];
        int argc = 0;

        char *token = strtok(line, " \t");
        while (token != NULL && argc < MAX_ARGS - 1) {
            argv[argc++] = token;
            token = strtok(NULL, " \t");
        }
        argv[argc] = NULL;

        if (argc == 0) {
            continue;
        }

        if (strcmp(argv[0], "exit") == 0) {
            break;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            execute_with_path(argv);
        }

        /*
         * Intentionally no blocking wait here:
         * the command runs in another (background) process,
         * so the shell can immediately read the next command.
         */
        printf("[started PID %d]\n", pid);
    }

    /* Reap any children that already finished before the shell exits. */
    reap_finished_children();

    return EXIT_SUCCESS;
}
