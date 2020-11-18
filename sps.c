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
#include <string.h>

char* delims = " ";

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
            delims = argv[i + 1];

            if (strstr(delims, "\"") != NULL || strstr(delims, "\\") != NULL)
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
