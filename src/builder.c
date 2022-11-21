#include <stdio.h>

#include "../include/ast.h"
#include "../include/ring.h"
#include "../include/tokenizer.h"

void token_print(struct token token) { printf("%s(%" PRId64 ")", TOKENS_STR[token.type], token.value); }

DECLARE_RING(ast, struct AST)

DEFINE_RING(ast, struct AST)

DEFINE_RING_PRINT(ast, ast_print)

#define RETURN_ERROR(code, msg) return printf(msg), code

DEFINE_RING(token, struct token)

DEFINE_RING_PRINT(token, token_print)


typedef struct AST *(binop_builder)(struct AST *left, struct AST *right);

static binop_builder *binop_builders[] = {
        [TOK_MUL] = mul,
        [TOK_DIV] = divide,
        [TOK_MINUS] = sub,
        [TOK_PLUS] = add,
        [TOK_MOD] = mod,
        [TOK_AND] = and,
        [TOK_OR] = or,
        [TOK_IMPL] = implication,
        [TOK_BIC] = bicondition
};

typedef struct AST *(unop_builder)(struct AST *node);

static unop_builder *unop_builders[] = {
        [TOK_NEG] = neg,
        [TOK_FACT] = fact,
        [TOK_NEGL] = negl
};

static struct AST *build_binop(struct ring_ast **ast_build, struct token operator) {
    struct AST* right = newnode(ring_ast_pop(ast_build));
    struct AST* left = newnode(ring_ast_pop(ast_build));
    return binop_builders[operator.type](left, right);
}

static struct AST *build_unop(struct ring_ast **ast_build, struct token operator) {
    return unop_builders[operator.type]
            (newnode(ring_ast_pop(ast_build)));

}

static struct AST *build_lit(struct ring_ast **ast_stack, struct token operator) {
    return lit(operator.value);
}

typedef struct AST *(builder)(struct ring_ast **ast_stack, struct token operator);

static builder *builders[] = {
        [AST_UNOP] = build_unop,
        [AST_BINOP] = build_binop,
        [AST_LIT] = build_lit,
};

static size_t lit_to_ast_map(struct token tok) {
    if (tok.type == TOK_LIT) return AST_LIT;
    if (is_binop(tok)) return AST_BINOP;
    if (is_unop(tok)) return AST_UNOP;
    return -1;
}

static struct AST *build_node(struct ring_ast **ast_stack, struct token tok) {
    size_t ast_type = lit_to_ast_map(tok);
    if (ast_type == -1) return NULL;
    return builders[ast_type](ast_stack, tok);
}

const short PRECEDENCES[] = {
        [TOK_MOD] = 5,
        [TOK_MUL] = 5,
        [TOK_DIV] = 5,
        [TOK_MINUS] = 4,
        [TOK_PLUS] = 4,
        [TOK_AND] = 3,
        [TOK_OR]  = 2,
        [TOK_IMPL] = 1,
        [TOK_BIC] = 0,
        [TOK_NEGL] = 6,
        [TOK_NEG] = 6,
        [TOK_FACT] = 6
};

struct AST *build_ast(char *str) {
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
            while ((ops_stack != NULL) && (ast_stack != NULL) &&
                   (PRECEDENCES[ring_token_last(ops_stack).type] >= PRECEDENCES[tok.type])) {
                struct token operator = ring_token_pop(&ops_stack);
                if (is_binop(operator) || is_unop(operator)) {
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