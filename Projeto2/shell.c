#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 100

char error_message[30] = "An error has occurred\n";

// PATH global
char *path_list[100];
int path_count = 1;

// imprime erro padrão
void print_error() {
    write(STDERR_FILENO, error_message, strlen(error_message));
}

// inicializa PATH
void init_path() {
    path_list[0] = strdup("/bin");
}

// busca executável no PATH
char *find_executable(char *cmd) {
    static char fullpath[256];

    for (int i = 0; i < path_count; i++) {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path_list[i], cmd);
        if (access(fullpath, X_OK) == 0) {
            return fullpath;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    FILE *input = stdin;

    // modo batch
    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            print_error();
            exit(1);
        }
    } else if (argc > 2) {
        print_error();
        exit(1);
    }

    init_path();

    char *line = NULL;
    size_t len = 0;

    while (1) {

        // prompt apenas no modo interativo
        if (input == stdin) {
            printf("wish> ");
        }

        if (getline(&line, &len, input) == -1) {
            exit(0);
        }

        // remover newline
        line[strcspn(line, "\n")] = '\0';

        // dividir comandos paralelos (&)
        char *commands[100];
        int cmd_count = 0;

        char *token;
        while ((token = strsep(&line, "&")) != NULL) {
            if (strlen(token) > 0) {
                commands[cmd_count++] = token;
            }
        }

        int pids[100];

        for (int i = 0; i < cmd_count; i++) {

            char *cmd = commands[i];

            // parsing argumentos
            char *args[MAX_ARGS];
            int arg_count = 0;

            char *redir_file = NULL;
            int redirect = 0;

            char *t;
            while ((t = strsep(&cmd, " \t")) != NULL) {
                if (strlen(t) == 0) continue;

                if (strcmp(t, ">") == 0) {
                    redirect = 1;
                    t = strsep(&cmd, " \t");
                    if (t == NULL) {
                        print_error();
                        continue;
                    }
                    redir_file = t;
                } else {
                    args[arg_count++] = t;
                }
            }
            args[arg_count] = NULL;

            if (arg_count == 0) continue;

            // 🔹 BUILT-INS
            if (strcmp(args[0], "exit") == 0) {
                if (arg_count != 1) {
                    print_error();
                } else {
                    exit(0);
                }
                continue;
            }

            if (strcmp(args[0], "cd") == 0) {
                if (arg_count != 2 || chdir(args[1]) != 0) {
                    print_error();
                }
                continue;
            }

            if (strcmp(args[0], "path") == 0) {
                path_count = 0;
                for (int j = 1; j < arg_count; j++) {
                    path_list[path_count++] = strdup(args[j]);
                }
                continue;
            }

            // 🔹 EXECUÇÃO NORMAL
            char *exec_path = find_executable(args[0]);
            if (!exec_path) {
                print_error();
                continue;
            }

            int rc = fork();
            if (rc == 0) {
                // filho

                if (redirect) {
                    int fd = open(redir_file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
                    if (fd < 0) {
                        print_error();
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }

                execv(exec_path, args);
                print_error();
                exit(1);
            } else if (rc > 0) {
                pids[i] = rc;
            } else {
                print_error();
            }
        }

        // esperar TODOS (paralelismo)
        for (int i = 0; i < cmd_count; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }

    return 0;
}