#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 100

char error_message[] = "An error has occurred\n";

char *path_list[100];
int path_count = 1;

void print_error() {
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void init_path() {
    path_list[0] = strdup("/bin");
}

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

        if (input == stdin) {
            printf("wish> ");
        }

        if (getline(&line, &len, input) == -1) {
            exit(0);
        }

        line[strcspn(line, "\n")] = '\0';

        char *commands[100];
        int cmd_count = 0;

        char *line_copy = line;
        char *token;

        while ((token = strsep(&line_copy, "&")) != NULL) {
            if (strlen(token) > 0) {
                commands[cmd_count++] = token;
            }
        }

        int pids[100];
        int pid_count = 0;

        for (int i = 0; i < cmd_count; i++) {

            char *cmd = commands[i];

            char *args[MAX_ARGS];
            int arg_count = 0;

            char *redir_file = NULL;
            int redirect = 0;
            int error = 0;

            char *t;

            while ((t = strsep(&cmd, " \t")) != NULL) {
                if (strlen(t) == 0) continue;

                if (strcmp(t, ">") == 0) {

                    if (redirect) {
                        error = 1;
                        break;
                    }

                    redirect = 1;

                    t = strsep(&cmd, " \t");

                    if (t == NULL || strlen(t) == 0) {
                        error = 1;
                        break;
                    }

                    redir_file = t;

                    // não pode ter mais nada depois
                    char *extra;
                    while ((extra = strsep(&cmd, " \t")) != NULL) {
                        if (strlen(extra) != 0) {
                            error = 1;
                            break;
                        }
                    }

                    break;
                } else {
                    args[arg_count++] = t;
                }
            }

            args[arg_count] = NULL;

            // ERRO: aborta completamente este comando
            if (error) {
                print_error();
                continue;
            }

            if (arg_count == 0) {
                continue;
            }

            // BUILT-INS

            if (strcmp(args[0], "exit") == 0) {
                if (redirect || arg_count != 1) {
                    print_error();
                } else {
                    exit(0);
                }
                continue;
            }

            if (strcmp(args[0], "cd") == 0) {
                if (redirect || arg_count != 2 || chdir(args[1]) != 0) {
                    print_error();
                }
                continue;
            }

            if (strcmp(args[0], "path") == 0) {
                if (redirect) {
                    print_error();
                    continue;
                }

                path_count = 0;
                for (int j = 1; j < arg_count; j++) {
                    path_list[path_count++] = strdup(args[j]);
                }
                continue;
            }

            // EXECUÇÃO

            char *exec_path = find_executable(args[0]);

            if (!exec_path) {
                print_error();
                continue;
            }

            int rc = fork();

            if (rc == 0) {
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
                pids[pid_count++] = rc;
            } else {
                print_error();
            }
        }

        for (int i = 0; i < pid_count; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }

    return 0;
}