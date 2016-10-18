#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include <get_block.h>
#include <constants.h>
#include <get_loc.h>
#include <globals.h>
#include <compile.h>
#include <values.h>
#include <funcs.h>

int compile_function(char *str, func_ent_t *func);
int compile_handle_func_stmt(char *line, func_ent_t *func);

int compile_file(char *fname) {
    FILE *f = fopen(fname, "r");
    if(!f) {
        fprintf(stderr, "compile_file: Could not open file: %s\n", fname);
        return 1;
    }
    fseek(f, 0, SEEK_END); // seek to end of file
    unsigned long fsize = (unsigned long)ftell(f); // get current file pointer
    fseek(f, 0, SEEK_SET); // seek back to beginning of file

    char *text = (char *)malloc(fsize+1);
    fread(text, fsize, 1, f);
    text[fsize] = 0;
    unsigned tp = 0;

    printf("%s\n", text);

    char tmp[513];
    unsigned long len = strlen(text);

    while(tp < len) {
        while(isspace(text[tp])) tp++;
        if(strstr(text + tp, "func ") == (text + tp)) {
            tp += 4;
            fprintf(stderr, "\ncompile_file: found function\n");
            func_ent_t *func = func_create_from_line(text + tp);
            int bs, be;
            get_block_cbraces(text + tp, &bs, &be);
            char *blkstr = (char *)malloc((unsigned long)(be - bs) + 1);
            memcpy(blkstr, text + tp + bs, (unsigned long)(be - bs));
            blkstr[be - bs] = '\0';
            tp += (unsigned)be;
            compile_function(blkstr, func);
        } else if(get_type(text + tp) != INVLD) {
            int nchrs = get_loc(text + tp, tmp, 512);
            var_create_from_line(tmp, 1);
            //printf("[tp: %d]", tp);
            tp += (unsigned)nchrs;
        } else if(text[tp] == '/') {
            if(text[tp+1] == '/') {
                tp += 2;
                while(text[tp] != '\n' && text[tp] != '\0') tp++;
            } else if(text[tp+1] == '*') {
                tp += 2;
                while(text[tp] != '\0' && !(text[tp] == '*' && text[tp] == '/')) tp++;
            }
        } else {
            fprintf(stderr, "compile_file: unrecognized statement: [%.32s]\n", text + tp);
            //return 1;
        }
        
    }

    vars_asm_gen_g();
    const_write_consts();
    fprintf(out, "_cZ:\n. 0\n");

    return 0;
}

int compile_function(char *str, func_ent_t *func) {
    current_func = func;
    lvars_set_func_name(func->name);
    fprintf(out, "%s:\n", func->name);
    unsigned tp = 1; // Skip first curly brace
    unsigned len = (unsigned)strlen(str) - 1; // Skip last curly brace
    char tmp[512];
    while(tp < len) {
        while(isspace(str[tp]))tp++;
        if(tp >= len) break;
        
        if(get_type(str + tp) != INVLD) {
            int nchrs = get_loc(str + tp, tmp, 512);
            var_create_from_line(tmp, 0);
            tp += (unsigned)nchrs;
        } else {
            int nchrs = get_loc(str + tp, tmp, 512);
            if(compile_handle_func_stmt(tmp, func)) {
                fprintf(stderr, "compile_function: unrecognized statement: %.32s\n", str + tp);
                return 1;
            }
            tp += (unsigned)nchrs;
        }
    }
    vars_asm_gen_l();

    int i;
    for(i = 0; i < func->n_args; i++) {
        fprintf(out, "%s.arg.%s:\n. 0\n", func->name, func->arg_names[i]);
    }
    fprintf(out, "%s.ret:\n. 0\n", func->name);
    clear_local_vars();
    return 0;
}

