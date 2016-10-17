#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <variables.h>
#include <compile.h>
#include <funcs.h>

char *infile = NULL;
char *outfile = NULL;

//static FILE *in  = NULL;
FILE *out = NULL;

void handle_opts(int, char **);
void usage(int);

unsigned line = 0;

int main(int argc, char **argv) {
	handle_opts(argc, argv);

	//if(!infile || !outfile) usage(1);

    if(outfile != NULL) {
        out = fopen(outfile, "w");
        if(!out) {
            printf("Output file %s could not be opened/created!", outfile);
            return 1;
        }
    }
    else out = stdout;

    /*
	if(!in) {
		printf("Input file %s could not be opened!", infile);
		return 1;
	}
	char str[1024];

	while(fgets(str, 1024, in)) {
    }

	fclose(in);
	fclose(out);
    */

    if(infile) {
        compile_file(infile, NULL);
    } else {
        // TESTING:
        /*var_create_g("test_var_1", INT, 32, NULL);
        var_create_g("test_var_2", PTR, 0, "test_var_1");
        var_asm_gen_g(stdout, var_find_g("test_var_1"));
        var_asm_gen_g(stdout, var_find_g("test_var_2"));*/

        var_create_from_line("int test = 2, a, b, c = -3, f;", 1);

        puts("\n");

        func_create_from_line("int test_func(int a, int b, ptr c, int d) {");
        func_create_from_line("int onearg(int a) {");
        func_create_from_line("int noargs() {");

        puts("\n");

        //parse_func("int test_func(int a, int b) { int test = 1; return test; }");

        puts("\n");

        var_asm_gen_g(stdout, var_find_g("test"));
        var_asm_gen_g(stdout, var_find_g("a"));
        var_asm_gen_g(stdout, var_find_g("c"));
        var_asm_gen_g(stdout, var_find_g("f"));
    }

	return 0;
}


void handle_opts(int argc, char **argv) {
    int i = 1;
    while(i < argc) {
        if(argv[i][0] == '-') {
            switch(argv[i][1]) {
                case 'h':
                    usage(0);
                    break;

                case 'i':
                    if(i == argc - 1) {
                        puts("Missing argument for -i!");
                        exit(1);
                    }
                    infile = argv[++i];
                    break;
                
                case 'o':
                    if(i == argc - 1) {
                        puts("Missing argument for -o!");
                        exit(1);
                    }
                    outfile = argv[++i];
                    break;

                default:
                    printf("Invalid option: %s\n", argv[i]);
                    usage(1);
                    break;
            }
        } else {
            printf("Unexpected argument: %s\n", argv[i]);
            usage(1);
        }
        i++;
    }
}

void usage(int retval) {
	puts(
    "USAGE: osic-asm [OPTIONS] -i infile -o outfile\n"
	"  OPTIONS:\n"
	"    -h: Show this help message.\n"
	"    -i: Source file to compile.\n"
	"    -o: Output assembly file.\n");
	exit(retval);
}

