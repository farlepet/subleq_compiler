#ifndef TYPES_H
#define TYPES_H


typedef enum {
    INVLD = -1, // Invalid type
    INT = 0,    // Generic integer
    /*PTR,        // Integer pointer
    PPTR,       // Integer pointer pointer
    STR         // String*/
} var_type_t;

var_type_t str_to_type(char *str);

var_type_t get_type(char *str);
char *type_to_str(var_type_t type);

#endif
