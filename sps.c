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
    char string_arg[1000]; // if command_args_type is String
    int int_args[2]; // if command_args_type is TwoInt
    int variable; // if command_args_type is Variable
} Command;
//endregion

//region List methods
void add(ListElementString *head, char *value)
{
    ListElementString *curr = head;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }

    curr->next = (ListElementString *) malloc(sizeof(ListElementString));
    curr->next->value = value;
    curr->next->next = NULL;
}
//endregion

ListElementString strsplit(char *string)
{

}

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

}

char* Delims = " ";

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

            return;
        }
    }
}

int main(int argc, char *argv[])
{
    read_delims(argc, argv);

    return 0;
}
