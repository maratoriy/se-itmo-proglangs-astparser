/* main.c */

#include <string.h>

#include "../include/ast.h"
#include "../include/ring.h"
#include "../include/tokenizer.h"
#include "../include/builder.h"



int main() {
    //char *str = "(1+ -2 )";
    const int MAX_LEN = 1024;
    char str[MAX_LEN];
    if (fgets(str, MAX_LEN, stdin) == NULL) {
        printf("Input is empty!");
        return 0;
    }
    if (str[strlen(str) - 1] == '\n')
        str[strlen(str) - 1] = '\0';

    struct AST *ast = build_ast(str);

    if (ast == NULL)
        printf("AST build error.\n");
    else {
        printf("AST: \n");
        ast_print(*ast);
        printf("\nInfix notation: \n%s = %" PRId64 "\n", str, calc_ast(ast));
        printf("Reverse polish notation: \n");
        p_print_ast(stdout, ast);
        printf(" = %" PRId64 "\n", calc_ast(ast));
    }

    return 0;
}
