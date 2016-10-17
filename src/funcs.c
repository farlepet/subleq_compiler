#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <funcs.h>


int n_funcs = 0;
// TODO: Allocate at run-time depending on number of functions in the source
func_ent_t functions[MAX_FUNC_ENTRIES];

func_ent_t *func_create(char *name, var_type_t ret_type, int n_args, var_type_t *arg_types, char **arg_names) {
    if(n_args && ((arg_types == NULL) || (arg_names == NULL))) {
        fprintf(stderr, "func_create: function has arguments, but array(s) are NULL!\n");
        return NULL;
    }
    strcpy(functions[n_funcs].name, name);
    functions[n_funcs].ret_type  = ret_type;
    functions[n_funcs].n_args    = n_args;
    functions[n_funcs].arg_types = arg_types;
    functions[n_funcs].arg_names = arg_names;
    functions[n_funcs].arg_fulln = (char **)malloc(sizeof(char *) * n_args);
    int i = 0;
    size_t namelen = strlen(name);
    for(; i < n_args; i++) {
        functions[n_funcs].arg_fulln[i] = (char *)malloc(namelen + strlen(arg_names[i]) + 6);
        sprintf(functions[n_funcs].arg_fulln[i], "%s.arg.%s", name, arg_names[i]);
    }

    n_funcs++;
    return &functions[n_funcs - 1];
}

func_ent_t *func_create_from_line(char *line) {
    char tmp[1024];
    strcpy(tmp, line);
    char *ln = tmp;
    while(isspace(*ln)) ln++;

    // Get function return value type:
    char *rtype = strtok(ln, "\t ");
    var_type_t r_type = str_to_type(rtype);
    if(r_type == INVLD) return NULL; // Invalid return type

    char *name = strtok(NULL, " ("); // Get function name

    printf("func: %s (%s)\n", name, rtype);

    // Get function arguments:
    char *arg, *args[32];
    int n_args = 0;
    while((arg = strtok(NULL, ",")) != NULL) {
        while(isspace(*arg)) arg++;
        if(strchr(arg, ')')) {
            if(arg[0] == ')') break; // There is nothing else before the parenthese
            arg = strtok(arg, ")");
            if(arg == NULL || arg[0] == 0) break;
            args[n_args] = arg;
            n_args++;
            break;
        }
        args[n_args] = arg;
        n_args++;
    }

    if(n_args) {
        var_type_t *arg_types = (var_type_t *)malloc(sizeof(var_type_t) * n_args);
        char **arg_names      = (char **)malloc(sizeof(char *) * n_args);
    
        // Individually parse each argument
        int i;
        for(i = 0; i < n_args; i++) {
            char *arg_type = strtok(args[i], "\t ");
            char *arg_name = strtok(NULL, "\t ");
            arg_types[i] = str_to_type(arg_type);
            if(arg_type[i] == INVLD) {
                fprintf(stderr, "func_create_from_line: invalid type!\n");
                return NULL;
            }
            arg_names[i] = (char *)malloc(strlen(arg_name) + 1);
            strcpy(arg_names[i], arg_name);
            printf("  arg %s (%s)\n", arg_names[i], arg_type);
        }
        return func_create(name, r_type, n_args, arg_types, arg_names);
    } else {
        return func_create(name, r_type, 0, NULL, NULL);
    }
}

var_type_t func_arg_type(func_ent_t *func, char *arg) {
    if(func == NULL) return INVLD;
    int i;
    for(i = 0; i < func->n_args; i++) {
        if(strcmp(func->arg_names[i], arg)) return func->arg_types[i];
    }
    return INVLD;
}
