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

//region Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
//endregion

//region Defines
#define ROW 0
#define COL 1
//endregion

//region Enums
typedef enum _commandType
{
    ChangeStructure,
    ChangeContent,
    VariableOperation,
    SelectionOperation
} CommandType;

typedef enum _commandName
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
    select // for all selection commands
} CommandName;

typedef enum _commandArgsType
{
    None,
    String,
    TwoInt,
    Variable
} CommandArgsType;

typedef enum _selectionType {
    CellSelection,
    RowSelection,
    ColumnSelection,
    WindowSelection,
    Full,
    Min,
    Max,
    FindString,
    FromVariable
} SelectionType;
//endregion

//region Data types
typedef struct _command
{
    CommandType type;
    CommandName name;
    CommandArgsType command_args_type;
    char string_arg[1001]; // if command_args_type is String or type is SelectionOperation
    int int_args[2]; // if command_args_type is TwoInt
    int variable; // if command_args_type is Variable
    SelectionType selection_type;
    int R1, C1, R2, C2;
} Command;

typedef struct _commandSequence
{
    Command value;
    struct _commandSequence* next;
} CommandSequence;

typedef struct _cell
{
    char* value;
    struct _cell* next;
} Cell;

typedef struct _row
{
    Cell* first_cell;
    struct _row* next;
} Row;

typedef struct _table
{
    Row* first_row;
} Table;

typedef struct _selection {
    int top_left[2];
    int down_right[2];
} Selection;
//endregion

//region List methods
CommandSequence* create_cmdseq(Command cmd)
{
    CommandSequence* cmdseq = malloc(sizeof(CommandSequence));
    if (cmdseq != NULL)
    {
        cmdseq->value = cmd;
        cmdseq->next = NULL;
    }
    return cmdseq;
}

void delete_cmdseq(CommandSequence* cmdseq)
{
    if (cmdseq->next != NULL)
    {
        delete_cmdseq(cmdseq->next);
    }
    free(cmdseq);
}

void add_cmdseq(CommandSequence* cmdseq, Command command)
{
    CommandSequence* current = cmdseq;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = create_cmdseq(command);
}

Cell* create_row(char *first_value)
{
    Cell* row = malloc(sizeof(Cell));
    if (row != NULL)
    {
        row->value = malloc(strlen(first_value) + 1);
        row->value = first_value;
        row->next = NULL;
    }
    return row;
}

void add_cell(Cell* firstCell, char* value)
{
    Cell* current = firstCell;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = create_row(value);
}

int cell_count(Cell* first_cell)
{
    Cell* current = first_cell;
    int count = 0;
    while (current != NULL)
    {
        ++count;
        current = current->next;
    }

    return count;
}

Row* create_table(Cell* first_cell)
{
    Row* table = malloc(sizeof(Row));
    if (table != NULL)
    {
        table->first_cell = malloc(sizeof(first_cell));
        table->first_cell = first_cell;
        table->next = NULL;
    }
    return table;
}

void add_row(Row* firstRow, Cell* firstCell)
{
    Row * current = firstRow;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = create_table(firstCell);
}

int rows_count(Row* firstRow)
{
    int count = 1;
    while (firstRow->next != NULL)
    {
        ++count;
        firstRow = firstRow->next;
    }

    return count;
}

Row* get_row(Row* table, int num)
{
    if (num < 0 || num >= rows_count(table)) return NULL;
    Row* result = table;
    for (int i = 0; i < num; ++i)
        result = result->next;

    return result;
}

Cell* get_cell(Cell* row, int num)
{
    if (num < 0 || num >= cell_count(row)) return NULL;
    Cell* result = row;
    for (int i = 0; i < num; ++i)
        result = result->next;

    return result;
}

//endregion

//region Utils
int starts_with(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

void split_to_array(char* input, char output[4][32], char delim, int output_size)
{
    int len = strlen(input);
    int charNum = 0;
    int tokenNum = 0;
    for (int i = 0; i < len; ++i)
    {
        if (tokenNum >= output_size)
        {
            break;
        }

        if (input[i] != delim)
        {
            output[tokenNum][charNum] = input[i];
            ++charNum;
        }
        else
        {
            output[tokenNum][charNum] = '\0';
            ++tokenNum;
            charNum = 0;
        }
    }
}
//endregion

//region Global variables
char* Delims = " ";
int HadCustomDelims = 0;
char* Variables[10];

int RowCount = 0;
int ColumnCount = 0;

Selection CurrentSelection;
Selection VariableSelection;
int vs_isset = 0;
//endregion

//region Command operations
CommandName str_to_cmd_name(char* command)
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
    else return -1;
}

