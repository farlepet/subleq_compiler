/*
 * variables_l.c: Local variable handler
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <variables.h>
#include <globals.h>

int n_local_vars = 0;
var_ent_t local_vars[MAX_LOCAL_VARS]; // Local variables
char *curr_fname = ""; // Current function name

int var_create_l(char *name, var_type_t type, uint64_t value, char *var_ptr) {
    strcpy(local_vars[n_local_vars].name, name);
    sprintf(local_vars[n_local_vars].fulln, "%s.vars.%s.%s", curr_fname, type_to_str(type), name);
    local_vars[n_local_vars].type = type;
    if(var_ptr != NULL) { // Variable is set as a pointer to another variable
        int i;
        for(i = 0; i < current_func->n_args; i++) {
            if(!strcmp(current_func->arg_fulln[i], var_ptr)) {
                local_vars[n_local_vars].value  = -(i+1);
                local_vars[n_local_vars].is_var = 1;
                n_local_vars++;
                return 0;
            }
        }
        for(i = 0; i < n_local_vars; ++i) { // Find variable index
            //printf("var_create_l: testing var %i: %s ?= %s\n", i, local_vars[i].name, var_ptr);
            if(!strcmp(local_vars[i].fulln, var_ptr)) {
                local_vars[n_local_vars].value  = i;
                local_vars[n_local_vars].is_var = 1;
                n_local_vars++;
                return 0;
            }
        }
        fprintf(stderr, "var_create_l: Could not find variable in order to create pointer: %s\n", var_ptr);
        return 1;
    } else { // Variable is just an integer value
        local_vars[n_local_vars].value  = value;
        local_vars[n_local_vars].is_var = 0;
    }
    n_local_vars++;
    return 0;
}

var_ent_t *var_find_l(char *name) {
    int i;
    for(i = 0; i < n_local_vars; ++i) { // Find variable index
        if(!strcmp(local_vars[i].name, name)) {
            return &local_vars[i];
        }
    }
    return NULL;
}

// NOTE: Return value is valid until the function is called again
char *var_let_asm_name_l(var_ent_t *var) {
    return var->fulln;
}

int var_asm_len_l(FILE *f, var_ent_t *var) {
    switch(var->type) {
        case INT:
            fprintf(f, "_lvars.int.%s:\n. %ld\n", var->name, var->value);
            break;

        case PTR:
            if(var->is_var) fprintf(f, "_lvars.ptr.%s:\n. %s\n", var->name, var_let_asm_name_l(&local_vars[var->value]));
            else fprintf(f, "_lvars.ptr.%s:\n. %ld\n", var->name, var->value);
            break;

        case PPTR:
            if(var->is_var) fprintf(f, "_lvars.pptr.%s:\n. %s\n", var->name, var_let_asm_name_l(&local_vars[var->value]));
            else fprintf(f, "_lvars.pptr.%s:\n. %ld\n", var->name, var->value);
            break;

        default:
            fprintf(stderr, "var_asm_len_l: unsupported variable type: %d\n", var->type);
            return 1;
    }
    return 0;
}

void lvars_set_func_name(char *name) {
    curr_fname = name;
}

void clear_local_vars() {
    n_local_vars = 0;
}

int vars_asm_gen_l() {
    int i = 0;
    for(; i < n_local_vars; i++) {
        if(local_vars[i].is_var) {
            if(local_vars[i].value < 0) {
                if(fprintf(out, "%s:\n. %s\n", local_vars[i].fulln, current_func->arg_fulln[-1 - local_vars[i].value])) return 1;
            } else {
                if(fprintf(out, "%s:\n. %s\n", local_vars[i].fulln, local_vars[local_vars[i].value].fulln)) return 1;
            }
        }
        else {
            if(fprintf(out, "%s:\n. %ld\n", local_vars[i].fulln, local_vars[i].value)) return 1;
        }
    }
    return 0;
}
