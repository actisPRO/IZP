/*
 * Autor: Denis Karev (xkarev00)
 * Výsoké učení technické v Brně
 * Fakulta informačních technologií
 *
 * IZP - Základy programování
 * Projekt č. 2
 *
 * Ten projekt nemá být použit pro jiné než výukové účely.
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>

//region Error codes
#define ERR_NO_ARGS 1
#define ERR_NO_DELIMS 2
#define ERR_READ_FILE 3
#define ERR_UNKNOWN_CMD 4
#define ERR_BAD_CMD_ARGS 5
#define ERR_SELECTION_VAR_EMPTY 6
#define ERR_CANT_DEF 7
#define ERR_CANT_USE 8
#define ERR_NOT_NUMBER 9
#define ERR_SUB_NOT_SET 10
#define ERR_INFINITE_CYCLE 11
//endregion

#define ROW 0
#define COL 1

//region Enums
typedef enum
{
    ChangeStructure,
    ChangeContent,
    VariableOperation,
    SelectionOperation,
    CommandManagement
} e_command_type;

typedef enum
{
    irow,
    arow,
    drow,
    icol,
    acol,
    dcol,
    set,
    clear,
    swap,
    sum,
    avg,
    count,
    len,
    def,
    use,
    inc,
    var_set, // [set]
    sel, // for all selection commands
    cmd_goto,
    iszero,
    sub
} e_command_name;

typedef enum
{
    None,
    String,
    TwoInt,
    Variable,
    Step,
    VariableAndStep,
    TwoVariables
} e_args_type;

typedef enum {
    CellSelection,
    RowSelection,
    ColumnSelection,
    WindowSelection,
    Full,
    Min,
    Max,
    FindString,
    FromVariable
} e_selection_type;
//endregion

//region Types
typedef struct s_cell {
    int row;
    int col;
    char *content;
    struct s_cell *next;
} t_cell;

typedef struct s_row
{
    int pos;
    int cols_count;
    t_cell *first;
    struct s_row *next;
} t_row;

typedef struct
{
    int rows_count;
    t_row *first;
} t_table;

typedef struct {
    e_command_name name;
    e_command_type type;
    e_args_type args_type;
    e_selection_type selection_type;

    int var_id;
    int var2_id;
    int step;
    int r1, c1, r2, c2;
    char str[1000];
} t_command;

typedef struct s_cmdseq {
    t_command cmd;
    struct s_cmdseq *next;
} t_cmdseq;

typedef struct {
    int top_left[2];
    int down_right[2];
    e_selection_type type;
} t_selection;
//endregion

//region Global variables
char g_delims[128];                     // array with delimiters
int g_error = 0;                        // error code (must be 0, or error() must be called)
char g_error_additional[1001] = {0};    // additional information for an error message (not for all error codes)

t_table *g_table = NULL;
t_selection g_selection;                // current selection
t_cmdseq *g_cmds = NULL;

t_selection v_selection;                // selection, stored in the variable _
char *v_var[10];                        // array of variables from _0 to _9
bool v_var_isset[10] = {false};         // which variables were set
bool v_sel_isset = false;               // was variable _ set
//endregion

//region List methods

// Creates a new structure and returns the pointer to it
t_cell* cell_create(int row, int col, char *value);
// Adds a new element to the end of the list and returns the pointer to it
t_cell* cell_add(t_cell *first, char *value);
// Adds a new element at the beginning of the list
void cell_push(t_cell **first, char *value);
// Inserts a new element after the specified one
void cell_insert(t_cell *previous, char *value);
// Returns a pointer to an element with a specified index
t_cell* cell_get(t_cell *first, int index);
// Returns an amount of elements in the list
int cell_count(t_cell *first);
// Removes an element with the specified index
void cell_delete_at(t_cell **first, int index);
// Removes all elements after the specified and including it.
void cell_delete(t_cell *first);

// Creates a new structure and returns the pointer to it
t_row* row_create(int pos, t_cell *first_cell);
// Adds a new element to the end of the list and returns the pointer to it
t_row* row_add(t_row *first, t_cell *first_cell);
// Adds a new element at the beginning of the list
void row_push(t_row **first, t_cell *value);
// Inserts a new element after the specified one
void row_insert(t_row *previous, t_cell *value);
// Returns a pointer to an element with the specified index
t_row* row_get(t_row *first, int index);
// Returns an amount of elements in the list
int row_count(t_row *first);
// Removes an element with the specified index
void row_delete_at(t_row **first, int index);
// Removes all elements after the specified and including it.
void row_delete(t_row *first);

// Creates a new structure and returns the pointer to it
t_cmdseq* cmdseq_create(t_command cmd);
// Adds a new element to the end of the list and returns the pointer to it
t_cmdseq* cmdseq_add(t_cmdseq *first, t_command cmd);
// Returns a pointer to an element with the specified index
t_cmdseq *cmdseq_get(t_cmdseq *first, int index);
// Removes all elements after the specified and including it.
void cmdseq_delete(t_cmdseq *first);

//endregion

//region Utilities
bool is_delim(char value);
// Does the string contain special characters (delimiter, quotes, backslahes)
bool contains_specials(char *value);
// Adds quotes and backslahes to the string
char* fix_string(char *input);
e_command_name get_cmd_name (char *command);
e_args_type get_args_type (e_command_name command);
e_command_type get_cmd_type (e_command_name command);
t_command parse_command(char *input);
bool str_starts(const char *pre, const char *str);

// Adds rows to the end of g_table
void add_rows(int count);
// Adds columns to the end of g_table
void add_columns(int count);
//endregion

//region Commands
void change_selection(t_command cmd);
void change_structure(t_command cmd);
void change_content(t_command cmd);
void exec_variable_cmd(t_command cmd);
void manage_sequence(t_command cmd, int *current_command);
//endregion

void read_delims(int argc, char *argv[]);
void fix_table();
void read_table(int argc, char *argv[]);
void read_commands(int argc, char *argv[]);
void run_commands();
void write_table(int argc, char *argv[]);
void print_table();

//region Exit functions
// Shows a error message and calls clean()
void error();
// Frees allocated memory
void clean();
//endregion

int main(int argc, char *argv[])
{
    for (int i = 0; i < 10; ++i)
        v_var[i] = calloc(1, sizeof(char));

    read_delims(argc, argv);
    if (g_error) error();

    read_table(argc, argv);
    if (g_error) error();

    fix_table();

    g_selection.top_left[ROW] = 1;
    g_selection.top_left[COL] = 1;
    g_selection.down_right[ROW] = 1;
    g_selection.down_right[COL] = 1;
    g_selection.type = Full;

    read_commands(argc, argv);
    if (g_error) error();

    run_commands();
    if (g_error) error();

    //print_table();
    write_table(argc, argv);
    clean();
    return 0;
}

void read_delims(int argc, char *argv[])
{
    if (argc < 2)
    {
        g_error = ERR_NO_ARGS;
        return;
    }
    if (strcmp(argv[1], "-d") == 0)
    {
        if (argc < 3)
        {
            g_error = ERR_NO_DELIMS;
            return;
        }
        strcpy(g_delims, argv[2]);
    }
    else
    {
        strcpy(g_delims, " ");
    }
}

void fix_table()
{
    // if table was empty we'll create the only cell
    if (g_table->first == NULL)
    {
        t_cell *new = cell_create(1, 1, "\0");
        g_table->first = row_create(1, new);
        g_table->rows_count = 1;
        return;
    }

    int max_cols = 0;
    t_row *row = g_table->first;
    while (row != NULL)
    {
        int cols = cell_count(row->first);
        if (max_cols < cols) max_cols = cols;
        row = row->next;
    }

    row = g_table->first;
    while (row != NULL)
    {
        int cols = cell_count(row->first);
        if (cols < max_cols)
            for (int i = 0; i < max_cols - cols; ++i)
                cell_add(row->first, "\0");
        row->cols_count = max_cols;

        row = row->next;
    }
}

void read_table(int argc, char *argv[])
{
    char *file_name = argv[argc - 1];
    FILE *fptr;
    fptr = fopen(file_name, "r");

    if (fptr == NULL)
    {
        g_error = ERR_READ_FILE;
        return;
    }

    int next = fgetc(fptr);
    int row = 1;
    int col = 1;

    char *content = malloc(sizeof(char));
    int pos = 0;

    t_cell *cell = NULL;
    t_row *first_row = NULL;

    bool is_quoted = false;
    bool next_escaped = false;
    while (next != EOF)
    {
        if (next == '\n')
        {
            if (cell == NULL)
                cell = cell_create(row, col, content);
            else
                cell_add(cell, content);

            if (first_row == NULL)
                first_row = row_create(1, cell);
            else
                row_add(first_row, cell);

            col = 1;
            ++row;
            cell = NULL;

            pos = 0;
            content = realloc(content, sizeof(char));
        }
        else if (next == '"' && !next_escaped)
        {
            is_quoted = !is_quoted;
        }
        else if (next == '\\' && !next_escaped && is_quoted)
        {
            next_escaped = true;
        }
        else if (is_delim((char) next) && !is_quoted)
        {
            if (cell == NULL)
                cell = cell_create(row, col, content);
            else
                cell_add(cell, content);

            pos = 0;
            content = realloc(content, sizeof(char));
        }
        else
        {
            content[pos] = (char) next;
            ++pos;
            content = realloc(content, sizeof(char) * (pos + 1));
            content[pos] = 0;

            if (next_escaped) next_escaped = false;
        }

        next = fgetc(fptr);
    }

    g_table = malloc(sizeof(t_table));
    g_table->first = first_row;
    g_table->rows_count = row_count(first_row);

    free(content);
}

void read_commands(int argc, char *argv[])
{
    int arg = strcmp(g_delims, " ") == 0 ? 1 : 3;
    if (arg >= argc - 1)
    {
        g_error = ERR_NO_ARGS;
        return;
    }

    char *next_cmd = calloc(1, sizeof(char));
    int pos = 0;

    for (int i = 0; i <= (int) strlen(argv[arg]); ++i)
    {
        if (argv[arg][i] == ';' || i == (int) strlen(argv[arg]))
        {
            t_command cmd = parse_command(next_cmd);
            if (g_error) return;

            if (g_cmds == NULL) g_cmds = cmdseq_create(cmd);
            else cmdseq_add(g_cmds, cmd);

            next_cmd = realloc(next_cmd, sizeof(char));
            pos = 0;
            next_cmd[pos] = 0;
        }
        else
        {
            next_cmd = realloc(next_cmd, sizeof(char) * (pos + 2));
            next_cmd[pos] = argv[arg][i];
            ++pos;
            next_cmd[pos] = 0;
        }
    }

    free(next_cmd);
}

void write_table(int argc, char *argv[])
{
    FILE *fptr;
    fptr = fopen(argv[argc - 1], "w");
    if (fptr == NULL)
    {
        g_error = ERR_READ_FILE;
        return;
    }

    t_row *row = g_table->first;
    while (row != NULL)
    {
        t_cell *c_cell = row->first;
        while (c_cell->next != NULL)
        {
            if (contains_specials(c_cell->content))
            {
                char* fixed = fix_string(c_cell->content);
                fprintf(fptr, "%s%c", fixed, g_delims[0]);
                free(fixed);
            }
            else fprintf(fptr, "%s%c", c_cell->content, g_delims[0]);
            c_cell = c_cell->next;
        }

        if (contains_specials(c_cell->content))
        {
            char* fixed = fix_string(c_cell->content);
            fprintf(fptr, "%s\n", fixed);
            free(fixed);
        }
        else fprintf(fptr, "%s\n", c_cell->content);

        row = row->next;
    }

    fclose(fptr);
}

void run_commands()
{
    t_cmdseq *next = g_cmds;
    int current_command = 0;
    while (next != NULL)
    {
        t_command cmd = next->cmd;
        switch (cmd.type)
        {
        case ChangeStructure:
            change_structure(cmd);
            break;
        case ChangeContent:
            change_content(cmd);
            break;
        case VariableOperation:
            exec_variable_cmd(cmd);
            break;
        case SelectionOperation:
            change_selection(cmd);
            break;
        case CommandManagement:
            manage_sequence(cmd, &current_command);
            break;
        }
        if (g_error) return;
        ++current_command;
        next = cmdseq_get(g_cmds, current_command);
    }
}

t_command parse_command(char *input)
{
    t_command cmd;
    // selection commands
    if (input[0] == '[' && strcmp(input, "[set]") != 0)
    {
        cmd.name = sel;
        cmd.type = SelectionOperation;
        if (strcmp(input, "[_,_]") == 0)
            cmd.selection_type = Full;
        else if (strcmp(input, "[min]") == 0)
            cmd.selection_type = Min;
        else if (strcmp(input, "[max]") == 0)
            cmd.selection_type = Max;
        else if (strcmp(input, "[_]") == 0)
            cmd.selection_type = FromVariable;
        else if (str_starts("[find", input))
        {
            cmd.selection_type = FindString;

            int len = (int) strlen(input) - 6;
            char str[len];
            for (int i = 6; i < (int) strlen(input) - 1; ++i) // -1 because we don't need a closing bracket
                str[i - 6] = input[i];
            str[len - 1] = 0;

            if (str[0] == '"')
            {
                memmove(str, str + 1, len - 2);
                str[len - 2] = 0;
            }

            char args_escaped[strlen(str) + 1];
            int pos = 0;
            bool next_escaped = false;
            for (int i = 0; i < (int) strlen(str); ++i)
            {
                if (str[i] == '\\' && !next_escaped)
                {
                    next_escaped = true;
                }
                else
                {
                    args_escaped[pos] = str[i];
                    ++pos;
                    if (next_escaped) next_escaped = false;
                }
            }
            args_escaped[pos] = 0;

            strcpy(cmd.str, args_escaped);
        }
        else
        {
            size_t len = strlen(input);
            memmove(input, input+1, len-2);
            input[len-2] = 0;

            int R1, C1, R2, C2;
            char content[4][32];
            for (int i = 0; i <= 4; ++i) content[i][0] = 0;

            int charNum = 0;
            int tokenNum = 0;
            for (int i = 0; i < (int) len; ++i)
            {
                if (input[i] != ',')
                {
                    content[tokenNum][charNum] = input[i];
                    ++charNum;
                }
                else
                {
                    content[tokenNum][charNum] = '\0';
                    ++tokenNum;
                    charNum = 0;
                }
            }

            if (content[0][0] == '_') //[_,C]
            {
                cmd.selection_type = ColumnSelection;
                C1 = atoi(content[1]);

                cmd.c1 = C1;
                return cmd;
            }
            R1 = atoi(content[0]);
            if (R1 <= 0)
            {
                g_error = ERR_BAD_CMD_ARGS;
                strcpy(g_error_additional, input);
            }
            cmd.r1 = R1;

            if (content[1][0] == '_' && content[2][0] == 0 && content[3][0] == 0) //[R,_]
            {
                cmd.selection_type = RowSelection;
                return cmd;
            }
            C1 = atoi(content[1]);
            if (C1 <= 0)
            {
                g_error = ERR_BAD_CMD_ARGS;
                strcpy(g_error_additional, input);
            }
            cmd.c1 = C1;

            if (content[2][0] == 0)
            {
                cmd.selection_type = CellSelection;
                return cmd;
            } //[R,C]

            if (content[2][0] == '-') R2 = g_table->rows_count;
            else R2 = atoi(content[2]);

            cmd.selection_type = WindowSelection;

            if (content[3][0] == '-') C2 = g_table->first->cols_count;
            else C2 = atoi(content[3]);

            if (R2 <= 0 || C2 <= 0)
            {
                g_error = ERR_BAD_CMD_ARGS;
                strcpy(g_error_additional, input);
            }
            if (R1 > R2 || C1 > C2)
            {
                g_error = ERR_BAD_CMD_ARGS;
                strcpy(g_error_additional, input);
            }
            cmd.r2 = R2;
            cmd.c2 = C2;
        }
    }
    else
    {
        char name[32] = {0}, args[1000] = {0};
        int c = 0;
        while (input[c] != ' ' && c < (int) strlen(input))
        {
            name[c] = input[c];
            ++c;
        }
        //name[c + 1] = 0;

        if (strlen(input) != strlen(name))
        {
            ++c;
            for (int i = c; i < (int) strlen(input); ++i)
            {
                args[i - c] = input[i];
            }
        }

        cmd.name = get_cmd_name(name);
        cmd.type = get_cmd_type(cmd.name);
        cmd.args_type = get_args_type(cmd.name);
        if (g_error)
        {
            strcpy(g_error_additional, input);
            return cmd;
        }

        if (cmd.args_type == TwoInt)
        {
            int row = 0, col = 0;
            int check = sscanf(args, "[%d,%d]", &row, &col);
            if (check != 2 || row <= 0 || col <= 0)
            {
                g_error = ERR_BAD_CMD_ARGS;
                strcpy(g_error_additional, input);
                return cmd;
            }

            cmd.r1 = row;
            cmd.c1 = col;
        }
        else if (cmd.args_type == String)
        {
            if (args[0] == '\0')
            {
                g_error = ERR_BAD_CMD_ARGS;
                strcpy(g_error_additional, input);
                return cmd;
            }

            if (args[0] == '"')
            {
                size_t len = strlen(args);
                memmove(args, args + 1, len - 2);
                args[len - 2] = 0;
            }

            char args_escaped[strlen(args) + 1];
            int pos = 0;
            bool next_escaped = false;
            for (int i = 0; i < (int) strlen(args); ++i)
            {
                if (args[i] == '\\' && !next_escaped)
                {
                    next_escaped = true;
                }
                else
                {
                    args_escaped[pos] = args[i];
                    ++pos;
                    if (next_escaped) next_escaped = false;
                }
            }
            args_escaped[pos] = 0;

            strcpy(cmd.str, args_escaped);
        }
        else if (cmd.args_type == Variable)
        {
            int num = 0;
            int check = sscanf(args, "_%d", &num);
            if (check != 1 || num < 0 || num > 9)
            {
                g_error = ERR_BAD_CMD_ARGS;
                strcpy(g_error_additional, input);
                return cmd;
            }

            cmd.var_id = num;
        }
        else if (cmd.args_type == Step)
        {
            int num = 0;
            int check = sscanf(args, "%d", &num);
            if (check != 1)
            {
                g_error = ERR_BAD_CMD_ARGS;
                strcpy(g_error_additional, input);
                return cmd;
            }

            cmd.step = num;
        }
        else if (cmd.args_type == TwoVariables)
        {
            int num1 = 0, num2 = 0;
            int check = sscanf(args, "_%d _%d", &num1, &num2);
            if (check != 2 || num1 < 0 || num1 > 9 || num2 < 0 || num2 > 9)
            {
                g_error = ERR_BAD_CMD_ARGS;
                strcpy(g_error_additional, input);
                return cmd;
            }

            cmd.var_id = num1;
            cmd.var2_id = num2;
        }
        else if (cmd.args_type == VariableAndStep)
        {
            int num1 = 0, num2 = 0;
            int check = sscanf(args, "_%d %d", &num1, &num2);
            if (check != 2 || num1 < 0 || num1 > 9)
            {
                g_error = ERR_BAD_CMD_ARGS;
                strcpy(g_error_additional, input);
                return cmd;
            }

            cmd.var_id = num1;
            cmd.step = num2;
        }
    }

    return cmd;
}

void change_selection(t_command cmd)
{
    if (cmd.selection_type == CellSelection)
    {
        if (g_table->first->cols_count < cmd.c1) add_columns(cmd.c1 - g_table->first->cols_count);
        if (g_table->rows_count < cmd.r1) add_rows(cmd.r1 - g_table->rows_count);
        g_selection.top_left[ROW] = cmd.r1;
        g_selection.top_left[COL] = cmd.c1;
        g_selection.down_right[ROW] = cmd.r1;
        g_selection.down_right[COL] = cmd.c1;
        g_selection.type = CellSelection;
    }
    else if (cmd.selection_type == RowSelection)
    {
        if (g_table->rows_count < cmd.r1) add_rows(cmd.r1 - g_table->rows_count);
        g_selection.top_left[ROW] = cmd.r1;
        g_selection.top_left[COL] = 1;
        g_selection.down_right[ROW] = cmd.r1;
        g_selection.down_right[COL] = g_table->first->cols_count;
        g_selection.type = RowSelection;
    }
    else if (cmd.selection_type == ColumnSelection)
    {
        if (g_table->first->cols_count < cmd.c1) add_columns(cmd.c1 - g_table->first->cols_count);
        g_selection.top_left[ROW] = 1;
        g_selection.top_left[COL] = cmd.c1;
        g_selection.down_right[ROW] = g_table->rows_count;
        g_selection.down_right[COL] = cmd.c1;
        g_selection.type = ColumnSelection;
    }
    else if (cmd.selection_type == WindowSelection)
    {
        if (g_table->first->cols_count < cmd.c2) add_columns(cmd.c2 - g_table->first->cols_count);
        if (g_table->rows_count < cmd.r2) add_rows(cmd.r2 - g_table->rows_count);
        g_selection.top_left[ROW] = cmd.r1;
        g_selection.top_left[COL] = cmd.c1;
        g_selection.down_right[ROW] = cmd.r2;
        g_selection.down_right[COL] = cmd.c2;
        g_selection.type = WindowSelection;
    }
    else if (cmd.selection_type == Full)
    {
        g_selection.top_left[ROW] = 1;
        g_selection.top_left[COL] = 1;
        g_selection.down_right[ROW] = g_table->rows_count;
        g_selection.down_right[COL] = g_table->first->cols_count;
        g_selection.type = Full;
    }
    else if (cmd.selection_type == Min)
    {
        double min = DBL_MAX;
        int col_s = 0, row_s = 0;
        for (int row = g_selection.top_left[ROW] - 1; row < g_selection.down_right[ROW]; ++row)
        {
            t_row *current_row = row_get(g_table->first, row);
            for (int col = g_selection.top_left[COL] - 1; col < g_selection.down_right[COL]; ++col)
            {
                t_cell *current_cell = cell_get(current_row->first, col);
                if (current_cell->content[0] != '\0')
                {
                    char *endptr;
                    double value = strtod(current_cell->content, &endptr);
                    if (*endptr == '\0' && value < min)
                    {
                        min = value;
                        col_s = col + 1;
                        row_s = row + 1;
                    }
                }
            }
        }

        if (col_s != 0 && row_s != 0)
        {
            g_selection.top_left[ROW] = row_s;
            g_selection.top_left[COL] = col_s;
            g_selection.down_right[ROW] = row_s;
            g_selection.down_right[COL] = col_s;
            g_selection.type = CellSelection;
        }
    }
    else if (cmd.selection_type == Max)
    {
        double max = DBL_MIN;
        int col_s = 0, row_s = 0;
        for (int row = g_selection.top_left[ROW] - 1; row < g_selection.down_right[ROW]; ++row)
        {
            t_row *current_row = row_get(g_table->first, row);
            for (int col = g_selection.top_left[COL] - 1; col < g_selection.down_right[COL]; ++col)
            {
                t_cell *current_cell = cell_get(current_row->first, col);
                if (current_cell->content[0] != '\0')
                {
                    char *endptr;
                    double value = strtod(current_cell->content, &endptr);
                    if (*endptr == '\0' && value > max)
                    {
                        max = value;
                        col_s = col + 1;
                        row_s = row + 1;
                    }
                }
            }
        }

        if (col_s != 0 && row_s != 0)
        {
            g_selection.top_left[ROW] = row_s;
            g_selection.top_left[COL] = col_s;
            g_selection.down_right[ROW] = row_s;
            g_selection.down_right[COL] = col_s;
            g_selection.type = CellSelection;
        }
    }
    else if (cmd.selection_type == FindString)
    {
        int col_s = 0, row_s = 0;
        for (int row = g_selection.top_left[ROW] - 1; row < g_selection.down_right[ROW]; ++row)
        {
            t_row *current_row = row_get(g_table->first, row);
            for (int col = g_selection.top_left[COL] - 1; col < g_selection.down_right[COL]; ++col)
            {
                t_cell *current_cell = cell_get(current_row->first, col);
                if (strcmp(cmd.str, current_cell->content) == 0)
                {
                    col_s = col + 1;
                    row_s = row + 1;
                    break;
                }
            }
            if (col_s != 0 && row_s != 0) break;
        }

        if (col_s != 0 && row_s != 0)
        {
            g_selection.top_left[ROW] = row_s;
            g_selection.top_left[COL] = col_s;
            g_selection.down_right[ROW] = row_s;
            g_selection.down_right[COL] = col_s;
            g_selection.type = CellSelection;
        }
    }
    else if (cmd.selection_type == FromVariable)
    {
        if (!v_sel_isset)
        {
            g_error = ERR_SELECTION_VAR_EMPTY;
            return;
        }
        g_selection = v_selection;
    }
}

void change_structure(t_command cmd)
{
    if (cmd.name == irow)
    {
        int top = g_selection.top_left[ROW];
        t_cell *new = cell_create(top, 1, "\0");
        for (int col = 2; col <= g_table->first->cols_count; ++col)
            cell_add(new, "\0");

        if (top - 1 == 0) row_push(&(g_table->first), new);
        else
        {
            t_row *prev = row_get(g_table->first, top - 2);
            row_insert(prev, new);
        }

        t_row *created = row_get(g_table->first, top - 1);
        created->cols_count = cell_count(created->first);

        ++g_table->rows_count;
    }
    else if (cmd.name == arow)
    {
        int top = g_selection.down_right[ROW];
        t_cell *new = cell_create(top, 1, "\0");
        for (int col = 2; col <= g_table->first->cols_count; ++col)
            cell_add(new, "\0");

        t_row *prev = row_get(g_table->first, top - 1);
        row_insert(prev, new);

        t_row *created = row_get(g_table->first, top);
        created->cols_count = cell_count(created->first);
    }
    else if (cmd.name == drow)
    {
        for (int row = g_selection.down_right[ROW] - 1; row >= g_selection.top_left[ROW] - 1; --row)
            row_delete_at(&(g_table->first), row);

        g_table->rows_count -= g_selection.down_right[ROW] - g_selection.top_left[ROW] + 1;
    }
    else if (cmd.name == icol)
    {
        int col = g_selection.top_left[COL] - 1;
        for (int row = 0; row < g_table->rows_count; ++row)
        {
            t_row *current_row = row_get(g_table->first, row);
            t_cell **current = &(current_row->first);
            if (col == 0) cell_push(current, "\0");
            else
            {
                t_cell *before = cell_get(*current, col - 1);
                cell_insert(before, "\0");
            }

            ++current_row->cols_count;
        }
    }
    else if (cmd.name == acol)
    {
        int col = g_selection.down_right[COL] - 1;
        for (int row = 0; row < g_table->rows_count; ++row)
        {
            t_row *current_row = row_get(g_table->first, row);
            t_cell *current = cell_get(current_row->first, col);
            cell_insert(current, "\0");
            ++current_row->cols_count;
        }
    }
    else if (cmd.name == dcol)
    {
        for (int row = 0; row < g_table->rows_count; ++row)
        {
            t_row *current_row = row_get(g_table->first, row);
            t_cell **first_cell = &(current_row->first);
            for (int col = g_selection.down_right[COL] - 1; col >= g_selection.top_left[COL] - 1; --col)
                cell_delete_at(first_cell, col);

            current_row->cols_count -= g_selection.down_right[COL] - g_selection.top_left[COL] + 1;
        }
    }
}

void change_content(t_command cmd)
{
    if (cmd.name == swap || cmd.name == sum || cmd.name == avg || cmd.name == count || cmd.name == len)
    {
        if (g_table->first->cols_count < cmd.c1) add_columns(cmd.c1 - g_table->first->cols_count);
        if (g_table->rows_count < cmd.r1) add_rows(cmd.r1 - g_table->rows_count);
    }

    if (cmd.name == set)
    {
        for (int row = g_selection.top_left[ROW] - 1; row < g_selection.down_right[ROW]; ++row)
        {
            t_row *c_row = row_get(g_table->first, row);
            for (int col = g_selection.top_left[COL] - 1; col < g_selection.down_right[COL]; ++col)
            {
                t_cell *c_cell = cell_get(c_row->first, col);
                c_cell->content = realloc(c_cell->content, strlen(cmd.str) + 1);
                strcpy(c_cell->content, cmd.str);
            }
        }
    }
    else if (cmd.name == clear)
    {
        for (int row = g_selection.top_left[ROW] - 1; row < g_selection.down_right[ROW]; ++row)
        {
            t_row *c_row = row_get(g_table->first, row);
            for (int col = g_selection.top_left[COL] - 1; col < g_selection.down_right[COL]; ++col)
            {
                t_cell *c_cell = cell_get(c_row->first, col);
                c_cell->content = realloc(c_cell->content, 1);
                strcpy(c_cell->content, "");
            }
        }
    }
    else if (cmd.name == swap)
    {
        t_cell *swap_cell = cell_get(row_get(g_table->first, cmd.r1 - 1)->first, cmd.c1 - 1);

        for (int row = g_selection.top_left[ROW] - 1; row < g_selection.down_right[ROW]; ++row)
        {
            t_row* c_row = row_get(g_table->first, row);
            for (int col = g_selection.top_left[COL] - 1; col < g_selection.down_right[COL]; ++col)
            {
                t_cell *c_cell = cell_get(c_row->first, col);
                char *buffer = calloc(strlen(swap_cell->content) + 1, sizeof(char));
                strcpy(buffer, swap_cell->content);

                swap_cell->content = realloc(swap_cell->content, strlen(c_cell->content) + 1);
                strcpy(swap_cell->content, c_cell->content);
                c_cell->content = realloc(c_cell->content, strlen(buffer) + 1);
                strcpy(c_cell->content, buffer);

                free(buffer);
            }
        }
    }
    else if (cmd.name == sum || cmd.name == avg || cmd.name == count || cmd.name == len)
    {
        double summ = 0;
        int with_number = 0;
        int not_empty = 0;
        int str_len = 0;

        for (int row = g_selection.top_left[ROW] - 1; row < g_selection.down_right[ROW]; ++row)
        {
            t_row* c_row = row_get(g_table->first, row);
            for (int col = g_selection.top_left[COL] - 1; col < g_selection.down_right[COL]; ++col)
            {
                t_cell* c_cell = cell_get(c_row->first, col);

                if (c_cell->content[0] != 0)
                {
                    ++not_empty;
                    str_len += (int) strlen(c_cell->content);

                    char *endptr;
                    double value = strtod(c_cell->content, &endptr);
                    if (*endptr == 0)
                    {
                        summ += value;
                        ++with_number;
                    }
                }
            }
        }
        double avrg = with_number != 0 ? summ / with_number : 0;

        char buffer[48] = {0};
        if (cmd.name == sum) sprintf(buffer, "%g", summ);
        else if (cmd.name == avg) sprintf(buffer, "%g", avrg);
        else if (cmd.name == count) sprintf(buffer, "%d", not_empty);
        else if (cmd.name == len) sprintf(buffer, "%d", str_len);

        t_cell *selected = cell_get(row_get(g_table->first, cmd.r1 - 1)->first, cmd.c1 - 1);
        selected->content = realloc(selected->content, strlen(buffer) + 1);
        strcpy(selected->content, buffer);
    }
}

void exec_variable_cmd(t_command cmd)
{
    if (cmd.name == var_set)
    {
        v_selection = g_selection;
        v_sel_isset = true;
    }
    else if (cmd.name == def)
    {
        if (g_selection.top_left[ROW] != g_selection.down_right[ROW]
                || g_selection.top_left[COL] != g_selection.down_right[COL])
        {
            g_error = ERR_CANT_DEF;
            sprintf(g_error_additional, "_%d", cmd.var_id);
            return;
        }

        t_row* current_row = row_get(g_table->first, g_selection.top_left[ROW] - 1);
        t_cell* current_cell = cell_get(current_row->first, g_selection.top_left[COL] - 1);
        v_var[cmd.var_id] = realloc(v_var[cmd.var_id], strlen(current_cell->content) + 1);

        strcpy(v_var[cmd.var_id], current_cell->content);
        v_var[cmd.var_id][strlen(current_cell->content)] = '\0';
        v_var_isset[cmd.var_id] = true;
    }
    else if (cmd.name == use)
    {
        if (!v_var_isset[cmd.var_id])
        {
            g_error = ERR_CANT_USE;
            sprintf(g_error_additional, "_%d", cmd.var_id);
            return;
        }

        for (int row = g_selection.top_left[ROW] - 1; row < g_selection.down_right[ROW]; ++row)
        {
            t_row* current_row = row_get(g_table->first, row);
            for (int col = g_selection.top_left[COL] - 1; col < g_selection.down_right[COL]; ++col)
            {
                t_cell* current_cell = cell_get(current_row->first, col);
                current_cell->content = realloc(current_cell->content, strlen(v_var[cmd.var_id]));
                strcpy(current_cell->content, v_var[cmd.var_id]);
            }
        }
    }
    else if (cmd.name == inc)
    {
        if (!v_var_isset[cmd.var_id] || v_var[cmd.var_id][0] == 0)
        {
            v_var[cmd.var_id] = realloc(v_var[cmd.var_id], sizeof(char) * 2);
            strcpy(v_var[cmd.var_id], "1");
            v_var_isset[cmd.var_id] = true;
            return;
        }

        char *endptr;
        double value = strtod(v_var[cmd.var_id], &endptr);
        if (*endptr == '\0') ++value;
        else value = 1;

        char buffer[64] = {0};
        sprintf(buffer, "%g", value);

        v_var[cmd.var_id] = realloc(v_var[cmd.var_id], strlen(buffer) + 1);
        strcpy(v_var[cmd.var_id], buffer);
    }
}

void manage_sequence(t_command cmd, int *current_command)
{
    if (cmd.name == sub)
    {
        if (!v_var_isset[cmd.var_id] || !v_var_isset[cmd.var2_id])
        {
            g_error = ERR_SUB_NOT_SET;
            sprintf(g_error_additional, "_%d _%d", cmd.var_id, cmd.var2_id);
            return;
        }

        char *endptr;
        double value1 = strtod(v_var[cmd.var_id], &endptr);
        if (*endptr != 0)
        {
            g_error = ERR_NOT_NUMBER;
            sprintf(g_error_additional, "_%d", cmd.var_id);
            return;;
        }

        double value2 = strtod(v_var[cmd.var2_id], &endptr);
        if (*endptr != 0)
        {
            g_error = ERR_NOT_NUMBER;
            sprintf(g_error_additional, "_%d", cmd.var_id);
            return;;
        }

        char buffer[64] = {0};
        sprintf(buffer, "%g", value1 - value2);
        v_var[cmd.var_id] = realloc(v_var[cmd.var_id], strlen(buffer) + 1);
        strcpy(v_var[cmd.var_id], buffer);
    }
    else if (cmd.name == cmd_goto)
    {
        if (cmd.step == 1) return;
        if (cmd.step >= 2) --cmd.step;
        int result = *current_command + cmd.step;
        if (result < 0) result = 0;

        if (result == *current_command)
        {
            g_error = ERR_INFINITE_CYCLE;
            return;
        }

        *current_command = result;
    }
    else if (cmd.name == iszero)
    {
        if (!v_var_isset[cmd.var_id])
        {
            g_error = ERR_CANT_USE;
            sprintf(g_error_additional, "%d", cmd.var_id);
            return;
        }

        char *endptr;
        double value = strtod(v_var[cmd.var_id], &endptr);
        if (*endptr != 0)
        {
            g_error = ERR_NOT_NUMBER;
            sprintf(g_error_additional, "_%d", cmd.var_id);
            return;
        }

        if (value == 0)
        {
            if (cmd.step == 1) return;
            if (cmd.step >= 2) --cmd.step;
            int result = *current_command + cmd.step;
            if (result < 0) result = 0;

            if (result == *current_command)
            {
                g_error = ERR_INFINITE_CYCLE;
                return;
            }

            *current_command = result;
        }
    }
}

void print_table()
{
    t_row *current = g_table->first;
    while (current != NULL)
    {
        t_cell *c_cell = current->first;
        while (c_cell->next != NULL)
        {
            if (contains_specials(c_cell->content))
            {
                char* fixed = fix_string(c_cell->content);
                printf("%s%c", fixed, g_delims[0]);
                free(fixed);
            }
            else printf("%s%c", c_cell->content, g_delims[0]);
            c_cell = c_cell->next;
        }

        if (contains_specials(c_cell->content))
        {
            char* fixed = fix_string(c_cell->content);
            printf("%s\n", fixed);
            free(fixed);
        }
        else printf("%s\n", c_cell->content);

        current = current->next;
    }
}

t_cell* cell_create(int row, int col, char *value)
{
    t_cell *new = malloc(sizeof(t_cell));
    new->row = row;
    new->col = col;
    new->content = malloc(strlen(value) + 1);
    strcpy(new->content, value);
    new->content[strlen(value)] = 0;
    new->next = NULL;

    return new;
}

t_cell* cell_add(t_cell *first, char *value)
{
    t_cell *current = first;
    while (current->next != NULL)
        current = current->next;

    current->next = cell_create(current->row, current->col + 1, value);

    return current->next;
}

void cell_push(t_cell **first, char *value)
{
    t_cell *new = cell_create(1, 1, value);
    new->next = *first;
    new->row = new->next->row;
    *first = new;

    t_cell *current = new->next;
    while (current != NULL)
    {
        ++current->col;
        current = current->next;
    }
}

void cell_insert(t_cell *previous, char *value)
{
    t_cell *new = cell_create(previous->row, previous->col + 1, value);
    new->next = previous->next;
    previous->next = new;

    t_cell *current = new->next;
    while (current != NULL)
    {
        ++current->col;
        current = current->next;
    }
}

t_cell* cell_get(t_cell *first, int index)
{
    if (index < 0) return NULL;

    t_cell *current = first;
    for (int i = 0; i < index; ++i)
    {
        if (current == NULL) return NULL;
        current = current->next;
    }

    return current;
}

int cell_count(t_cell *first)
{
    int count = 0;
    t_cell *current = first;
    while (current != NULL)
    {
        ++count;
        current = current->next;
    }

    return count;
}

void cell_delete_at(t_cell **first, int index)
{
    t_cell *temp = *first;
    t_cell *previous = NULL;

    if (index == 0)
    {
        *first = temp->next;
    }
    else
    {
        for (int i = 1; i <= index; ++i)
        {
            previous = temp;
            temp = temp->next;
            if (temp == NULL) return;
        }

        previous->next = temp->next;
    }

    free(temp->content);
    free(temp);

    t_cell *current = *first;
    while (current != NULL)
    {
        --current->col;
        current = current->next;
    }
}

void cell_delete(t_cell *first)
{
    if (first->next != NULL)
        cell_delete(first->next);

    free(first->content);
    free(first);
}

t_row* row_create(int pos, t_cell *first_cell)
{
    t_row *new = malloc(sizeof(t_row));
    new->pos = pos;
    new->cols_count = 1;
    new->first = first_cell;
    new->next = NULL;

    return new;
}

t_row* row_add(t_row *first, t_cell *first_cell)
{
    t_row *current = first;
    while (current->next != NULL)
        current = current->next;

    current->next = row_create(current->pos + 1, first_cell);

    return current->next;
}

void row_push(t_row **first, t_cell *value)
{
    t_row *new = row_create(1,  value);
    new->next = *first;
    *first = new;

    t_row *current = new->next;
    while (current != NULL)
    {
        ++current->pos;
        current = current->next;
    }
}

void row_insert(t_row *previous, t_cell *value)
{
    t_row *new = row_create(previous->pos + 1, value);
    new->next = previous->next;
    previous->next = new;

    t_row *current = new->next;
    while (current != NULL)
    {
        ++current->pos;
        current = current->next;
    }
}

t_row* row_get(t_row *first, int index)
{
    if (index < 0) return NULL;

    t_row *current = first;
    for (int i = 0; i < index; ++i)
    {
        if (current == NULL) return NULL;
        current = current->next;
    }

    return current;
}

int row_count(t_row *first)
{
    int count = 0;
    t_row *current = first;
    while (current != NULL)
    {
        ++count;
        current = current->next;
    }

    return count;
}

void row_delete_at(t_row **first, int index)
{
    t_row *temp = *first;
    t_row *previous = NULL;

    if (index == 0)
    {
        *first = temp->next;
    }
    else
    {
        for (int i = 1; i <= index; ++i)
        {
            previous = temp;
            temp = temp->next;
            if (temp == NULL) return;
        }

        previous->next = temp->next;
    }

    cell_delete(temp->first);
    free(temp);

    t_row *current = *first;
    while (current != NULL)
    {
        --current->pos;
        current = current->next;
    }
}

void row_delete(t_row *first)
{
    if (first->next != NULL)
        row_delete(first->next);

    cell_delete(first->first);
    free(first);
}

t_cmdseq* cmdseq_create(t_command cmd)
{
    t_cmdseq *new = malloc(sizeof(t_cmdseq));
    new->cmd = cmd;
    new->next = NULL;

    return new;
}

t_cmdseq* cmdseq_add(t_cmdseq *first, t_command cmd)
{
    t_cmdseq *current = first;
    while (current->next != NULL)
        current = current->next;

    current->next = cmdseq_create(cmd);

    return current->next;
}

t_cmdseq *cmdseq_get(t_cmdseq *first, int index)
{
    if (index < 0) return NULL;

    t_cmdseq *current = first;
    for (int i = 0; i < index; ++i)
    {
        if (current == NULL) return NULL;
        current = current->next;
    }

    return current;
}

void cmdseq_delete(t_cmdseq *first)
{
    if (first->next != NULL)
        cmdseq_delete(first->next);
    free(first);
}

void add_rows(int count)
{
    if (count < 1) return;

    int last_row = g_table->rows_count;
    for (int i = 1; i <= count; ++i)
    {
        t_cell *new = cell_create(last_row + i, 1, "\0");
        for (int j = 2; j <= g_table->first->cols_count; ++j)
            cell_add(new, "\0");
        row_add(g_table->first, new);
    }

    g_table->rows_count += count;
}

void add_columns(int count)
{
    if (count < 1) return;
    t_row *current = g_table->first;
    while (current != NULL)
    {
        for (int i = 1; i <= count; ++i)
            cell_add(current->first, "\0");
        current->cols_count += count;
        current = current->next;
    }
}

void error()
{
    switch (g_error)
    {
    default:
        return;
    case ERR_NO_ARGS:
        fprintf(stderr, "ERROR: not enough arguments.\n");
        break;
    case ERR_NO_DELIMS:
        fprintf(stderr, "ERROR: delimiters weren't specified.\n");
        break;
    case ERR_READ_FILE:
        fprintf(stderr, "ERROR: unable to read from the file.\n");
        break;
    case ERR_UNKNOWN_CMD:
        fprintf(stderr, "ERROR: sequence has an unknown command: %s.\n", g_error_additional);
        break;
    case ERR_BAD_CMD_ARGS:
        fprintf(stderr, "ERROR: command doesn't have enough arguments or arguments can't be parsed: %s.\n", g_error_additional);
        break;
    case ERR_SELECTION_VAR_EMPTY:
        fprintf(stderr, "ERROR: can't run command [_]: variable is empty.\n");
        break;
    case ERR_CANT_DEF:
        fprintf(stderr, "ERROR: can't set value of variable %s: more then one cell is selected.\n", g_error_additional);
        break;
    case ERR_CANT_USE:
        fprintf(stderr, "ERROR: can't use value of variable %s: value isn't set.\n", g_error_additional);
        break;
    case ERR_NOT_NUMBER:
        fprintf(stderr, "ERROR: can't run command: variable %s has no number value.\n", g_error_additional);
        break;
    case ERR_SUB_NOT_SET:
        fprintf(stderr, "ERROR: can't run command sub: one of the variables %s is not set.\n", g_error_additional);
        break;
    case ERR_INFINITE_CYCLE:
        fprintf(stderr, "ERROR: program caused an infinite cycle.\n");
        break;
    }

    clean();
    exit(EXIT_FAILURE);
}

void clean()
{
    if (g_table != NULL && g_table->first != NULL)
    {
        row_delete(g_table->first);
        free(g_table);
    }
    if (g_cmds != NULL) cmdseq_delete(g_cmds);
    for (int i = 0; i < 10; ++i)
        free(v_var[i]);
}

bool is_delim(char value)
{
    for (int i = 0; i < (int) strlen(g_delims); ++i)
        if (g_delims[i] == value)
            return true;

    return false;
}

bool contains_specials(char *input)
{
    for (int i = 0; i < (int) strlen(input); ++i)
    {
        if (input[i] == g_delims[0] || input[i] == '"' || input[i] == '\\')
        {
            return true;
        }
    }
    return false;
}

char* fix_string(char *input)
{
    bool has_specials = false; int must_be_escaped = 0;
    for (int i = 0; i < (int) strlen(input); ++i)
    {
        if (input[i] == g_delims[0] || input[i] == '"' || input[i] == '\\')
        {
            has_specials = true;
            if (input[i] == '"' || input[i] == '\\') ++must_be_escaped;
        }
    }

    if (has_specials)
    {
        char *result = calloc(strlen(input) + 3 + must_be_escaped, sizeof(char));
        result[0] = '"';
        int pos = 1;
        for (int i = 0; i < (int) strlen(input); ++i)
        {
            if (input[i] == '"' || input[i] == '\\')
            {
                result[pos] = '\\';
                ++pos;
                result[pos] = input[i];
                ++pos;
            }
            else
            {
                result[pos] = input[i];
                ++pos;
            }
        }
        result[pos] = '"';
        result[pos + 1] = 0;

        input = realloc(input, strlen(result) + 1);
        strcpy(input, result);

        return result;
    }

    return input;
}

e_command_name get_cmd_name (char *command)
{
    if (strcmp(command, "irow") == 0) return irow;
    else if (strcmp(command, "arow") == 0) return arow;
    else if (strcmp(command, "drow") == 0) return drow;
    else if (strcmp(command, "icol") == 0) return icol;
    else if (strcmp(command, "acol") == 0) return acol;
    else if (strcmp(command, "dcol") == 0) return dcol;
    else if (strcmp(command, "set") == 0) return set;
    else if (strcmp(command, "clear") == 0) return clear;
    else if (strcmp(command, "swap") == 0) return swap;
    else if (strcmp(command, "sum") == 0) return sum;
    else if (strcmp(command, "avg") == 0) return avg;
    else if (strcmp(command, "count") == 0) return count;
    else if (strcmp(command, "len") == 0) return len;
    else if (strcmp(command, "def") == 0) return def;
    else if (strcmp(command, "use") == 0) return use;
    else if (strcmp(command, "inc") == 0) return inc;
    else if (strcmp(command, "[set]") == 0) return var_set;
    else if (strcmp(command, "goto") == 0) return cmd_goto;
    else if (strcmp(command, "iszero") == 0) return iszero;
    else if (strcmp(command, "sub") == 0) return sub;
    else g_error = 4;

    return 1000;
}

e_args_type get_args_type (e_command_name command)
{
    switch (command)
    {
    case irow:
        return None;
    case arow:
        return None;
    case drow:
        return None;
    case icol:
        return None;
    case acol:
        return None;
    case dcol:
        return None;
    case set:
        return String;
    case clear:
        return None;
    case swap:
        return TwoInt;
    case sum:
        return TwoInt;
    case avg:
        return TwoInt;
    case count:
        return TwoInt;
    case len:
        return TwoInt;
    case def:
        return Variable;
    case use:
        return Variable;
    case inc:
        return Variable;
    case var_set:
        return None;
    case cmd_goto:
        return Step;
    case iszero:
        return VariableAndStep;
    case sub:
        return TwoVariables;
    default:
        g_error = ERR_UNKNOWN_CMD;
    }

    return 0;
}

e_command_type get_cmd_type (e_command_name command)
{
    if (command >= irow && command <= dcol)
    {
        return ChangeStructure;
    }
    else if (command >= set && command <= len)
    {
        return ChangeContent;
    }
    else if (command >= def && command <= var_set)
    {
        return VariableOperation;
    }
    else if (command >= cmd_goto && command <= sub)
    {
        return CommandManagement;
    }
    else
    {
        g_error = ERR_UNKNOWN_CMD;
        return 0;
    }
}

bool str_starts(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}