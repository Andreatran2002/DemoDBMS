#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#pragma warning(disable : 4996)

using namespace std; 




struct Item {
    //int order; ///thứ tự từ bên phải qua của cột
    //dữ liệu ở đây
    string data; 
};

struct Column {
    string Name;
    string Type;
    bool PK = false; 
    bool FK = false; 
    vector<string> Items; 
    
    //cac thong tin phu tro khac nhu PK, FK, NOT NULL, DEFAULT, REFERENCES, ..
};

struct Table {
    string Name; 
    vector<Column> Columns; 
};
struct Database {
    string Name="";
    vector<Table> Tables; 
};

vector<Database> databases; 

Database database_use; 


int help(char** args, int& count)
{
    int i;
    printf("------------------------Menu help-------------------------- \n");
    printf("clear :  clean the screen \n");
    printf("exit  :  terminate the program --.\n");
    printf("print :  print the database into .txt file --.\n");
    printf("------------------------Menu help-------------------------- \n");
    return 1;
}

int exit(char** args, int& count)
{
    return 0;
}
int clear(char** args, int& count)
{
    system("cls");
    return 0;
}

int show(char** args, int& count)
{
    if (strcmp(args[1], "database") == 0) {
        if (databases.size() == 0)
        {
            cout << "You haven't entered any database" << endl;
            return 1;
        }
        printf("List database : \n"); 
        for (int i =  0 ; i< databases.size(); i++)
        {
            cout <<  databases[i].Name<< " \n";
        }
    }
    else if (strcmp(args[1], "table") == 0){
        if (database_use.Tables.size() == 0)
        {
            cout << "You haven't entered any table in curren database" << endl; 
            return 1; 
        }
        printf("List table in current database: \n");
        for (int i = 0; i < database_use.Tables.size(); i++)
        {
            cout << database_use.Tables[i].Name << " \n";
        }
    }
    else {

    }
    
     return 1;
}
int use(char** args, int& count)
{
    for (int i = 0; i < databases.size(); i++)
    {
        if (args[1] == databases[i].Name) {
            database_use = databases[i];
            cout << "You are in database " + databases[i].Name<< "\n";
            return 1; 
        }
    }
    cout << "Wrong format . Please try another query ." << endl;
    return 1;
}



int create_database(char** args, int count) {
    if (count > 3) {
        cout << "Wrong format . Please try another query . " << endl;
        return 1;
    }
    Database database;
    database.Name = args[2];
    databases.push_back(database);
    cout << "You have create a database "<< database.Name<< " successfully.\n"; 
    
    return 1; 
}

int create_table(char** args, int count)
{
    if (database_use.Name == "") {
        cout << "You have to use a database" << endl;
        return 1; 
    }
    Table table; 
    vector<Column> columns; 
    table.Name = args[2]; 
    int i_tail = 3;
    for (int i = 3; i < count; i++ ) {
        if (strcmp(args[i], ",") == 0 ) {
            Column column; 
            column.Name = args[i_tail + 1];
            column.Type = args[i_tail + 2];

            if (i - i_tail - 1 == 4) {
                if (strcmp(args[i_tail + 3],"primary"))
                column.PK = true; 
                else column.FK = true;
            }

            columns.push_back(column);
            i_tail = i;

            
        }
        else if (strcmp(args[i], ")") ==0 ) {

            Column column;
            column.Name = args[i_tail + 1];
            column.Type = args[i_tail + 2];
            if (i - i_tail - 1 == 4) {
                if (strcmp(args[i_tail + 3], "primary"))
                    column.PK = true;
                else column.FK = true;
            }
            columns.push_back(column);
        }
    }
    table.Columns = columns;
    /*for(int j = 0; j<table.Columns.size();j++) {
        cout << table.Columns[j].Name + "\n"; 
    }*/
    cout << "You have created table successfully"; 
    database_use.Tables.push_back(table);
    return 1;
}
//INSERT INTO titles  
//(title_id, title, type, pub_id, price)
//VALUES('BU9876', 'Creating Web Pages', 'business', '1389', '29.99')

