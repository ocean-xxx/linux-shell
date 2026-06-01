#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAX_INPUT 1024
struct command {
    char *command_set[100];
    char *inputfile[10];
    char *outputfile[10];
};
//helper for pipe
static void new_string_set(char **string_set[10], char *argv[100], int *argv_count, int *string_set_count, bool *if_invalid_pipe, bool *pipe_background);
static void apply_redirection(struct command *cmd);
static void run_pipe(char **string_set[10], bool pipe_background);
//helper for redirection
void set_redirection_structure(struct command *cmd, char *argv[100], int in_count, int out_count, int command_count, bool *redir_invaild,  bool *redir_background);
static void execute_command(struct command *cmd, bool redir_background);
static void exit_command(char *history[MAX_INPUT]);
/////pipe line/////
//  input = ls -l | grep txt | sort.

//  output = string_set[0] = {"ls", "-l", NULL}
//  string_set[1] = {"grep", "txt", NULL}
//  string_set[2] = {"sort", NULL}
//  string_set[3] = NULL
static void new_string_set(char **string_set[10], char *argv[100], int *argv_count, int *string_set_count, bool *if_invalid_pipe, bool *pipe_background) {
    int string_count = 0;
    char **string = malloc(10 * sizeof(char *));
    for (int i = 0; i < 100; i++) {
        if (argv[*argv_count] == NULL) {
            string[string_count] = NULL;
            string_set[*string_set_count] = string;
            (*string_set_count)++;
            return;
        } else if (strcmp(argv[*argv_count], "|") == 0) {
            string[string_count] = NULL;
            string_set[*string_set_count] = string;
            (*string_set_count)++;
            //skip "|"
            (*argv_count)++;
            break;
        } else if (strcmp(argv[*argv_count], "&") == 0) {
            if (argv[(*argv_count) + 1] == NULL) {
                *pipe_background = true;
                (*argv_count)++;
                string[string_count] = NULL;
                string_set[*string_set_count] = string;
                string_set[(*string_set_count) + 1] = NULL;
                (*string_set_count)++;
                return;
            } else {
                fprintf(stderr, "Invalid background command.\n");
                *if_invalid_pipe = true;
                return;
            }
        } else {
            string[string_count] = argv[*argv_count];
            string_count++;
            (*argv_count)++;
        }
    }
    return;
}

static void run_pipe(char **string_set[10], bool pipe_background) {
    int prev_fd = -1;

    for (int i = 0; string_set[i] != NULL; i++) {
        int fd[2];
        if (string_set[i + 1] != NULL) {
            if (pipe(fd) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            if (prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            if (string_set[i + 1] != NULL) {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }

            execvp(string_set[i][0], string_set[i]);
            perror("execvp");
            exit(1);
        }

        if (prev_fd != -1) {
            close(prev_fd);
        }

        if (string_set[i + 1] != NULL) {
            close(fd[1]);
            prev_fd = fd[0];
        }
    }

    if (!pipe_background) {
        while (wait(NULL) > 0) {
        }
    }
}
/////pipe line/////
/////redirection//////
void set_redirection_structure(struct command *cmd, char *argv[100], int in_count, int out_count, int command_count, bool *redir_invaild, bool *redir_background) {
    int i = 0;
    while (argv[i] != NULL) {
        if (strcmp(argv[i], ">") == 0) {
            if (i == 0) {
                fprintf(stderr, "invalid redirection command\n");
                (*redir_invaild) = true;
                return;
            }
            if ((argv[i + 1] == NULL) || (argv[i - 1] == NULL)) {
                fprintf(stderr, "invalid redirection command\n");
                (*redir_invaild) = true;
                return;
            }
            if (strcmp(argv[i + 1], ">") == 0 || strcmp(argv[i + 1], "<") == 0 ||
             strcmp(argv[i - 1], ">") == 0 || strcmp(argv[i - 1], "<") == 0) {
                fprintf(stderr, "invalid redirection command\n");
                (*redir_invaild) = true;
                return;
             }
            cmd->outputfile[out_count++] = argv[i + 1];
            i += 2;
        } else if (strcmp(argv[i], "<") == 0) {
           if (i == 0) {
                fprintf(stderr, "invalid redirection command\n");
                (*redir_invaild) = true;
                return;
            }
            if ((argv[i + 1] == NULL) || (argv[i - 1] == NULL)) {
                fprintf(stderr, "invalid redirection command\n");
                (*redir_invaild) = true;
                return;
            }
            if (strcmp(argv[i + 1], ">") == 0 || strcmp(argv[i + 1], "<") == 0 ||
             strcmp(argv[i - 1], ">") == 0 || strcmp(argv[i - 1], "<") == 0) {
                fprintf(stderr, "invalid redirection command\n");
                (*redir_invaild) = true;
                return;
            }
            cmd->inputfile[in_count++] = argv[i + 1];
            i += 2;
        } else if (strcmp(argv[i], "&") == 0) {
            if (argv[i + 1] == NULL) {
                *redir_background = true;
                break;
            } else {
                fprintf(stderr, "Invalid background command.\n");
                *redir_invaild = true;
                return;
            }
        } else {
            cmd->command_set[command_count++] = argv[i];
            i++;
        }
    }
    cmd->inputfile[in_count] = NULL;
    cmd->outputfile[out_count] = NULL;
    cmd->command_set[command_count] = NULL;
}

static void apply_redirection(struct command *cmd) {
    for (int i = 0; cmd->inputfile[i] != NULL; i++) {
        int fd = open(cmd->inputfile[i], O_RDONLY);

        if (fd == -1) {
            perror(cmd->inputfile[i]);
            exit(1);
        }

        if (cmd->inputfile[i + 1] == NULL) {
            dup2(fd, STDIN_FILENO);
        }

        close(fd);
    }

    for (int i = 0; cmd->outputfile[i] != NULL; i++) {
        int fd = open(cmd->outputfile[i],  O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd == -1) {
            perror(cmd->outputfile[i]);
            exit(1);
        }

        if (cmd->outputfile[i + 1] == NULL) {
            dup2(fd, STDOUT_FILENO);
        }
        
        close(fd);
    }
}

static void execute_command(struct command *cmd, bool redir_background) {
    if (cmd->command_set[0] == NULL) {
        fprintf(stderr, "invalid redirection command\n");
        return;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        apply_redirection(cmd);
        execvp(cmd->command_set[0], cmd->command_set);
        perror(cmd->command_set[0]);
        exit(1);
    }

    if (!redir_background) {
        waitpid(pid, NULL, 0);
    }
}
//exit command
static void exit_command(char *history[MAX_INPUT]) {
    int i = 0;
    while (history[i] != NULL) {
        free(history[i]);
        history[i] = NULL;
        i++;
    }
    exit(1);
}