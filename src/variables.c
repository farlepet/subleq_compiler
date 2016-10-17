#include <string.h>
#include <ctype.h>

#include <variables.h>
#include <values.h>

int var_create_from_line(char *line, int is_global) {
    char tmp[512];
    strcpy(tmp, line);
    char *code = strtok(tmp, ";"); // Get code up until the semicolon
    while(isspace(*code)) code++; // Remove trailing whitespace

    char *type = strtok(code, "\t ");
    printf("var_create_from_line: var type: %s (%s)\n", type, is_global ? "global" : "local");

    var_type_t type_code;

    type_code = str_to_type(type);
    if(type_code == INVLD) return 1;

    char *varstr, *varstr_s;
    varstr = strtok_r(type + strlen(type) + 1, ",", &varstr_s);
    do {
        while(isspace(*varstr)) varstr++; // Remove trailing whitespace
        printf("  variable assignment: %s\n", varstr);
        char *name = strtok(varstr, "= ");
        char *value;
        if((value = strtok(NULL, "= ")) != NULL) {
            int64_t val;
            char *var_ptr = NULL;
            if(!get_value(value, &val, &var_ptr)) {
                printf("   value: %ld (%s)\n", val, var_ptr);
                if(is_global) {
                    if(var_create_g(name, type_code, val, var_ptr)) {
                        fprintf(stderr, "var_create_from_line: [var_create_g] could not create global variable!\n");
                        return 1;
                    }
                } else {
                    if(var_create_l(name, type_code, val, var_ptr)) {
                        fprintf(stderr, "var_create_from_line: [var_create_l] could not create local variable!\n");
                        return 1;
                    }
                }
            } else {
                fprintf(stderr, "var_create_from_line: multi-part initial assignments are not yet supported!");
                return 1;
            }
        } else {
            if(is_global) {
                if(var_create_g(name, type_code, 0, NULL)) {
                    fprintf(stderr, "var_create_from_line: [var_create_g] could not create global variable!\n");
                }
            } else {
                if(var_create_l(name, type_code, 0, NULL)) {
                    fprintf(stderr, "var_create_from_line: [var_create_l] could not create local variable!\n");
                    return 1;
                }
            }
        }
    } while((varstr = strtok_r(NULL, ",", &varstr_s)) != NULL);

    return 0;
}
