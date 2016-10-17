#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <variables.h>
#include <funcs.h>

func_ent_t *current_func = NULL;

int get_value(char *str, int64_t *value, char **var_ptr) {
    while(isspace(*str)) str++;
    // Check if assignment contains arithmetic
    if(strchr(str, ' ') || strchr(str+1, '+') || strchr(str, '*') || strchr(str, '/') || strchr(str+1, '-') /* it can be a negative number! */
            || strchr(str, '(') || strchr(str, ')')) {
        fprintf(stderr, "get_value: value [%s] is not a simple value\n", str);
        return 1;
    }
    if(isdigit(*str) || ((*str == '-' || *str == '+') && isdigit(*(str + 1)))) {
        *value = strtoll(str, NULL, 0);
        *var_ptr = 0;
        return 0;
    } else {
        if(current_func) {
            int i;
            for(i = 0; i < current_func->n_args; i++) {
                if(!strcmp(current_func->arg_names[i], str)) {
                    if(var_ptr == NULL) return 0;
                    else {
                        *var_ptr = current_func->arg_fulln[i];
                        return 0;
                    }
                }
            }
        }
        var_ent_t *var = var_find_l(str);
        if(!var) {
            var = var_find_g(str);
            if(!var) {
                fprintf(stderr, "get_value: could not find variable: %s\n", str);
                return 1;
            }
        }
        if(var_ptr == NULL) {
            return 0;
        } else {
            *var_ptr = var->fulln;
            return 0;
        }
    }
}
