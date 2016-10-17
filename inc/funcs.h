#ifndef FUNCS_H
#define FUNCS_H

#include <types.h>
#include <variables.h>

#define MAX_FUNC_ENTRIES 128

typedef struct {
    char name[64];         // Name
    var_type_t ret_type;   // Return-value type
    int n_args;            // Number of arguments
    char **arg_names;      // Argument names
    char **arg_fulln;      // Full argument names (e.g. func_name.arg.arg_name)
    var_type_t *arg_types; // Argument types
} func_ent_t;

//func_ent_t *parse_func(char *func);
func_ent_t *func_create(char *name, var_type_t ret_type, int n_args, var_type_t *arg_types, char **arg_names);
func_ent_t *func_create_from_line(char *line);

var_type_t func_arg_type(func_ent_t *func, char *arg);

#endif