CommandArgsType get_args_type(CommandName command)
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
    default:
        return -1;
    }
}

CommandType get_cmd_type(CommandName command)
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
    else
    {
        return -1;
    }
}

Command str_to_cmd(char* input)
{
    char name[64], args[1000];
    int res = sscanf(input, "%s %s", name, args);
    Command converted;

    if (res == 0)
    {
        fprintf(stderr, "ERROR: something went wrong while converting '%s' to a command.\n", input);
        exit(EXIT_FAILURE);
    }

    if (input[0] == '[') // selection commands
    {
        converted.name = select;
        converted.type = SelectionOperation;
        if (strcmp(input, "[_,_]") == 0)
        {
            converted.selection_type = Full;
        }
        else if (strcmp(input, "[min]") == 0)
        {
            converted.selection_type = Min;
        }
        else if (strcmp(input, "[max]") == 0)
        {
            converted.selection_type = Max;
        }
        else if (strcmp(input, "[_]") == 0)
        {
            converted.selection_type = FromVariable;
        }
        else if (starts_with("[_,", input) == 1) //[_,C]
        {
            int col = 0;
            res = sscanf(input, "[_,%d]", &col);
            if (res == 0)
            {
                fprintf(stderr, "ERROR: something went wrong while converting '%s' to a command.\n", input);
                exit(EXIT_FAILURE);
            }

            converted.selection_type = ColumnSelection;
            converted.C1 = col;
        }
        else if (starts_with("[find ", input) == 1)
        {
            converted.selection_type = FindString;

            int len = strlen(input) - 6;
            char str[len];
            for (int i = 6; i < (int) strlen(input) - 1; ++i) // -1 because we don't need a closing bracket
                str[i - 6] = input[i];
            str[len - 1] = 0;

            strcpy(converted.string_arg, str);
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
                converted.selection_type = ColumnSelection;
                C1 = atoi(content[1]);

                converted.C1 = C1;
                return converted;
            }
            R1 = atoi(content[0]);
            if (R1 <= 0)
            {
                fprintf(stderr, "ERROR: unable to parse command %s\n", input);
                exit(EXIT_FAILURE);
            }
            converted.R1 = R1;

            if (content[1][0] == '_' && content[2][0] == 0 && content[3][0] == 0) //[R,_]
            {
                converted.selection_type = RowSelection;
                return converted;
            }
            C1 = atoi(content[1]);
            if (C1 <= 0)
            {
                fprintf(stderr, "ERROR: unable to parse command %s\n", input);
                exit(EXIT_FAILURE);
            }
            converted.C1 = C1;

            if (content[2][0] == 0)
            {
                converted.selection_type = CellSelection;
                return converted;
            } //[R,C]

            if (content[2][0] == '_') R2 = RowCount;
            else R2 = atoi(content[2]);

            converted.selection_type = WindowSelection;

            if (content[3][0] == '_') C2 = ColumnCount;
            else C2 = atoi(content[3]);

            if (R2 <= 0 || C2 <= 0)
            {
                fprintf(stderr, "ERROR: unable to parse command %s\n", input);
                exit(EXIT_FAILURE);
            }
            if (R1 > R2 || C1 > C2)
            {
                fprintf(stderr, "ERROR: R1 or C1 can't be bigger then R2 or C2 in the command %s\n", input);
                exit(EXIT_FAILURE);
            }
            converted.R2 = R2;
            converted.C2 = C2;

            return converted;
        }

        return converted;
    }

    CommandName commandName = str_to_cmd_name(name);
    if ((int) commandName == -1)
    {
        fprintf(stderr, "ERROR: '%s' isn't a valid command.\n", name);
        exit(EXIT_FAILURE);
    }
    converted.name = commandName;

    converted.command_args_type = get_args_type(commandName);
    converted.type = get_cmd_type(commandName);

    if (converted.command_args_type == None && res == 1)
    {
        return converted;
    }

    if (converted.command_args_type == TwoInt)
    {
        if (res == 1)
        {
            fprintf(stderr, "ERROR: not enough arguments for the command '%s'.", name);
            exit(EXIT_FAILURE);
        }

        int R = 0, C = 0;
        res = sscanf(args, "[%d,%d]", &R, &C);
        if (res != 2)
        {
            fprintf(stderr, "ERROR: not enough arguments for the command '%s'.", name);
            exit(EXIT_FAILURE);
        }

        converted.int_args[0] = R;
        converted.int_args[1] = C;
    }

    if (converted.command_args_type == String)
    {
        if (res == 1)
        {
            fprintf(stderr, "ERROR: not enough arguments for the command '%s'.", name);
            exit(EXIT_FAILURE);
        }

        if (args[0] == '\"')
        {
            res = sscanf(input, "%s \"%[^\"]\"", name, args); // rescan using another template (with quotes)
            if (res != 2)
            {
                fprintf(stderr, "ERROR: something went wrong while parsing arguments for the command '%s'.", name);
                exit(EXIT_FAILURE);
            }

            strcpy(converted.string_arg, args);
        }
        else
        {
            strcpy(converted.string_arg, args);
        }
    }

    if (converted.command_args_type == Variable)
    {
        int variable = args[1] - 48;
        if (variable < 0 || variable > 9 || strlen(args) > 2)
        {
            fprintf(stderr, "ERROR: %s has no correct variable argument.", input);
            exit(EXIT_FAILURE);
        }

        converted.variable = variable;
    }

    return converted;
}
//endregion

