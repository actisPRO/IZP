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
} CommandArgsType;
//endregion

//region Data types
typedef struct _command {
    CommandType type;
    CommandName name;
    CommandArgsType command_args_type;
    char string_arg[1000]; // if command_args_type is String
    int int_args[2]; // if command_args_type is TwoInt
} Command;
//endregion

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
