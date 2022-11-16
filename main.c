/* main.c */

#include <string.h>

#include "ast.h"
#include "ring.h"
#include "tokenizer.h"

void ast_print(struct AST ast) {
    print_ast(stdout, &ast);
}

void token_print(struct token token) { printf("%s(%" PRId64 ")", TOKENS_STR[token.type], token.value); }

DECLARE_RING(ast, struct AST)

DEFINE_RING(ast, struct AST)

DEFINE_RING_PRINT(ast, ast_print)

DEFINE_RING(token, struct token)

DEFINE_RING_PRINT(token, token_print)

#define RETURN_ERROR(code, msg) return printf(msg), code

const short PRECEDENCES[] = {
        [TOK_MUL] = 2,
        [TOK_DIV] = 2,
        [TOK_MINUS] = 1,
        [TOK_PLUS] = 1,
        [TOK_NEG] = 3
};

typedef struct AST *(binop_builder)(struct AST *left, struct AST *right);

static binop_builder *binop_builders[] = {
        [TOK_MUL] = mul,
        [TOK_DIV] = divide,
        [TOK_MINUS] = sub,
        [TOK_PLUS] = add
};

typedef struct AST *(unop_builder)(struct AST* node);

static unop_builder *unop_builders[] = {
        [TOK_NEG] = neg
};

struct AST* build_binop(struct ring_ast **ast_build, struct ring_token **ops) {
    return binop_builders[ring_token_pop(ops).type]
            (newnode(ring_ast_pop(ast_build)),
             newnode(ring_ast_pop(ast_build)));
}

struct AST* build_unop(struct ring_ast **ast_build, struct ring_token **ops) {
    return unop_builders[ring_token_pop(ops).type]
            (newnode(ring_ast_pop(ast_build)));

}

struct AST *build_op(struct ring_ast **ast_build, struct ring_token **ops, struct token operator) {
    if(is_binop(operator)) {
        return build_binop(ast_build, ops);
    } else if(is_unop(operator)) {
        return build_unop(ast_build, ops);
    } else {
        return NULL;
    }
}

struct AST *build_ast(char *str) {
    struct ring_token *tokens = NULL;
    if ((tokens = tokenize(str)) == NULL)
        RETURN_ERROR(NULL, "Tokenization error.\n");

    struct ring_ast *ast_build = NULL;
    struct ring_token *ops = NULL;
    while (tokens != NULL) {
        struct token tok = ring_token_pop_top(&tokens);
        if (tok.type == TOK_LIT) {
            ring_push_create(ast, ast_build, *lit(tok.value))
        } else if (is_binop(tok) || is_unop(tok)) {
            while ((ops != NULL)) {
                struct token operator = ring_token_last(ops);
                if(PRECEDENCES[operator.type] < PRECEDENCES[tok.type])
                    break;
                if(is_binop(operator) || is_unop(operator)) {
                    ring_push_create(ast, ast_build, *build_op(&ast_build, &ops, operator));
                } else {
                    break;
                }
            }
            ring_push_create(token, ops, tok);
        } else if (tok.type == TOK_OPEN) {
            ring_push_create(token, ops, tok);
        } else if (tok.type == TOK_CLOSE) {
            while ((ops != NULL) && ring_token_last(ops).type != TOK_OPEN) {
                ring_push_create(ast, ast_build, *build_op(&ast_build, &ops, ring_token_last(ops)));
            }
            ring_token_pop(&ops);
        }
    }
    while (ops != NULL) {
        ring_push_create(ast, ast_build, *build_op(&ast_build, &ops, ring_token_last(ops)));
    }

    struct AST *result = newnode(ring_ast_pop(&ast_build));
    ring_token_free(&tokens);


    return result;
}


int main() {
    //char *str = "(1+ -2 )";
    const int MAX_LEN = 1024;
    char str[MAX_LEN];
    if (fgets(str, MAX_LEN, stdin) == NULL)
        RETURN_ERROR(0, "Input is empty.");
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