int isDelim(char value)
{
    for (int i = 0; i < (int) strlen(Delims); ++i)
    {
        if (value == Delims[i]) return 1;
    }

    return 0;
}

/*
 * Reads delimiters from the argument -d to the global variable delims.
 * Exits if:
 * Delimiter wasn't specified in the argument.
 * Delimiter contains forbidden symbols (" or \").
 */
void read_delims(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-d") == 0)
        {

            if (i + 1 >= argc)
            {
                fprintf(stderr, "ERROR: delimeter wasn't specified in the argument '-d'.\n");
                exit(EXIT_FAILURE);
            }
            Delims = argv[i + 1];

            if (strstr(Delims, "\"") != NULL || strstr(Delims, "\\") != NULL)
            {
                fprintf(stderr, "ERROR: delimiter can't be \" or \\.\n");
                exit(EXIT_FAILURE);
            }

            HadCustomDelims = 1;
            return;
        }
    }
}

/*
 * Loads the table from the file, specified by the last argument
 */
Row* load_table(int argc, char* argv[])
{
    char* fileName = argv[argc - 1];
    FILE* fptr;
    fptr = fopen(fileName, "r");

    if (fptr == NULL)
    {
        fprintf(stderr, "ERROR: specified FILE can't be opened or FILE wasn't specified.\n");
        exit(EXIT_FAILURE);
    }

    char nextChar = (char)fgetc(fptr);
    int cellLength = 1;
    char *cellStr = malloc(1);
    cellStr[0] = '\0';
    Cell* row = malloc(sizeof(Cell));
    Row* table = malloc(sizeof(Row));

    int cell_row_pos = 0;
    int row_pos = 0;
    int inQuotes = 0;
    int nextCharEscaped = 0;
    while (1) // time for some INFINITE cycles (no). sry im bored.
    {
        if (isDelim(nextChar) && !inQuotes) // here we'll add a new Cell to a Row (which is currently a List of Cells)
        {
            if (cell_row_pos == 0)
            {
                row = create_row(cellStr);
            }
            else
            {
                add_cell(row, cellStr);
            }

            ++cell_row_pos;

            cellLength = 1; // and we'll reallocate memory for the next cell string
            cellStr = malloc(1);
            cellStr[0] = '\0';
        }
        else if (nextChar == '\n' || nextChar == EOF)
        {
            add_cell(row, cellStr);

            if (row_pos == 0)
            {
                table = create_table(row);
            }
            else
            {
                add_row(table, row);
            }

            if (nextChar == EOF) break;

            ++row_pos;

            row = malloc(sizeof(Cell));

            cell_row_pos = 0;
            cellLength = 1; // and we'll allocate memory for the next cell string
            cellStr = malloc(1);
            cellStr[0] = '\0';

            inQuotes = 0;
        }
        else
        {
            if (nextChar == '"' && !nextCharEscaped) inQuotes = !inQuotes;

            if (inQuotes && nextChar == '\\')
            {
                nextCharEscaped = 1;
            }

            // dynamically increasing the size of a cell string
            cellStr = realloc(cellStr, cellLength + 1);
            cellStr[cellLength] = '\0';
            cellStr[cellLength - 1] = nextChar;
            ++cellLength;

            nextCharEscaped = 0;
        }

        nextChar = (char)fgetc(fptr);
    }

    fclose(fptr);

    return table;
}

