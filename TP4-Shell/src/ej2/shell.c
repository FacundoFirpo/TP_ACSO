#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200

// Función para limpiar espacios al inicio y final de un string
char *trim(char *str) {
    while(*str == ' ') str++; // Salta espacios iniciales
    char *end = str + strlen(str) - 1;
    while(end > str && *end == ' ') *end-- = '\0'; // Elimina espacios finales
    return str;
}

// Nueva función para parsear argumentos respetando comillas dobles
void parse_args(char *cmd, char **argv) {
    int arg = 0;
    char *p = cmd;
    while (*p) {
        while (*p == ' ') p++; // Salta espacios
        if (*p == '\"') {
            p++;
            argv[arg++] = p;
            while (*p && *p != '\"') p++;
            if (*p) { *p = '\0'; p++; }
        } else if (*p) {
            argv[arg++] = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
        }
    }
    argv[arg] = NULL;
}

int main() {

    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    while (1) 
    {
        printf("Shell> ");
        
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // Exit loop on EOF
        }
        command[strcspn(command, "\n")] = '\0';

        // Reemplazar tabs por espacios
        for (int i = 0; command[i]; i++) {
            if (command[i] == '\t') command[i] = ' ';
        }

        // Si el usuario escribe "exit", salir del shell
        if (strcmp(command, "exit") == 0) {
            break;
        }

        // Validar pipes al inicio, final o dobles
        char *tmp = command;
        while (*tmp == ' ') tmp++;
        if (*tmp == '|') {
            printf("Syntax error\n");
            continue;
        }
        int len = strlen(command);
        tmp = command + len - 1;
        while (tmp > command && *tmp == ' ') tmp--;
        if (*tmp == '|') {
            printf("Syntax error\n");
            continue;
        }
        if (strstr(command, "||") != NULL) {
            printf("Syntax error\n");
            continue;
        }
        // Pipes vacíos (ls | | wc)
        char *p = command;
        int last_pipe = 1; // true
        int empty_pipe = 0;
        while (*p) {
            if (*p == '|') {
                if (last_pipe) empty_pipe = 1;
                last_pipe = 1;
            } else if (*p != ' ') {
                last_pipe = 0;
            }
            p++;
        }
        if (empty_pipe) {
            printf("Syntax error\n");
            continue;
        }

        char *token = strtok(command, "|");
        while (token != NULL) 
        {
            commands[command_count++] = trim(token);
            token = strtok(NULL, "|");
        }

        /* You should start programming from here... */
        int fd[2];
        int in_fd = 0; // Entrada estándar al principio
        pid_t pid;
        int i;
        int too_many_args = 0;

        for (i = 0; i < command_count; i++) 
        {
            // Preparamos el pipe, excepto para el último comando
            if (i < command_count - 1) {
                pipe(fd);
            }

            pid = fork();
            if (pid == 0) {
                // Proceso hijo

                // Redirigir entrada si no es el primer comando
                if (in_fd != 0) {
                    dup2(in_fd, 0);
                    close(in_fd);
                }

                // Redirigir salida si no es el último comando
                if (i < command_count - 1) {
                    dup2(fd[1], 1);
                    close(fd[0]);
                    close(fd[1]);
                }

                // Parsear el comando en argumentos respetando comillas
                char *argv[256];
                parse_args(commands[i], argv);
                int argc = 0;
                while (argv[argc]) argc++;
                if (argc > 255) {
                    printf("Too many arguments\n");
                    exit(1);
                }
                execvp(argv[0], argv);
                printf("command not found\n");
                exit(1);
            } else if (pid < 0) {
                perror("fork");
                exit(1);
            }

            // Proceso padre
            if (in_fd != 0) close(in_fd);
            if (i < command_count - 1) {
                close(fd[1]);
                in_fd = fd[0];
            }
        }

        // Esperar a todos los hijos
        for (i = 0; i < command_count; i++) {
            wait(NULL);
        }

        // Reset para el próximo comando
        command_count = 0;
    }
    return 0;
}
