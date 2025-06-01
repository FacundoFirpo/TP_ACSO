#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200
#define MAX_LINE 256

void liberar_comandos(char *commands[], int count) {
    for (int i = 0; i < count; i++) {
        free(commands[i]);
    }
}

int parsear_comandos(char *linea, char *commands[]) {
    int count = 0;
    int in_quotes = 0;
    char *start = linea;

    for (char *p = linea;; ++p) {
        if (*p == '"') in_quotes = !in_quotes;
        if ((*p == '|' && !in_quotes) || *p == '\0') {
            size_t len = p - start;

            // Trim espacios al inicio
            while (len > 0 && start[0] == ' ') { start++; len--; }
            // Trim espacios al final
            while (len > 0 && start[len - 1] == ' ') len--;

            char *cmd = malloc(len + 1);
            if (!cmd) {
                perror("malloc");
                exit(1);
            }
            strncpy(cmd, start, len);
            cmd[len] = '\0';

            commands[count++] = cmd;
            if (*p == '\0') break;

            start = p + 1;
        }
    }

    return count;
}

void ejecutar_comandos(char *commands[], int count) {
    int in_fd = 0;
    int fd[2];

    for (int i = 0; i < count; i++) {
        if (i < count - 1) {
            if (pipe(fd) < 0) {
                perror("pipe");
                exit(1);
            }
        }

        pid_t pid = fork();
        if (pid == 0) {
            if (in_fd != 0) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (i < count - 1) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
            }

            execlp("sh", "sh", "-c", commands[i], (char *)NULL);
            perror("command not found");
            exit(1);
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        }

        if (in_fd != 0) close(in_fd);
        if (i < count - 1) {
            close(fd[1]);
            in_fd = fd[0];
        }
    }

    for (int i = 0; i < count; i++) {
        wait(NULL);
    }
}

int main() {
    char linea[MAX_LINE];
    char *commands[MAX_COMMANDS];
    int count;

    while (1) {
        printf("Shell> ");
        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            printf("\n");
            break;
        }

        linea[strcspn(linea, "\n")] = '\0';

        if (strcmp(linea, "exit") == 0) {
            break;
        }

        count = parsear_comandos(linea, commands);
        ejecutar_comandos(commands, count);
        liberar_comandos(commands, count);
    }

    return 0;
}
