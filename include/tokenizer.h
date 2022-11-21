/* tokenizer.h */

#pragma once
#ifndef _LLP_TOKENIZER_H_
#define _LLP_TOKENIZER_H_

#include <inttypes.h>
#include <stdbool.h>
#include "ring.h"

struct token {
    enum token_type {
        // Binary operators
        TOK_BIC = 0,
        TOK_IMPL,
        TOK_OR,
        TOK_AND,
        TOK_PLUS,
        TOK_MINUS,
        TOK_MUL,
        TOK_DIV,
        TOK_MOD,

        // Unary operators
        TOK_NEGL,
        TOK_FACT,

        // Brackets
        TOK_OPEN,
        TOK_CLOSE,

        // Non - reachable in a standard way
        TOK_LIT,
        TOK_NEG,

        // Technical tokens
        TOK_END,
        TOK_ERROR
    } type;
    int64_t value;
};

DECLARE_RING(token, struct token)

struct ring_token *tokenize(char *str);
bool is_binop(struct token);
bool is_unop(struct token);

extern const char *TOKENS_STR[];

#endif
