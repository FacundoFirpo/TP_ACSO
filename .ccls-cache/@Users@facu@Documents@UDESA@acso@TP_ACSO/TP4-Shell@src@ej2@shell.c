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
        
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        // Si el usuario escribe "q", salir del shell
        if (strcmp(command, "q") == 0) {
            break;
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
                char *argv[64];
                parse_args(commands[i], argv);

                execvp(argv[0], argv);
                perror("execvp");
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
