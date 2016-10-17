#include <string.h>
#include <stdio.h>

#include <types.h>

var_type_t str_to_type(char *str) {
    if(!strcmp(str, "int")) {
        return INT;
    } else if(!strcmp(str, "ptr")) {
        return PTR;
    } else if(!strcmp(str, "pptr")) {
        return PPTR;
    } else {
        fprintf(stderr, "str_to_type: Invalid type: %s!\n", str);
        return INVLD;
    }
}

var_type_t get_type(char *str) {
    if(strstr(str, "int") == str) return INT;
    else if(strstr(str, "ptr") == str) return PTR;
    else if(strstr(str, "pptr") == str) return PPTR;
    else return INVLD;
}

char *type_to_str(var_type_t type) {
    switch(type) {
        case INT:   return "int";
        case PTR:   return "ptr";
        case PPTR:  return "pptr";
        case STR:   return "str";
        case INVLD: return "invld";
    }
    return "";
}
