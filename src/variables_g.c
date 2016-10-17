/*
 * variables_g.c: Global variable handler
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <variables.h>
#include <globals.h>

int n_global_vars = 0;
var_ent_t global_vars[MAX_GLOBAL_VARS]; // Global variables

int var_create_g(char *name, var_type_t type, uint64_t value, char *var_ptr) {
    strcpy(global_vars[n_global_vars].name, name);
    sprintf(global_vars[n_global_vars].fulln, "_gvars.%s.%s", type_to_str(type), name);
    global_vars[n_global_vars].type = type;
    if(var_ptr != NULL) { // Variable is set as a pointer to another variable
        int i;
        for(i = 0; i < n_global_vars; ++i) { // Find variable index
            //printf("var_create_g: testing var %i: %s ?= %s\n", i, global_vars[i].name, var_ptr);
            if(!strcmp(global_vars[i].fulln, var_ptr)) {
                global_vars[n_global_vars].value  = i;
                global_vars[n_global_vars].is_var = 1;
                break;
            }
        }
        if(i == n_global_vars) { // Variable could not be found
            fprintf(stderr, "var_create: Could not find variable in order to create pointer: %s\n", var_ptr);
            return 1;
        }
    } else { // Variable is just an integer value
        global_vars[n_global_vars].value  = value;
        global_vars[n_global_vars].is_var = 0;
    }
    n_global_vars++;
    return 0;
}

var_ent_t *var_find_g(char *name) {
    int i;
    for(i = 0; i < n_global_vars; ++i) { // Find variable index
        if(!strcmp(global_vars[i].name, name)) {
            return &global_vars[i];
        }
    }
    return NULL;
}

// NOTE: Return value is valid until the function is called again
char *var_get_asm_name_g(var_ent_t *var) {
    static char ret[128];
    switch(var->type) {
        case INT:
            sprintf(ret, "_gvars.int.%s", var->name);
            break;

        case PTR:
            sprintf(ret, "_gvars.ptr.%s", var->name);
            break;

        case PPTR:
            sprintf(ret, "_gvars.pptr.%s", var->name);
            break;

        default:
            fprintf(stderr, "var_get_asm_name_g: unsupported variable type: %d\n", var->type);
            return NULL;
    }
    return ret;
}

int var_asm_gen_g(var_ent_t *var) {
    switch(var->type) {
        case INT:
            fprintf(out, "_gvars.int.%s:\n. %ld\n", var->name, var->value);
            break;

        case PTR:
            if(var->is_var) fprintf(out, "_gvars.ptr.%s:\n. %s\n", var->name, var_get_asm_name_g(&global_vars[var->value]));
            else fprintf(out, "_gvars.ptr.%s:\n. %ld\n", var->name, var->value);
            break;

        case PPTR:
            if(var->is_var) fprintf(out, "_gvars.pptr.%s:\n. %s\n", var->name, var_get_asm_name_g(&global_vars[var->value]));
            else fprintf(out, "_gvars.pptr.%s:\n. %ld\n", var->name, var->value);
            break;

        default:
            fprintf(stderr, "var_asm_gen_g: unsupported variable type: %d\n", var->type);
            return 1;
    }
    return 0;
}

int vars_asm_gen_g() {
    int i = 0;
    for(; i < n_global_vars; i++) {
        //if(var_asm_gen_g(f, &global_vars[i])) return 1;
        if(fprintf(out, "%s:\n. %ld\n", global_vars[i].fulln, global_vars[i].value)) return 1;
        // TODO: support pointers
    }
    return 0;
}
