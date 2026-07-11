/*
 * shell.c - My Custom Shell v2.0
 * Features:
 * - Command execution
 * - Built-in commands (cd, exit, help)
 * - Output redirection (echo hello > file.txt)
 * - Input redirection  (sort < file.txt)
 * - Pipes             (ls | findstr shell)
 *
 * Compile: gcc shell.c -o shell
 * Run:     ./shell
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

#define MAX_INPUT 1024
#define MAX_ARGS  64

/* ─────────────────────────────────────────
   STEP 1 - Read input from user
   ───────────────────────────────────────── */
void read_input(char *input) {
    printf("\nmyshell> ");
    fflush(stdout);

    if (fgets(input, MAX_INPUT, stdin) == NULL) {
        printf("\nGoodbye!\n");
        exit(0);
    }

    int len = strlen(input);
    if (len > 0 && input[len-1] == '\n')
        input[len-1] = '\0';
}

/* ─────────────────────────────────────────
   STEP 2 - Parse input into tokens
   ───────────────────────────────────────── */
int parse_input(char *input, char **args) {
    int count = 0;
    char *token = strtok(input, " ");

    while (token != NULL && count < MAX_ARGS - 1) {
        args[count++] = token;
        token = strtok(NULL, " ");
    }
    args[count] = NULL;
    return count;
}

/* ─────────────────────────────────────────
   STEP 3 - Handle built-in commands
   ───────────────────────────────────────── */
int handle_builtin(char **args) {
    if (strcmp(args[0], "exit") == 0) {
        printf("Goodbye! Keep coding!\n");
        exit(0);
    }

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            _chdir(getenv("USERPROFILE"));
        } else {
            if (_chdir(args[1]) != 0)
                printf("cd: cannot find: %s\n", args[1]);
        }
        return 1;
    }

    if (strcmp(args[0], "help") == 0) {
        printf("\n================================\n");
        printf("     My Shell v2.0 - Help\n");
        printf("================================\n");
        printf("Built-in commands:\n");
        printf("  cd [dir]      change directory\n");
        printf("  exit          quit the shell\n");
        printf("  help          show this menu\n");
        printf("\nSpecial features:\n");
        printf("  cmd > file    save output to file\n");
        printf("  cmd < file    read input from file\n");
        printf("  cmd1 | cmd2   pipe output to next cmd\n");
        printf("\nCommands to try:\n");
        printf("  ls            list files\n");
        printf("  echo [msg]    print a message\n");
        printf("  whoami        show your username\n");
        printf("================================\n\n");
        return 1;
    }

    return 0;
}

/* ─────────────────────────────────────────
   STEP 4 - Check for redirection or pipes
   and build the final command accordingly
   ───────────────────────────────────────── */
void execute_command(char **args) {
    char command[MAX_INPUT] = "";
    char output_file[256] = "";
    char input_file[256] = "";
    char pipe_cmd[MAX_INPUT] = "";
    int has_output = 0;
    int has_input = 0;
    int has_pipe = 0;

    /* Scan args for >, <, | */
    int i = 0;
    while (args[i] != NULL) {

        /* Output redirection: echo hello > file.txt */
        if (strcmp(args[i], ">") == 0) {
            has_output = 1;
            if (args[i+1] != NULL)
                strcpy(output_file, args[i+1]);
            args[i] = NULL;   /* cut the command here */
            break;
        }

        /* Input redirection: sort < file.txt */
        else if (strcmp(args[i], "<") == 0) {
            has_input = 1;
            if (args[i+1] != NULL)
                strcpy(input_file, args[i+1]);
            args[i] = NULL;
            break;
        }

        /* Pipe: ls | findstr shell */
        else if (strcmp(args[i], "|") == 0) {
            has_pipe = 1;
            /* collect everything after | as the second command */
            int j = i + 1;
            while (args[j] != NULL) {
                strcat(pipe_cmd, args[j]);
                if (args[j+1] != NULL)
                    strcat(pipe_cmd, " ");
                j++;
            }
            args[i] = NULL;
            break;
        }

        i++;
    }

    /* Build the base command from remaining args */
    int k = 0;
    while (args[k] != NULL) {
        strcat(command, args[k]);
        if (args[k+1] != NULL)
            strcat(command, " ");
        k++;
    }

    /* Build final command with redirection/pipe */
    char final_command[MAX_INPUT * 2] = "";

    if (has_output) {
        /* echo hello > file.txt */
        sprintf(final_command, "%s > %s", command, output_file);
        printf("Output saved to: %s\n", output_file);
    }
    else if (has_input) {
        /* sort < file.txt */
        sprintf(final_command, "%s < %s", command, input_file);
    }
    else if (has_pipe) {
        /* ls | findstr shell */
        sprintf(final_command, "%s | %s", command, pipe_cmd);
    }
    else {
        /* normal command */
        strcpy(final_command, command);
    }

    int result = system(final_command);
    if (result != 0 && !has_output)
        printf("Command failed: %s\n", args[0]);
}

/* ─────────────────────────────────────────
   MAIN - The shell loop
   ───────────────────────────────────────── */
int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    printf("================================\n");
    printf("   Welcome to My Shell v2.0!\n");
    printf("   Built by Kavyansh in C\n");
    printf("================================\n");
    printf("Type 'help' to see commands.\n");
    printf("Type 'exit' to quit.\n");

    while (1) {
        read_input(input);

        if (strlen(input) == 0)
            continue;

        char input_copy[MAX_INPUT];
        strcpy(input_copy, input);

        int count = parse_input(input_copy, args);

        if (count == 0)
            continue;

        if (handle_builtin(args))
            continue;

        execute_command(args);
    }

    return 0;
}