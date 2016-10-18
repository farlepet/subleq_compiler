#include <stdint.h>
#include <stdio.h>

#include <constants.h>
#include <globals.h>

static int n_constants = 0;
static int64_t constants[MAX_CONSTANTS];

static int const_create(int64_t n) {
    if(n_constants == MAX_CONSTANTS) return 1;
    constants[n_constants++] = n;
    return 0;
}

static int const_get_idx(int64_t n) {
    int i = 0;
    for(; i < n_constants; i++) {
        if(constants[i] == n)
            return i;
    }
    return -1;
}

char *const_get_name(int64_t n) {
    static char ret[512];
    int idx = const_get_idx(n);
    if(idx < 0) {
        if(const_create(n)) {
            fprintf(stderr, "const_get_name: Maximum number of constants reached!\n");
            return NULL;
        }
    }
    sprintf(ret, "_consts.%c%ld", ((n < 0) ? 'n' : 'p'), (n < 0) ? -n : n);
    return ret;
}

int const_write_consts() {
    int i = 0;
    for(; i < n_constants; i++) {
        int64_t n = constants[i];
        if(fprintf(out, "_consts.%c%ld:\n. %ld\n", ((n < 0) ? 'n' : 'p'), (n < 0) ? -n : n, n) < 0) return 1;
    }
    return 0;
}
