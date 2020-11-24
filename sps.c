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
//endregion

//region Enums
typedef enum _commandType
{
    ChangeStructure,
    ChangeContent,
    VariableOperation
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
    var_set // [set]
} CommandName;

typedef enum _commandArgsType
{
    None,
    String,
    TwoInt,
    Variable
} CommandArgsType;
//endregion

//region Data types
typedef struct _command
{
    CommandType type;
    CommandName name;
    CommandArgsType command_args_type;
    char string_arg[1001]; // if command_args_type is String
    int int_args[2]; // if command_args_type is TwoInt
    int variable; // if command_args_type is Variable
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

int row_length(Cell* first_cell)
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

//endregion

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
    char name[32], args[1000];
    int res = sscanf(input, "%s %s", name, args);
    Command converted;

    if (res == 0)
    {
        fprintf(stderr, "ERROR: something went wrong while converting '%s' to a command.\n", input);
        exit(EXIT_FAILURE);
    }

    if (res == 1)
    {
        // check if is a selection command
    }

    CommandName commandName = str_to_cmd_name(name);
    if (commandName == -1)
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

        return converted;
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

        return converted;
    }
}

//region Global variables
char* Delims = " ";
int HadCustomDelims = 0;
//endregion

int isDelim(char value)
{
    for (int i = 0; i < strlen(Delims); ++i)
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
    while (1)
    {
        if (isDelim(nextChar)) // here we'll add a new Cell to a Row (which is currently a List of Cells)
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
            cellLength = 1; // and we'll reallocate memory for the next cell string
            cellStr = malloc(1);
            cellStr[0] = '\0';
        }
        else
        {
            // dynamically increasing the size of a cell string
            cellStr = realloc(cellStr, cellLength + 1);
            cellStr[cellLength] = '\0';
            cellStr[cellLength - 1] = nextChar;
            ++cellLength;
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
        int len = row_length(currentRow->first_cell);
        if (len > max) max = len;
        currentRow = currentRow->next;
    }

    currentRow = table;
    while (currentRow != NULL)
    {
        int len = row_length(currentRow->first_cell);
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

int main(int argc, char* argv[])
{
    read_delims(argc, argv);
    Row* table = load_table(argc, argv);
    CommandSequence* cmdseq = read_cmds(argc, argv);
    fix_table(table);

    return 0;
}