#include <string.h>
#include <stdio.h>

#include <get_loc.h>

/*
 * Get a line of code (ending in a semi-colon) from text.
 */
int get_loc(char *text, char *str, int len) {
    char ch;
    int i = 0, tp = 0;
    int sq = 0, dq = 0; // Single quotes, double quotes
    while((ch = text[tp++]) != 0 && (ch != ';')) {
        if(ch == '/' && !(sq | dq)) {
            ch = text[tp++];
            if(ch == EOF) break;
            if(ch == '/') { // Single-line comment
                while((ch = text[tp++]) != 0 && (ch != '\n'));
            } else if(ch == '*') { // Multi-line comment
                while((ch = text[tp++]) != 0) {
                    if(ch == '*') {
                        ch = text[tp++];
                        if(ch == EOF) break;
                        if(ch == '/') break;
                    }
                }
                if(ch == EOF) {
                    fprintf(stderr, "get_loc: Could not find end to '/*' comment!\n");
                    return 1;
                }
            }
            continue;
        }
        if((ch == '\"') && !sq) dq = !dq;
        if((ch == '\'') && !dq) sq = !sq;
        //printf("[i: %d]", i);
        str[i++] = ch;
        if(i == len) {
            str[i] = 0;
            return -1;
        }
    }
    str[i] = 0;
    return tp;
}
