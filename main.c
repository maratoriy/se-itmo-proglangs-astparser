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

typedef struct AST* (binop_builder)(struct AST *left, struct AST *right);

static binop_builder *binop_builders[] = {
        [TOK_MUL] = mul,
        [TOK_DIV] = divide,
        [TOK_MINUS] = sub,
        [TOK_PLUS] = add
};

typedef struct AST* (unop_builder)(struct AST* node);

static unop_builder *unop_builders[] = {
        [TOK_NEG] = neg
};

static struct AST* build_binop(struct ring_ast **ast_build, struct token operator) {
    return binop_builders[operator.type]
            (newnode(ring_ast_pop(ast_build)),
             newnode(ring_ast_pop(ast_build)));
}

static struct AST* build_unop(struct ring_ast **ast_build, struct token operator) {
    return unop_builders[operator.type]
            (newnode(ring_ast_pop(ast_build)));

}

static struct AST* build_lit(struct ring_ast **ast_stack, struct token operator) {
    return lit(operator.value);
}

typedef struct AST* (builder)(struct ring_ast **ast_stack, struct token operator);

static builder *builders[] = {
        [AST_UNOP] = build_unop,
        [AST_BINOP] = build_binop,
        [AST_LIT] = build_lit,
};

static int lit_to_ast_map(struct token tok) {
    if(tok.type == TOK_LIT) return AST_LIT;
    if(is_binop(tok)) return AST_BINOP;
    if(is_unop(tok)) return AST_UNOP;
    return -1;
}

static struct AST* build_node(struct ring_ast **ast_stack, struct token tok) {
    int ast_type = lit_to_ast_map(tok);
    if(ast_type == -1) return NULL;
    return builders[lit_to_ast_map(tok)](ast_stack, tok);
}

const short PRECEDENCES[] = {
        [TOK_MUL] = 2,
        [TOK_DIV] = 2,
        [TOK_MINUS] = 1,
        [TOK_PLUS] = 1,
        [TOK_NEG] = 3
};

struct AST* build_ast(char *str) {
    struct ring_token *tokens = NULL;
    if ((tokens = tokenize(str)) == NULL)
        RETURN_ERROR(NULL, "Tokenization error.\n");

    ring_token_print(tokens);

    struct ring_ast *ast_stack = NULL;
    struct ring_token *ops_stack = NULL;
    while (tokens != NULL) {
        struct token tok = ring_token_pop_top(&tokens);
        if (tok.type == TOK_LIT) {
            ring_ast_push(&ast_stack, *build_node(&ast_stack, tok));
        } else if (is_binop(tok) || is_unop(tok)) {
            while ((ops_stack != NULL) && (PRECEDENCES[ring_token_last(ops_stack).type] >= PRECEDENCES[tok.type])) {
                struct token operator = ring_token_pop(&ops_stack);
                if(is_binop(operator) || is_unop(operator)) {
                    ring_ast_push(&ast_stack, *build_node(&ast_stack, operator));
                } else break;
            }
            ring_token_push(&ops_stack, tok);
        } else if (tok.type == TOK_OPEN) {
            ring_token_push(&ops_stack, tok);
        } else if (tok.type == TOK_CLOSE) {
            while ((ops_stack != NULL) && ring_token_last(ops_stack).type != TOK_OPEN) {
                ring_ast_push(&ast_stack, *build_node(&ast_stack, ring_token_pop(&ops_stack)));
            }
            ring_token_pop(&ops_stack);
        }
    }
    while (ops_stack != NULL) {
        ring_ast_push(&ast_stack, *build_node(&ast_stack, ring_token_pop(&ops_stack)));
    }

    struct AST *result = newnode(ring_ast_pop(&ast_stack));
    ring_token_free(&tokens);
    ring_ast_free(&ast_stack);


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