/*
 * Reads commands from arguments and returns the command sequence.
 */
CommandSequence* read_cmds(int argc, char* argv[])
{
    int cmdArg = 1; // start from
    if (strcmp(argv[cmdArg], "-d") == 0) cmdArg = 3;

    if (argc <= cmdArg)
    {
        fprintf(stderr, "ERROR: not enough arguments!\n");
        exit(EXIT_FAILURE);
    }

    int commandStrLength = 0;
    for (int i = cmdArg; i < argc - 1; ++i) // the final argument is FILE
    {
        commandStrLength += (int) strlen(argv[i]) + 1;
    }
    char commandStr[commandStrLength];
    commandStr[0] = 0;
    for (int i = cmdArg; i < argc - 1; ++i)
    {
        strncat(commandStr, argv[i], strlen(argv[i]) + 1);
        strncat(commandStr, " ", 1);
    }

    // checks if there is only one command (w/o quotes)
    if (argv[cmdArg][0] != '\'')
    {
        Command cmd = str_to_cmd(commandStr);
        CommandSequence* result = create_cmdseq(cmd);
        return result;
    }
    else
    {
        //stripping the first and the last character
        memmove(commandStr, commandStr + 1, commandStrLength - 2);
        commandStr[commandStrLength - 3] = 0;

        //now tokenize and parse
        char* token;
        token = strtok(commandStr, ";");
        Command nextCmd = str_to_cmd(token);
        CommandSequence* result = create_cmdseq(nextCmd);

        token = strtok(NULL, ";");
        while (token != NULL)
        {
            nextCmd = str_to_cmd(token);
            add_cmdseq(result, nextCmd);
            token = strtok(NULL, ";");
        }

        return result;
    }
}

//region Table operations
void add_columns_end(Row* table, unsigned int count)
{
    if (count < 1) return;
    Row* current = table;
    while (current != NULL)
    {
        for (unsigned int i = 1; i <= count; ++i)
            add_cell(current->first_cell, "\0");
        current = current->next;
    }

    ColumnCount += count;
}

void add_rows_end(Row* table, unsigned int count)
{
    if (count < 1) return;
    for (unsigned int i = 1; i <= count; ++i)
    {
        Cell* new = create_row("\0");
        for (int j = 2; j <= ColumnCount; ++j)
            add_cell(new, "\0");
        add_row(table, new);
    }

    RowCount += count;
}

/*
 * If some rows have more cells then another, this procedure will add empty cells, where it's required
 */
void fix_table(Row* table)
{
    Row* currentRow = table;
    // first get the longest row
    int max = 0;
    while (currentRow != NULL)
    {
        int len = cell_count(currentRow->first_cell);
        if (len > max) max = len;
        currentRow = currentRow->next;
    }
    ColumnCount = max;

    currentRow = table;
    while (currentRow != NULL)
    {
        int len = cell_count(currentRow->first_cell);
        if (len < max)
        {
            for (int i = 0; i < max - len; ++i)
            {
                add_cell(currentRow->first_cell, "\0");
            }
        }
        currentRow = currentRow->next;
    }
}
//endregion

/*
 * Debug function, which prints table to stdin.
 */
void print_table(Row* table)
{
    Row* currentRow = table;
    while (currentRow != NULL)
    {
        Cell* currentCell = currentRow->first_cell;
        while (currentCell->next != NULL)
        {
            printf("%s%c", currentCell->value, Delims[0]);
            currentCell = currentCell->next;
        }
        printf("%s\n", currentCell->value);
        currentRow = currentRow->next;
    }
}

