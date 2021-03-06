#include <iostream>
#include <string> 
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <conio.h>
#include <Windows.h>
#include <cstdlib>
#include <cstring>
#include <list>
#include <sstream>
#include <fstream>

//====header================
#include "Hash.h"
#pragma warning(disable : 4996)

using namespace std;

struct DataBaseTable {
	vector<string>key_tb;
	vector<string>value_tb;
	void add(string k, string v) {
		key_tb.push_back(k);
		value_tb.push_back(v);
	}
};
bool output(DataBaseTable& table, string tb_key_name) {
	ofstream out(tb_key_name, ios::binary);
	if (!out)
		return false;
	size_t vsize = table.key_tb.size();
	out.write(reinterpret_cast<char*>(&vsize), sizeof vsize);
	for (size_t i = 0; i < vsize; i++) {
		size_t size = table.key_tb[i].size();
		out.write(reinterpret_cast<char*>(&size), sizeof size);
		string p = table.key_tb[i];
		out.write(p.c_str(), table.key_tb[i].size());
	}
	size_t vsizev = table.value_tb.size();
	out.write(reinterpret_cast<char*>(&vsizev), sizeof vsizev);
	for (size_t i = 0; i < vsizev; i++) {
		size_t size = table.value_tb[i].size();
		out.write(reinterpret_cast<char*>(&size), sizeof size);
		string p = table.value_tb[i];
		out.write(p.c_str(), table.value_tb[i].size());
	}
	return true;
}
bool input(DataBaseTable& table, string tb_key_name) {
	ifstream in(tb_key_name, ios::binary);
	if (!in)
		return false;

	size_t vsize = 0;
	in.read(reinterpret_cast<char*>(&vsize), sizeof vsize);

	for (size_t i = 0; i < vsize; i++) {
		size_t size = 0;
		in.read(reinterpret_cast<char*>(&size), sizeof size);
		char* str = new char[size + 1];
		in.read(str, size);
		str[size] = '\0';
		table.key_tb.push_back(str);
		delete[] str;
	}
	size_t vsizev = 0;
	in.read(reinterpret_cast<char*>(&vsizev), sizeof vsizev);

	for (size_t i = 0; i < vsizev; i++) {
		size_t size = 0;
		in.read(reinterpret_cast<char*>(&size), sizeof size);
		char* str = new char[size + 1];
		in.read(str, size);
		str[size] = '\0';
		table.value_tb.push_back(str);
		delete[] str;
	}
	return true;
}
#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
string DATA_TYPE[3] = { "string", "number", "boolean" };
HashMapTable hash;
string currDB = "";
bool readed = false;
//=====init function ========
//void PrintTable(int i);
void SetCursor(int x, int y);
int help(char** args, int& count);
int exit(char** args, int& count);
int clear(char** args, int& count);
int show(char** args, int& count);
int use(char** args, int& count);
int save(char** args, int& count);
int read(char** args, int& count);
int create_database(char** args, int count);
int create_table(char** args, int count);
int create_value(char** args, int count);
int create(char** args, int& count);
int update(char** args, int& count);
int select(char** args, int& count);
int num_builtins();
char* read_line(void);
char** split_line(char* line, int& count);
int execute(char** args, int& count);
int remove(char** args, int& count);
void loop();
int GetLength(string a) {
	int result = 0;
	int index = 0;
	for (int i = 0; i < a.length(); i++) {
		if (a[i] == ' ') {
			return (int)a[i + 1] - 48;
		}
	}
	return -1;
}
const char* builtin_str[] =
{
  "help",
  "exit",
  "select",
  "update",
  "create",
  "clear",
  "show",
  "use",
  "save",
  "read",
  "remove",
};
int (*builtin_func[]) (char**, int&) = {
  &help,
  &exit,
  &select,
  &update,
  &create,
  &clear,
  &show,
  &use,
  &save,
  &read,
  &remove,
};