int compile_handle_func_stmt(char *line, func_ent_t *func) {
    (void)func;
    while(isspace(*line)) line++;
    fprintf(stderr, "Curent line: [%s]\n", line);
    fprintf(out, "\n# %s\n", line);
    if(strstr(line, "return") == line) {
        // TODO: handle text after return!
        fprintf(out, "ret\n");
    } else if(strstr(line, "++")) {
        // Increment variable
        char *lhs = strtok(line, "++");
        char *var; int64_t val;
        if(get_value(lhs, &val, &var)) {
            fprintf(stderr, "compile_handle_func_stmt: Left-hand side of `++` operator must be a variable!\n");
            return 1;
        }
        char *neg1 = const_get_name(-1);
        if(neg1 == NULL) {
            fprintf(stderr, "compile_handle_func_stmt: Could not create constant!\n");
            return 1;
        }
        fprintf(out, "%s, %s\n", neg1, lhs);
    } else if(strstr(line, "--")) {
        // Decrement variable
        char *lhs = strtok(line, "--");
        char *var; int64_t val;
        if(get_value(lhs, &val, &var)) {
            fprintf(stderr, "compile_handle_func_stmt: Left-hand side of `--` operator must be a variable!\n");
            return 1;
        }
        char *one = const_get_name(1);
        if(one == NULL) {
            fprintf(stderr, "compile_handle_func_stmt: Could not create constant!\n");
            return 1;
        }
        fprintf(out, "%s, %s\n", one, lhs);

    } else if(strstr(line, "+=")) {
        // Add to variable
        char *lhs = strtok(line, " +="); // Left-hand side
        char *rhs = strtok(NULL, " +="); // Right-hand side
        char *lhs_var = NULL, *rhs_var = NULL; int64_t val;
        if(get_value(lhs, &val, &lhs_var)) return 1;
        if(lhs_var == NULL) {
            fprintf(stderr, "compile_handle_func_stmt: Left-hand side of `+=` operator must be a variable!\n");
            return 1;
        }
        if(get_value(rhs, &val, &rhs_var)) return 1;
        if(rhs_var) {
            fprintf(out, "%s, _cZ\n_cZ, %s\n_cZ, _cZ\n", rhs_var, lhs_var);
        } else {
            char *clbl = const_get_name(-val);
            if(!clbl) {
                fprintf(stderr, "compile_handle_func_stmt: Could not create constant!\n");
                return 1;
            }
            fprintf(out, "%s, %s\n", clbl, lhs_var);
        }
    } else if(strstr(line, "-=")) {
        // Subtract from variable
        char *lhs = strtok(line, " -="); // Left-hand side
        char *rhs = strtok(NULL, " -="); // Right-hand side
        char *lhs_var = NULL, *rhs_var = NULL; int64_t val;
        if(get_value(lhs, &val, &lhs_var)) return 1;
        if(lhs_var == NULL) {
            fprintf(stderr, "compile_handle_func_stmt: Left-hand side of `+=` operator must be a variable!\n");
            return 1;
        }
        if(get_value(rhs, &val, &rhs_var)) return 1;
        if(rhs_var) {
            fprintf(out, "%s, %s\n", rhs_var, lhs_var);
        } else {
            char *clbl = const_get_name(val);
            if(!clbl) {
                fprintf(stderr, "compile_handle_func_stmt: Could not create constant!\n");
                return 1;
            }
            fprintf(out, "%s, %s\n", clbl, lhs_var);
        }
    } else if(strstr(line, "*=")) {
        // Multiply variable by
    } else if(strstr(line, "/=")) {
        // Divide variable by
    } else if(strstr(line, "=")) {
        // Assign value to variable
        char *lhs = strtok(line, " ="); // Left-hand side
        char *rhs = strtok(NULL, " ="); // Right-hand side
        char *lhs_var = NULL, *rhs_var = NULL; int64_t val;
        if(get_value(lhs, &val, &lhs_var)) return 1;
        if(lhs_var == NULL) {
            fprintf(stderr, "compile_handle_func_stmt: Left-hand side of `=` operator must be a variable!\n");
            return 1;
        }
        if(get_value(rhs, &val, &rhs_var)) return 1;
        if(rhs_var) {
            fprintf(out, "%s, %s\n%s, _cZ\n_cZ, %s\n_cZ, _cZ\n", lhs_var, lhs_var, rhs_var, lhs_var);
        } else {
            char *clbl = const_get_name(-val);
            if(!clbl) {
                fprintf(stderr, "compile_handle_func_stmt: Could not create constant!\n");
                return 1;
            }
            fprintf(out, "%s, %s\n%s, %s\n", lhs_var, lhs_var, clbl, lhs_var);
        }
    }
    
    else {
        fprintf(stderr, "compile_handle_func_stmt: unrecognized statement: %s\n", line);
        return 1;
    }
    return 0;
}
