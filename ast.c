/* ast.c */

#include <stdlib.h>

#include "ast.h"

struct AST *newnode(struct AST ast) {
    struct AST *const node = malloc(sizeof(struct AST));
    *node = ast;
    return node;
}

struct AST _lit(int64_t value) {
    return (struct AST) {AST_LIT, .as_literal = {value}};
}

struct AST *lit(int64_t value) {
    return newnode(_lit(value));
}

struct AST _unop(enum unop_type type, struct AST *operand) {
    return (struct AST) {AST_UNOP, .as_unop = {type, operand}};
}

struct AST *unop(enum unop_type type, struct AST *operand) {
    return newnode(_unop(type, operand));
}

struct AST _binop(enum binop_type type, struct AST *left, struct AST *right) {
    return (struct AST) {AST_BINOP, .as_binop = {type, left, right}};
}

struct AST *binop(enum binop_type type, struct AST *left, struct AST *right) {
    return newnode(_binop(type, left, right));
}

static const char *BINOPS[] = {
        [BIN_PLUS] = "+", [BIN_MINUS] = "-", [BIN_MUL] = "*", [BIN_DIV] = "/"};
static const char *UNOPS[] = {[UN_NEG] = "-"};

typedef void(printer)(FILE *, struct AST *);


static void print_binop(FILE *f, struct AST *ast) {
    fprintf(f, "(");
    print_ast(f, ast->as_binop.left);
    fprintf(f, ")");
    fprintf(f, "%s", BINOPS[ast->as_binop.type]);
    fprintf(f, "(");
    print_ast(f, ast->as_binop.right);
    fprintf(f, ")");
}

static void print_unop(FILE *f, struct AST *ast) {
    fprintf(f, "%s(", UNOPS[ast->as_unop.type]);
    print_ast(f, ast->as_unop.operand);
    fprintf(f, ")");
}

static void print_lit(FILE *f, struct AST *ast) {
    fprintf(f, "%" PRId64, ast->as_literal.value);
}

static printer *ast_printers[] = {
        [AST_BINOP] = print_binop, [AST_UNOP] = print_unop, [AST_LIT] = print_lit};

void print_ast(FILE *f, struct AST *ast) {
    if (ast)
        ast_printers[ast->type](f, ast);
    else
        fprintf(f, "<NULL>");
}

typedef int64_t (parser)(struct AST *);

// UNOP TYPE

static int64_t parse_neg(struct AST *ast) {
    return -calc_ast(ast);
}

static parser *unop_parsers[] = {
        [UN_NEG] = parse_neg
};

// BINOP TYPE

typedef int64_t (binop_parser)(struct AST *, struct AST *);

#define DEFINE_SIMPLE_BINOP_PARSER(operation, operat) \
static int64_t parse_binop_##operation(struct AST *left, struct AST *right) { \
    return calc_ast(left) operat calc_ast(right); \
}

DEFINE_SIMPLE_BINOP_PARSER(add, +)

DEFINE_SIMPLE_BINOP_PARSER(sub, -)

DEFINE_SIMPLE_BINOP_PARSER(mul, *)

DEFINE_SIMPLE_BINOP_PARSER(div, /)

static binop_parser *binop_parsers[] = {
        [BIN_PLUS] = parse_binop_add, [BIN_MINUS] = parse_binop_sub, [BIN_DIV] = parse_binop_div, [BIN_MUL] = parse_binop_mul
};

// AST TYPE

static int64_t parse_lit(struct AST *ast) {
    return ast->as_literal.value;
}

static int64_t parse_unop(struct AST *ast) {
    return unop_parsers[ast->as_unop.type](ast->as_unop.operand);
}

static int64_t parse_binop(struct AST *ast) {
    return binop_parsers[ast->as_binop.type](ast->as_binop.left, ast->as_binop.right);
}

static parser *ast_parsers[] = {
        [AST_LIT] = parse_lit, [AST_UNOP] = parse_unop, [AST_BINOP] = parse_binop
};


int64_t calc_ast(struct AST *ast) {
    if (ast)
        return ast_parsers[ast->type](ast);
    else
        return 0;
}


static void p_print_binop(FILE *f, struct AST *ast) {
    p_print_ast(f, ast->as_binop.left);
    p_print_ast(f, ast->as_binop.right);
    fprintf(f, "%s ", BINOPS[ast->as_binop.type]);
}

static void p_print_unop(FILE *f, struct AST *ast) {
    p_print_ast(f, ast->as_unop.operand);
    fprintf(f, "%s ", UNOPS[ast->as_unop.type]);
}

static void p_print_lit(FILE *f, struct AST *ast) {
    fprintf(f, "%"PRId64" ", ast->as_literal.value);
}


static printer *ast_p_printers[] = {
        [AST_BINOP] = p_print_binop, [AST_UNOP] = p_print_unop, [AST_LIT] = p_print_lit};

void p_print_ast(FILE *f, struct AST *ast) {
    if (ast)
        ast_p_printers[ast->type](f, ast);
    else
        fprintf(f, "<NULL>");
}
