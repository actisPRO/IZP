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
typedef enum _commandType {
    ChangeStructure,
    ChangeContent,
    VariableOperation
} CommandType;

typedef enum _commandName {
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

typedef enum _commandArgsType {
    None,
    String,
    TwoInt,
    Variable
} CommandArgsType;
//endregion

//region Data types
typedef struct _strListEl {
    char *value;
    struct _strListEl *next;
} ListElementString;

typedef struct _command {
    CommandType type;
    CommandName name;
    CommandArgsType command_args_type;
    char *string_arg; // if command_args_type is String
    int int_args[2]; // if command_args_type is TwoInt
    int variable; // if command_args_type is Variable
} Command;

typedef struct _commandElList {
    Command *value;
    struct _commandElList *next;
} CommandElementList;
//endregion

//region List methods
/*
 * Creates a new empty List of strings
 */
ListElementString* string_list()
{
    ListElementString *result = NULL;
    result = malloc(sizeof(ListElementString));
    result->value = NULL;
    result->next = NULL;

    return result;
}

CommandElementList* command_list()
{
    CommandElementList *result = NULL;
    result = malloc(sizeof(CommandElementList));
    result->value = NULL;
    result->next = NULL;

    return result;
}

/*
 * Adds an element to the List of strings
 */
void strlist_add(ListElementString *head, char *value)
{
    if (head->value == NULL)
    {
        head->value = malloc(strlen(value) + 1);
        head->value = value;
        return;
    }

    ListElementString* curr = head;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }

    curr->next = (ListElementString*)malloc(sizeof(ListElementString));
    if (value == NULL)
    {
        curr->next->value = NULL;
    }
    else
    {
        curr->next->value = malloc(strlen(value) + 1);
        curr->next->value = value;
    }
    curr->next->next = NULL;
}

void cmdlist_add(CommandElementList *head, Command *value)
{
    if (head->value == NULL)
    {
        head->value = malloc(sizeof(Command));
        head->value = value;
        return;
    }

    CommandElementList * curr = head;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }

    curr->next = malloc(sizeof(CommandElementList));
    if (value == NULL)
    {
        curr->next->value = NULL;
    }
    else
    {
        curr->next->value = malloc(sizeof(Command));
        curr->next->value = value;
    }
    curr->next->next = NULL;
}

/*
 * Sets the value of the specified List element
 */
void strlist_set(ListElementString *head, int index, char *value)
{
    ListElementString* curr = head;
    int i = 0;
    while (i != index || curr->next != NULL)
    {
        curr = curr->next;
        ++i;
    }

    curr->value = malloc(strlen(value) + 1);
    curr->value = value;
}

/*
 * Returns number of elements in the List.
 */
int strlist_count(ListElementString *head)
{
    int count = 0;
    if (head->value == NULL) return 0;

    ListElementString* current = head;
    while (current->next != NULL)
    {
        ++count;
        current = current->next;
    }

    return count;
}
//endregion

CommandName str_to_cmd_name(char *command)
{
    if (strcmp(command, "irow") == 0)       return irow;
    else if (strcmp(command, "arow") == 0)  return arow;
    else if (strcmp(command, "drow") == 0)  return drow;
    else if (strcmp(command, "icol") == 0)  return icol;
    else if (strcmp(command, "acol") == 0)  return acol;
    else if (strcmp(command, "dcol") == 0)  return dcol;
    else if (strcmp(command, "set") == 0)   return set;
    else if (strcmp(command, "clear") == 0) return clear;
    else if (strcmp(command, "swap") == 0)  return swap;
    else if (strcmp(command, "sum") == 0)   return sum;
    else if (strcmp(command, "avg") == 0)   return avg;
    else if (strcmp(command, "count") == 0) return count;
    else if (strcmp(command, "len") == 0)   return len;
    else if (strcmp(command, "def") == 0)   return def;
    else if (strcmp(command, "use") == 0)   return use;
    else if (strcmp(command, "inc") == 0)   return inc;
    else if (strcmp(command, "[set]") == 0) return var_set;
    else                                    return -1;
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

Command str_to_cmd(char *input)
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

            converted.string_arg = args;
        }
        else
        {
            converted.string_arg = args;
        }

        return converted;
    }
}

//region Global variables
char* Delims = " ";
int HadCustomDelims = 0;
//endregion

/*
 * Reads delimiters from the argument -d to the global variable delims.
 * Exits if:
 * Delimiter wasn't specified in the argument.
 * Delimiter contains forbidden symbols (" or \").
 */
void read_delims(int argc, char *argv[])
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
 * Reads commands from the sequence and adds them to a new List.
 * Returns list of strings
 */
ListElementString* read_commands(int argc, char *argv[])
{
    ListElementString* resultHead = string_list();
    int startArg = 1;
    if (HadCustomDelims == 1) startArg = 3;

    if (argc <= startArg)
    {
        fprintf(stderr, "ERROR: Command sequence wasn't specified!\n");
        exit(EXIT_FAILURE);
    }

    if (argv[startArg][0] != '\'')
    {
        strlist_add(resultHead, argv[startArg]);
        return resultHead;
    }

    //find the last argument (must end with ')
    int lastArg = startArg;
    while (argv[lastArg][strlen(argv[lastArg]) - 1] != '\'' && lastArg < argc - 1)
    {
        ++lastArg;
    }

    int wasClosed = 0;
    if (argv[lastArg][strlen(argv[lastArg]) - 1] == '\'') wasClosed = 1;

    int len = 0;
    for (int i = startArg; i <= lastArg; ++i)
    {
        len += strlen(argv[i]);
    }
    len += lastArg - startArg; // spaces

    char commandString[len];
    commandString[0] = 0;

    for (int i = startArg; i <= lastArg; ++i)
    {
        strncat(commandString, argv[i], strlen(argv[i]));
        strncat(commandString, " ", 1);
    }
    len = strlen(commandString);

    //strip the first and the last char
    memmove(commandString, commandString + 1, len - 2);
    commandString[len - 2 - wasClosed] = 0;

    char *token;
    token = strtok(commandString, ";");
    while (token != NULL)
    {
        strlist_add(resultHead, token);
        token = strtok(NULL, ";");
    }
    // tokenize and add to the list

    return resultHead;
}

int main(int argc, char *argv[])
{
    read_delims(argc, argv);
    ListElementString* str_commands = read_commands(argc, argv);

    return 0;
}