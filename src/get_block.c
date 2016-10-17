#include <stdio.h>
#include <string.h>

int get_block_cbraces(char *str, int *start, int *end) {
    char *b1 = strchr(str, '{');
    if(b1 == NULL) {
        fprintf(stderr, "get_block_cbraces: Could not locate first curly brace!\n");
        return 1;
    }
    *start = (b1 - str);
    int bn = 1;
    b1++;
    while(*b1 != '\0') {
        if(*b1 == '{') bn++;
        else if(*b1 == '}') bn--;
        b1++;
        if(bn == 0) {
            *end = (b1 - str);
            return 0;
        }
    }
    fprintf(stderr, "get_block_cbraces: Could not locate last curly brace!\n");
    return 1;
}