int create_value(char** args, int count)
{
    int index = 0; 
    for (int i = 0; i < database_use.Tables.size(); i++)
    {
        if (args[2]== database_use.Tables[i].Name) {
            if (strcmp(args[3], "values" ) == 0 ) {
                vector<string> items;
                    int k = 0; 
                    string n = "";
                        for (int jj = 5; jj < count; jj++) {
                        if (strcmp(args[jj], "'") == 0)
                        {
                            if (k != 0 && k % 2 == 0 ) {
                                items.push_back(n);
                                n = "";
                            }
                            k++;
                        }
                        if (strcmp(args[jj], ")") != 0 && strcmp(args[jj], ",") != 0 && strcmp(args[jj], "'") != 0 ) n = n + " " +  args[jj];
                        
                    }
                    if (k %2 == 0 )items.push_back(n);
                    if (items.size() == database_use.Tables[i].Columns.size()) {
                        for (int kk = 0; kk < items.size(); kk++) {
                            database_use.Tables[i].Columns[index++].Items.push_back(items[kk]); 
                        }
                    }
                    else {
                        cout << "Wrong format . Please try another query . " << endl;
                        return 1;
                    }
                    cout << "You have enter a value successfully ! " << endl; 
                    break; 
                }
                else {
                    cout << "Wrong format . Please try another query . " << endl; 
                    return 1; 
                }
            
        }
    }
    
    return 1;
}


const char* create_str[] =
{
  "database",
  "table",
  "new"
};
int (*create_func[]) (char**, int) = {
    &create_database,
  &create_table,
  &create_value,
};
int num_create() {
    return sizeof(create_str) / sizeof(char*);
}


int create(char** args, int& count)
{
    int i = 0; 
    for (i = 0; i < num_create(); i++) {
        if (strcmp(args[1], create_str[i]) == 0) {
            return (*create_func[i])(args,count);
        }
    }
    return 1;
}
int update(char** args, int& count)
{
    return 1;
}
int select(char** args,int& count)
{
    if (strcmp(args[1], "*") == 0) {
        if (strcmp(args[2], "from") == 0) {
            for (int i = 0; i < database_use.Tables.size(); i++) {
                if (args[3] == database_use.Tables[i].Name) {
                    for (int j = 0; j < database_use.Tables[i].Columns[0].Items.size(); j++) {
                        for (int jj = 0; jj < database_use.Tables[i].Columns.size(); jj++) {
                            cout << database_use.Tables[i].Columns[jj].Items[j] << " ";
                        }
                        cout << endl; 
                    }
                    break; 
                }
            }
        }
    }/*
    else {
        string n = "";
        vector<string> field; 
        for (int i = 1; i < count; i++) {
            n = n + " " + args[i];
            if (strcmp(args[i], "from") == 0) {
                for (int i)
            }
        }
    }*/
    return 1;
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
  "create",
  "clear",
  "show",
  "use"
};

int (*builtin_func[]) (char**,int&) = {
  &help,
  &exit,
  &select,
  &update,
  &create,
  &clear,
  &show,
  &use
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
            return (*builtin_func[i])(args,count);
        }
    }

    return 1; 
}

void loop()
{
    char* line;
    char** args;
    int status;
    bool check = true; 
    do {
        int count = 0; 
        cout<< database_use.Name<< " >> ";
        line = read_line();
        args = split_line(line,count);
        status = execute(args,count);
        free(line);
        free(args);
        check = false; 
    } while (check==false);
}


void initialize() {
    database_use.Name = "";
}


int main()
{
	// Load config files, if any.

  // Run command loop.
    initialize(); 
	loop();

	// Perform any shutdown/cleanup.

	return EXIT_SUCCESS;
}
