#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdint.h>
#include <stdio.h>

#include <types.h>

#define MAX_GLOBAL_VARS 1024
#define MAX_LOCAL_VARS  256

typedef struct {
    char       name[64];  // Variable name
    char       fulln[96]; // Full variable name
    var_type_t type;      // Variable type
    int64_t    value;     // Initial value
    int        is_var;    // Is the `value` an index in the variable array?
} var_ent_t;

int var_create_g(char *, var_type_t, uint64_t, char *);
var_ent_t *var_find_g(char *name);
int var_asm_gen_g(var_ent_t *var);
int vars_asm_gen_g();

int var_create_l(char *, var_type_t, uint64_t, char *);
var_ent_t *var_find_l(char *name);
int var_asm_gen_l(var_ent_t *var);
void lvars_set_func_name(char *name);
void clear_local_vars();
int vars_asm_gen_l();

int var_create_from_line(char *line, int is_global);

#endif
