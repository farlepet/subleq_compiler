/*
 * variables_l.c: Local variable handler
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <variables.h>
#include <globals.h>

static int n_local_vars = 0;
static var_ent_t local_vars[MAX_LOCAL_VARS]; // Local variables
static char *curr_fname = ""; // Current function name

int var_create_l(char *name, var_type_t type, int64_t value, char *var_ptr) {
    strcpy(local_vars[n_local_vars].name, name);
    sprintf(local_vars[n_local_vars].fulln, "%s.vars.%s.%s", curr_fname, type_to_str(type), name);
    local_vars[n_local_vars].type = type;
    local_vars[n_local_vars].ptr  = NULL;
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
char *var_get_asm_name_l(var_ent_t *var) {
    return var->fulln;
}

void lvars_set_func_name(char *name) {
    curr_fname = name;
}

void clear_local_vars() {
    if(n_local_vars == 0) return;
    do {
        n_local_vars--;
        if(local_vars[n_local_vars].ptr != NULL) {
            free(local_vars[n_local_vars].ptr);
            local_vars[n_local_vars].ptr = NULL;
        }
    } while(n_local_vars);
}

int vars_asm_gen_l() {
    int i = 0;
    for(; i < n_local_vars; i++) {
        if(local_vars[i].is_var) {
            if(local_vars[i].value < 0) {
                if(fprintf(out, "%s:\n. %s\n", local_vars[i].fulln, current_func->arg_fulln[-1 - local_vars[i].value]) < 0) return 1;
            } else {
                if(fprintf(out, "%s:\n. %s\n", local_vars[i].fulln, local_vars[local_vars[i].value].fulln) < 0) return 1;
            }
        }
        else {
            if(fprintf(out, "%s:\n. %ld\n", local_vars[i].fulln, local_vars[i].value) < 0) return 1;
        }
        if(local_vars[i].ptr != NULL) {
            fprintf(out, "%s:\n. %s\n", local_vars[i].ptr, local_vars[i].fulln);
        }
    }
    return 0;
}
