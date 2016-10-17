#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <funcs.h>

/*
 * func: contains entire function as a string
 */
int parse_func(char *func) {
    char *header = (char *)malloc(strlen(func));
    strcpy(header, func);
    char *header_end = strchr(header, ')') + 1;
    *header_end = 0;

    printf("parse_func: header: %s\n", header);
    func_create_from_line(header);

    //char *body = header_end + 1;

    return 0;
}
