// DemoDBMS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/wait.h>
//#include <unistd.h>
#pragma warning(disable : 4996)
//using namespace std::basic_istream;

using namespace std; 
const int PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100
typedef struct {
    int num_rows;
    void* pages[TABLE_MAX_PAGES];
} Table;

int help(char** args)
{
    int i;
    printf("------------------------Menu help-------------------------- \n");
    printf("clear :  clean the screen \n");
    printf("exit  :  terminate the program --.\n");
    printf("print :  print the database into .txt file --.\n");
    printf("------------------------Menu help-------------------------- \n");
    return 1;
}

int exit(char** args)
{
    return 0;
}
int clear(char** args)
{
    system("cls");
    return 0;
}
// insert 

int insert_table(char** args)
{
    system("cls");
    return 0;
}

int insert_value(char** args)
{
    system("cls");
    return 0;
}


const char* insert_str[] =
{
  "table",
  "into"
};
int (*insert_func[]) (char**) = {
  &insert_table,
  &insert_value,
};
int num_insert() {
    return sizeof(insert_str) / sizeof(char*);
}


int insert(char** args)
{
    int i = 0; 
    for (i = 0; i < num_insert(); i++) {
        if (strcmp(args[1], insert_str[i]) == 0) {
            return (*insert_func[i])(args);
        }
    }
    return 0;
}
int update(char** args)
{
    return 0;
}
int select(char** args)
{
    return 0;
}


/*
  List of builtin commands, followed by their corresponding functions.
 */
const char* builtin_str[] =
{
  "help",
  "exit",
  "select",
  "update",
  "insert",
  "clear"
};

int (*builtin_func[]) (char**) = {
  &help,
  &exit,
  &select,
  &update,
  &insert,
  &clear
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}



#define LSH_RL_BUFSIZE 1024
char* read_line(void)
{
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char* buffer = (char*)malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = (char*)realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}


#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char** split_line(char* line, int & count)
{
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char** tokens = (char **)malloc(bufsize * sizeof(char*));
    char* token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = (char **)realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    count = position; 
    return tokens;
}
int execute(char** args,int &count)
{
    int i;

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    for (i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return 1; 
}

void loop(void)
{
    char* line;
    char** args;
    int status;
    bool check = true; 
    do {
        int count = 0; 
        printf("> ");
        line = read_line();
        args = split_line(line,count);
        status = execute(args,count);
        free(line);
        free(args);
        check = false; 
    } while (check==false);
}

int main()
{
	// Load config files, if any.

  // Run command loop.
	loop();

	// Perform any shutdown/cleanup.

	return EXIT_SUCCESS;
}
