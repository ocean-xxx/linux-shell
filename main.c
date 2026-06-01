#include "shell.h"
int main (void) {
    char input[MAX_INPUT];
    char *history[MAX_INPUT] = {NULL};
    int num_hty = 0;
    while (1) {
        printf("mysh> ");
        // ctrl + D = EOF or press enter will activate fgets();
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            exit_command(history);
        }
        bool pipe_background = false;
        bool redir_background = false;
        bool general_background = false;
        //copy string set to history
        if (input[0] != '\n') {
            int his_copy_count = strlen(input);
            history[num_hty] = malloc(sizeof(char) * his_copy_count);
            strcpy(history[num_hty], input);
            num_hty++;
        }
        //
        input[strcspn(input, "\n")] = '\0';
        //press enter without typing anything
        if (strlen(input) == 0) {
            continue;
        } else if (strcmp(input, "exit") == 0) {
            exit_command(history);
        }
        //parsing
        char *argv[100];
        int num = 0;
        char *token = strtok(input, " ");

        while (token != NULL) {
            argv[num++] = token;
            token = strtok(NULL, " ");
        }
        argv[num] = NULL;
        //add to history
        if (strcmp(argv[0], "history") == 0) {
            int history_output = 0;
            while (history[history_output] != NULL) {
                fputs(history[history_output++], stdout);
            }
            continue;
        }
        //check if is cd command
        if (!strcmp(argv[0], "cd")) {
            if ((argv[1] == NULL) || !strcmp(argv[1], "~")) {
                chdir(getenv("HOME"));
            } else {
                if (chdir(argv[1]) != 0) {
                    perror("cd failed");
                }
            }
            continue;
        }
        //check if pipe()
        bool if_pipe = false;
        for (int i = 0; argv[i] != NULL; i++) {
            if (strcmp(argv[i], "|") == 0) {
                if_pipe = true;
                break;
            }
        }
        /// do pipe
        if (if_pipe) {
            char **string_set[10];
            int argv_count = 0;
            int string_set_count = 0;
            bool if_invalid_pipe = false;
            while (argv[argv_count] != NULL) {
                new_string_set(string_set, argv, &argv_count, &string_set_count, &if_invalid_pipe, &pipe_background);
            }
            string_set[string_set_count] = NULL;
            if (!if_invalid_pipe) {
                run_pipe(string_set, pipe_background);
            }
            //最後要free_string_set
            for (int i = 0; i < string_set_count; i++) {
                free(string_set[i]);
            }
            continue;
        }
        /////////redirection/////////
        //if is redirection//
        bool if_redirection = false;
        for (int i = 0; argv[i] != NULL; i++) {
            if ((strcmp(argv[i], ">") == 0) || (strcmp(argv[i], "<") == 0)) {
                if_redirection = true;
                break;
            }
        }
        if (if_redirection) {
            bool redir_invaild = false;
            struct command cmd;
            set_redirection_structure(&cmd, argv, 0, 0, 0, &redir_invaild, &redir_background);
            if (redir_invaild) {
                continue;
            }
            execute_command(&cmd, redir_background);
            continue;
        }
        /////////////////////////////
        // general commend
        if (strcmp(argv[num - 1], "&") == 0) {
            general_background = true;
            argv[num - 1] = NULL;
        }
        pid_t pid = fork();

        if (pid == 0) {
            // child process
            execvp(argv[0], argv);
            // only runs if exec fails
            perror("exec failed");
            exit(1);
        } else {
            // parent process
            if (!general_background) {
                waitpid(pid, NULL, 0);
            }
        }
    }
    return 0;
}