int num_builtins() {
	return sizeof(builtin_str) / sizeof(char*);
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
char** split_line(char* line, int& count)
{
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char** tokens = (char**)malloc(bufsize * sizeof(char*));
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
			tokens = (char**)realloc(tokens, bufsize * sizeof(char*));
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


int execute(char** args, int& count);



//=====Main=================================
int main()
{
	cout << "   _____ ____    __    ____  ____  ____ " << endl;
	cout << "  / ___// __ |  / /   / __ |/ __ |/ __ |" << endl;
	cout << "   __  / / / / / /   / / / / / / / /_/ /" << endl;
	cout << " ___/ / /_/ / / /___/ /_/ / /_/ / ____/ " << endl;
	cout << "/____/|___|_|/_____/|____/|____/_/      " << endl;
	cout << "\nType 'help' to see more... \n";
	loop();
	return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*                                    loop                                    */
/* -------------------------------------------------------------------------- */

void loop()
{
	char* line;
	char** args;
	int status;
	bool check = true;
	do {
		int count = 0;
		cout << currDB << " >> ";
		line = read_line();
		args = split_line(line, count);
		status = execute(args, count);
		free(line);
		free(args);
		check = false;
	} while (check == false);
}

int create_database(char** args, int count) {
	if (count > 3) {
		cout << "[ERROR] Wrong format . Please try another query ." << endl;
		return 1;
	}
	string key;
	string value = "";
	key = args[2];
	::hash.Insert(key, value);
	cout << "[STATUS] [SUCCESS] You have create a database " << key << " successfully.\n" << endl;
	return 1;
}

int create_table(char** args, int count)
{
	if (currDB == "") {
		cout << "[STATUS] [SUCCESS] You have to use a database" << endl;
		return 1;
	}
	string tbKey = currDB + "_" + args[2];
	string tbValue = "";
	string dbValue = ::hash.SearchKey(currDB) + " " + tbKey;
	::hash.Update(currDB, dbValue);
	::hash.Insert(tbKey, tbValue);

	for (int i = 3; i < count; i++) {
		if (strcmp(args[i], ":") == 0) {
			string colKey = currDB + "_" + args[2] + "_" + args[i - 1];
			string dataType = "null";
			if (strcmp(args[i + 1], "string") == 0)
				dataType = "0";
			if (strcmp(args[i + 1], "number") == 0)
				dataType = "1";
			if (strcmp(args[i + 1], "boolean") == 0)
				dataType = "2";
			if (dataType == "null") {
				cout << "[STATUS] [ERROR] The data type \"";
				cout << args[i + 1];
				cout << "\" is not supported !!" << endl;
			}
			else {
				::hash.Update(tbKey, ::hash.SearchKey(tbKey) + " " + colKey);
				::hash.Insert(colKey, dataType + " 0");
			}
		}
	}

	cout << "[STATUS] [SUCCESS] You have created table successfully" << endl;
	cout << "[STATUS] [WARNING] You should enter a new value with enough columns. If empty value, enter ' null ' to avoid errors in the system" << endl << endl;
	return 1;
}

//=====create value=========================
//create new tb name : Vy id : 20110735
/*
create database db1
use db1
create table tb1 name : string id : number oke : oke male : boolean lastname : string
create new tb1 name : ' Phuong Vy ' id : ' 20110735 ' male : ' true ' lastname : ' VyVy '

*/

int create_value(char** args, int count)
{
	if (count <= 5) {
		cout << "[ERROR] Wrong format . Please try another query." << endl;
		return 1;
	}
	string tbKey = currDB + "_" + args[2];
	string tbValue = ::hash.SearchKey(tbKey);
	if (tbValue == "[empty]") {
		cout << "[STATUS] [ERROR] table \"" << args[2] << "\" has not been created yet." << endl;
		return 1;
	}
	for (int i = 3; i < count; i++) {
		if (strcmp(args[i], ":") == 0) {
			string colKey = currDB + "_" + args[2] + "_" + args[i - 1];
			string colValue = ::hash.SearchKey(colKey);
			string dataType(1, colValue[0]);
			char index = colValue[2];
			int length = (int)index - 47;
			if (colValue == "[empty]") {
				cout << "[STATUS] [FAIL] the key doesn't exists yet!!" << endl;
			}
			else {
				string rowKey = colKey + "_" + index;
				string rowValue = "";
				if (strcmp(args[i + 1], "'") == 0) {
					for (int x = i + 2; x < count; x++) {
						if (strcmp(args[x], "'") == 0) {
							break;
						}
						else {
							rowValue = rowValue + args[x] + " ";
						}
					}
				}
				colValue[2] = '0' + length;
				::hash.Update(colKey, colValue);
				::hash.Insert(rowKey, rowValue);
			}
		}
	}
	cout << endl;
	return 1;
}


int execute(char** args, int& count)
{
	int i;
	if (args[0] == NULL) {
		// An empty command was entered.
		return 1;
	}
	for (i = 0; i < num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args, count);
		}
	}

	return 1;
}


int help(char** args, int& count)
{
	printf("------------------------Menu help--------------------------------------------------------------------- \n");
	printf("clear                                                   :  clean the screen \n");
	printf("exit                                                    :  terminate the program \n");
	printf("save <table>                                            :  save the database table to binary file\n");
	printf("read <database>_<table>	                                :  read the database table file\n");
	printf("create database <database>                              :  create new database\n");
	printf("use <database>                                          :  use new table \n");
	printf("create table <table>  <column> : <type>                 :  create new table \n");
	printf("create new <table>  <column> : ' <value> '              :  create new value \n");
	printf("update <table> <index> <column> : ' <updateValue> '     :  update new value \n");
	printf("remove <table> <index>                                  :  remove value \n");
	printf("select <column1> <column2> from <table>	                :  select columns from a table --.\n");

	printf("\nExample:\ncreate database db1\nuse db1\ncreate table tb1 name : string id : number phone : number\ncreate new tb1 name : ' Phuong Vy ' id : ' 20110735 ' phone : ' 0788892441 '\ncreate new tb1 name : ' Pham Nhat Tien ' id : ' 20110735 ' phone : ' 0394973287 '\ncreate new tb1 name : ' Loc ' id : '1311222' phone : ' 039292932 '\ncreate new tb1 name : ' Tien dep tai ' id : ' 1 ' phone : ' 0785984392 '\nshow tb1\n");
	printf("------------------------Menu help---------------------------------------------------------------------- \n");
	return 1;
}

int exit(char** args, int& count)
{
	cout << "Press [ENTER] to exit";
	char key_press;
	int ascii_value;
	while (1)
	{
		key_press = getch();
		ascii_value = key_press;
		if (ascii_value == 13) {
			exit(EXIT_SUCCESS);
			break;
		}
		else {
			cout << endl;
			return 1;
		}
	}
}
/* -------------------------------------------------------------------------- */
/*                                xóa màn hình                                */
/* -------------------------------------------------------------------------- */
int clear(char** args, int& count)
{
	system("cls");
	return 0;
}

/* -------------------------------------------------------------------------- */
/*                                lưu vào file                                */
/* -------------------------------------------------------------------------- */
int save(char** args, int& count) {
	DataBaseTable tb1;
	if (currDB == "") {
		cout << "[STATUS] [SUCCESS] You have to use a database to save" << endl;
		return 1;
	}
	string tbKey = currDB + "_" + args[1];
	/*tb1.tbKey = tbKey;*/
	string tbValue = ::hash.SearchKey(tbKey);
	/*tb1.tbValue = tbValue;*/
	tb1.add(tbKey, tbValue);
	if (tbValue == "[empty]") {
		cout << "[STATUS] [ERROR] table \"" << args[1] << "\" doesn't existes yet." << endl << endl;
		return 1;
	}
	for (int i = 0; i < ST; i++) {
		Node* en = ::hash.HT[i];
		while (en != NULL) {
			tb1.add(en->key, en->value);
			en = en->next;
		}
	}
	if (!output(tb1, tbKey)) {
		cout << "Error writing file.\n";
		return 1;
	}
	cout << "File save" << endl;
	return 1;
}

//======show================================
/*
create database db1
use db1
create table tb1 name : string id : number phone : number
create new tb1 name : ' Phuong Vy ' id : ' 20110735 ' phone : ' 0788892441 '
create new tb1 name : ' Pham Nhat Tien ' id : ' 20110735 ' phone : ' 0394973287 '
create new tb1 name : ' Loc ' id : '1311222' phone : ' 039292932 '
create new tb1 name : ' Tien dep tai ' id : ' 1 ' phone : ' 0785984392 '
show tb1

*/
//show tb1 
/* -------------------------------------------------------------------------- */
/*                                 Hàm update                                 */
/* -------------------------------------------------------------------------- */
int update(char** args, int& count) {

	if (count <= 4) {
		cout << "[STATUS] [ERROR] Wrong format . Please try another query." << endl;
		return 1;
	}
	string tbKey, tbValue;
	tbKey = currDB + "_" + args[1];
	tbValue = ::hash.SearchKey(tbKey);

	if (tbValue == "[empty]") {
		cout << "[STATUS] [ERROR] table \"" << args[1] << "\" doesn't existes yet." << endl << endl;
		return 1;
	}

	for (int i = 0; i < count; i++) {
		if (strcmp(":", args[i]) == 0) {
			string valueKey = currDB + "_" + args[1] + "_" + args[i - 1] + "_" + args[2];
			string valueValue = ::hash.SearchKey(valueKey);
			string updateValue = "";

			if (strcmp(args[i + 1], "'") == 0) {
				for (int x = i + 2; x < count; x++) {
					if (strcmp(args[x], "'") == 0) {
						break;
					}
					else {
						updateValue = updateValue + args[x] + ' ';
					}
				}
			}
			::hash.Update(valueKey, updateValue);
		}
	}

	return 1;
}

/* -------------------------------------------------------------------------- */
/*                                  hiển thị                                  */
/* -------------------------------------------------------------------------- */
int show(char** args, int& count)
{
	if (count < 2) {
		cout << "[STATUS] [ERROR] Wrong format . Please try another query." << endl;
		return 1;
	}
	string tbKey, tbValue;
	tbKey = currDB + "_" + args[1];
	tbValue = ::hash.SearchKey(tbKey);

	if (tbValue == "[empty]") {
		cout << "[STATUS] [ERROR] table \"" << args[1] << "\" doesn't existes yet." << endl << endl;
		return 1;
	}
	system("cls");
	//string to char
	int countTb = 0;
	char** tbList;
	//const char* line = tbValue.c_str();
	char* line = new char[tbValue.size() + 1];
	strcpy(line, tbValue.c_str());
	//string to char8
	tbList = split_line(line, countTb);
	string a = ::hash.SearchKey(tbList[0]);
	int lengthi = (int)a[2] - 48;
	int baseHeight = countTb * lengthi + 3;
	int lengthOut = 0;
	for (int i = 0; i < countTb; i++) {
		SetCursor(i * 30, baseHeight);
		cout << "|" << tbList[i];
		SetCursor(0, i * lengthi);
		string tbValue = ::hash.SearchKey(tbList[i]);
		int length = (int)tbValue[2] - 48;
		lengthOut = length;
		for (int x = 0; x < length; x++) {
			//cout << x << endl;
			char index = '0' + x;
			string stra(tbList[i]);
			string rowKey = stra + "_" + index;
			SetCursor(0, i * lengthi + x);
			string rowValue = ::hash.SearchKey(rowKey);
			SetCursor(i * 30, baseHeight + x + 2);
			cout << "|" << rowValue;
		}
	}

	for (int i = 0; i < lengthOut + 2; i++) {
		SetCursor(countTb * 30, baseHeight + i);
		if (i == 0)
			cout << "| [Column] ";
		else if (i == 1)
			cout << "|";
		else
			cout << "| [" << i - 2 << "]";
	}
	for (int i = 0; i < countTb * 30; i++) {
		SetCursor(i, baseHeight + 1);
		if (i % 30 == 0)
			cout << "+";
		else
			cout << "-";
	}
	for (int i = 0; i <= countTb * 30; i++) {
		SetCursor(i, baseHeight - 1);
		cout << "_";
	}
	for (int i = 0; i <= countTb * 30; i++) {
		SetCursor(i, baseHeight + lengthOut + 2);
		if (i % 30 == 0)
			cout << "|";
		else
			cout << "_";
	}

	free(line);
	free(tbList);
	SetCursor(0, baseHeight + lengthOut + 2);
	cout << endl;
	return 1;
}
/* -------------------------------------------------------------------------- */
/*                                 đọc từ file                                */
/* -------------------------------------------------------------------------- */
int read(char** args, int& count) {
	readed = true;
	if (count < 2) {
		cout << "[STATUS] [ERROR] Wrong format . Please try another query." << endl;
		return 1;
	}
	DataBaseTable tb;
	string tbKeyCheck = args[1];
	if (!input(tb, tbKeyCheck)) {
		cout << "Error reading file.\n";
		return 1;
	}
	for (unsigned i = 0; i < tb.key_tb.size(); ++i) {
		::hash.Insert(tb.key_tb[i], tb.value_tb[i]);
	}
	string file_name = args[1];
	currDB = file_name.substr(0, file_name.find("_"));
	cout << "[STATUS] using database: " << currDB << endl;

	return 1;
}

int use(char** args, int& count)
{
	if (readed == true) {
		string str(args[1]);
		string delimiter = "_";
		currDB = str.substr(0, str.find(delimiter));
		cout << "[STATUS] using database: " << currDB << endl;
		readed = false;
		return 1;
	}
	if (::hash.SearchKey(args[1]) != "empty") {
		currDB = args[1];
		cout << "[STATUS] using database: " << currDB << endl;
		return 1;
	}

	cout << "[ERROR] Wrong format . Please try another query ." << endl;
	return 1;
}




int num_create() {
	return sizeof(create_str) / sizeof(char*);
}


int create(char** args, int& count)
{
	int i = 0;
	for (i = 0; i < num_create(); i++) {
		if (strcmp(args[1], create_str[i]) == 0) {
			return (*create_func[i])(args, count);
		}
	}
	return 1;
}

//int update(char** args, int& count)
//{
//	return 1;
//}
//select name id from tb1
int select(char** args, int& count)
{
	bool check = false;
	int indexFrom = -1;
	char** arr;
	for (int i = 0; i < count; i++) {
		if (strcmp(args[i], "from") == 0 && i != 1 && i != count - 1) {
			indexFrom = i;
			check = true;
		}
	}
	if (check) {
		if (strcmp(args[1], "*") == 0) {
			string tbKey = currDB + "_" + args[count - 1];
			string tbValue = ::hash.SearchKey(tbKey);
			if (tbValue == "[empty]") {
				cout << "[STATUS] [ERROR] table \"" << args[1] << "\" doesn't existes yet." << endl << endl;
				return 1;
			}
			system("cls");
			//string to char
			int countTb = 0;
			char** tbList;
			//const char* line = tbValue.c_str();
			char* line = new char[tbValue.size() + 1];
			strcpy(line, tbValue.c_str());
			//string to char8
			tbList = split_line(line, countTb);
			string a = ::hash.SearchKey(tbList[0]);
			int lengthi = (int)a[2] - 48;
			int baseHeight = countTb * lengthi + 3;
			int lengthOut = 0;
			for (int i = 0; i < countTb; i++) {
				SetCursor(i * 30, baseHeight);
				cout << "|" << tbList[i];
				SetCursor(0, i * lengthi);
				string tbValue = ::hash.SearchKey(tbList[i]);
				int length = (int)tbValue[2] - 48;
				lengthOut = length;
				for (int x = 0; x < length; x++) {
					//cout << x << endl;
					char index = '0' + x;
					string stra(tbList[i]);
					string rowKey = stra + "_" + index;
					SetCursor(0, i * lengthi + x);
					string rowValue = ::hash.SearchKey(rowKey);
					SetCursor(i * 30, baseHeight + x + 2);
					cout << "|" << rowValue;
				}
			}

			for (int i = 0; i < lengthOut + 2; i++) {
				SetCursor(countTb * 30, baseHeight + i);
				if (i == 0)
					cout << "| [Column] ";
				else if (i == 1)
					cout << "|";
				else
					cout << "| [" << i - 1 << "]";
			}
			for (int i = 0; i < countTb * 30; i++) {
				SetCursor(i, baseHeight + 1);
				if (i % 30 == 0)
					cout << "+";
				else
					cout << "-";
			}
			for (int i = 0; i <= countTb * 30; i++) {
				SetCursor(i, baseHeight - 1);
				cout << "_";
			}
			for (int i = 0; i <= countTb * 30; i++) {
				SetCursor(i, baseHeight + lengthOut + 2);
				if (i % 30 == 0)
					cout << "|";
				else
					cout << "_";

			}
			delete[] line;
			free(tbList);
			SetCursor(0, baseHeight + lengthOut + 2);
			cout << endl;
			return 1;

		}
		string tbKey = currDB + "_" + args[count - 1];
		string tbValue = ::hash.SearchKey(tbKey);
		if (tbValue == "[empty]") {
			cout << "[STATUS] [ERROR] table \"" << args[1] << "\" doesn't existes yet." << endl << endl;
			return 1;
		}
		system("cls");
		int baseHeight = 0;
		for (int i = 1; i < indexFrom; i++) {
			string key = currDB + "_" + args[count - 1] + "_" + args[i];
			SetCursor(0, i - 1);
			string a = ::hash.SearchKey(key);
			int lengthi = (int)a[2] - 48;
			baseHeight = indexFrom * lengthi + 3;
			int lengthOut = 0;
			SetCursor((i - 1) * 30, baseHeight);
			cout << "|" << key;
			SetCursor(0, i * lengthi);
			int length = (int)a[2] - 48;
			lengthOut = length;
			for (int x = 0; x < length; x++) {
				//cout << x << endl;
				char index = '0' + x;
				string stra(key);
				string rowKey = stra + "_" + index;
				SetCursor(0, i * lengthi + x);
				string rowValue = ::hash.SearchKey(rowKey);
				SetCursor((i - 1) * 30, baseHeight + x + 2);
				cout << "|" << rowValue;
			}
		}
		SetCursor(0, indexFrom + baseHeight + 4);
		cout << endl;
		return 1;
	}
	else {
		cout << "[STATUS] [ERROR] Wrong format.Please try another query." << endl;
		return 1;
	}
	return 1;
}


/*
  List of builtin commands, followed by their corresponding functions.
 */

 //=====control console cursor ===============
void SetCursor(int x, int y) {
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { x, y };
	SetConsoleCursorPosition(output, pos);
};

int remove(char** args, int& count) {
	if (count != 3) {
		cout << "[ERROR] Wrong format . Please try another query ." << endl;
		return 1;
	}
	string tbKey, tbValue;
	tbKey = currDB + "_" + args[1];
	tbValue = ::hash.SearchKey(tbKey);

	if (tbValue == "[empty]") {
		cout << "[STATUS] [ERROR] table \"" << args[1] << "\" doesn't existes yet." << endl << endl;
		return 1;
	}
	string linedelete = args[2];
	int countTb = 0;
	char** tbList;
	//const char* line = tbValue.c_str();
	char* line = new char[tbValue.size() + 1];
	strcpy(line, tbValue.c_str());
	//string to char8
	tbList = split_line(line, countTb);
	for (int i = 0; i < countTb; i++) {
		string stra(tbList[i]);
		string rowKey = stra + "_" + linedelete;
		::hash.Remove(rowKey);
	}
	delete[] line;
	free(tbList);
	return 1;
}