void change_selection(Row* table, Command cmd)
{
    int row_count = 0, col_count = 0;
    Row* row = NULL;
    Cell* cell = NULL;

    char* endptr;
    errno = 0;
    int found = 0;

    long long value = 0;
    long long max = LLONG_MIN;
    long long min = LLONG_MAX;

    int row_pos = 0;
    int col_pos = 0;

    switch (cmd.selection_type)
    {
    case CellSelection:
        if (ColumnCount < cmd.C1) add_columns_end(table, cmd.C1 - ColumnCount);
        if (RowCount < cmd.R1) add_rows_end(table, cmd.R1 - ColumnCount);
        CurrentSelection.top_left[ROW] = cmd.R1;
        CurrentSelection.top_left[COL] = cmd.C1;
        CurrentSelection.down_right[ROW] = cmd.R1;
        CurrentSelection.down_right[COL] = cmd.C1;
        break;
    case RowSelection:
        if (RowCount < cmd.R1) add_rows_end(table, cmd.R1 - ColumnCount);
        CurrentSelection.top_left[ROW] = cmd.R1;
        CurrentSelection.top_left[COL] = 1;
        CurrentSelection.down_right[ROW] = cmd.R1;
        CurrentSelection.down_right[COL] = ColumnCount;
        break;
    case ColumnSelection:
        if (ColumnCount < cmd.C1) add_columns_end(table, cmd.C1 - ColumnCount);
        CurrentSelection.top_left[ROW] = 1;
        CurrentSelection.top_left[COL] = cmd.C1;
        CurrentSelection.down_right[ROW] = RowCount;
        CurrentSelection.down_right[COL] = cmd.C1;
        break;
    case WindowSelection:
        if (ColumnCount < cmd.C2) add_columns_end(table, cmd.C2 - ColumnCount);
        if (RowCount < cmd.R2) add_rows_end(table, cmd.R2 - RowCount);
        CurrentSelection.top_left[ROW] = cmd.R1;
        CurrentSelection.top_left[COL] = cmd.C1;
        CurrentSelection.down_right[ROW] = cmd.R2;
        CurrentSelection.down_right[COL] = cmd.C2;
        break;
    case Full:
        CurrentSelection.top_left[ROW] = 1;
        CurrentSelection.down_right[COL] = 1;
        CurrentSelection.down_right[ROW] = RowCount;
        CurrentSelection.down_right[COL] = ColumnCount;
        break;
    case Min:
        row_count = CurrentSelection.top_left[ROW] - CurrentSelection.down_right[ROW];
        col_count = CurrentSelection.top_left[COL] - CurrentSelection.down_right[COL];
        for (int row_num = CurrentSelection.top_left[ROW] - 1; row_num < CurrentSelection.down_right[ROW]; ++row_num)
        {
            row = get_row(table, row_num);
            for (int col_num = CurrentSelection.top_left[COL] - 1; col_num < CurrentSelection.down_right[COL]; ++col_num)
            {
                cell = get_cell(row->first_cell, col_num);
                value = strtoll(cell->value, &endptr, 10);

                if (*endptr == '\0' && cell->value[0] != '\0')
                {
                    found = 1;
                    if (value < min)
                    {
                        min = value;
                        row_pos = row_num; col_pos = col_num;
                    }
                }
            }
        }

        if (found == 0)
        {
            fprintf(stderr, "ERROR: unable to run command [min] - no numbers were found in the selection!\n");
            exit(EXIT_FAILURE);
        }

        CurrentSelection.top_left[ROW] = row_pos + 1;
        CurrentSelection.top_left[COL] = col_pos + 1;
        CurrentSelection.down_right[ROW] = row_pos + 1;
        CurrentSelection.down_right[COL] = col_pos + 1;

        break;
    case Max:
        break;
    case FindString:
        break;
    case FromVariable:
        if (!vs_isset)
        {
            fprintf(stderr, "ERROR: unable to execute command [_], variable is not set.\n");
            exit(EXIT_FAILURE);
        }
        CurrentSelection = VariableSelection;
        break;
    }
}

void run_commands(Row* table, CommandSequence* cmdseq)
{
    CommandSequence* current_command = cmdseq;
    while (current_command != NULL)
    {
        Command cmd = current_command->value;
        switch (cmd.type)
        {
        case ChangeStructure:
            break;
        case ChangeContent:
            break;
        case VariableOperation:
            break;
        case SelectionOperation:
            change_selection(table, cmd);
            break;
        }

        current_command = current_command->next;
    }
}

int main(int argc, char* argv[])
{
    read_delims(argc, argv);
    Row* table = load_table(argc, argv);
    fix_table(table);
    RowCount = rows_count(table);

    CurrentSelection.top_left[ROW] = 1;
    CurrentSelection.top_left[COL] = 1;
    CurrentSelection.down_right[ROW] = RowCount;
    CurrentSelection.down_right[COL] = ColumnCount;

    CommandSequence* cmdseq = read_cmds(argc, argv);
    run_commands(table, cmdseq);

    print_table(table);

    return 0;
}