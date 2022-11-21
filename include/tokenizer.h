/* tokenizer.h */

#pragma once
#ifndef _LLP_TOKENIZER_H_
#define _LLP_TOKENIZER_H_

#include <inttypes.h>
#include <stdbool.h>
#include "ring.h"

struct token {
    enum token_type {
        TOK_BIC = 0, TOK_IMPL, TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_MOD, TOK_AND, TOK_OR, TOK_NEGL,
        TOK_FACT,
        TOK_OPEN, TOK_CLOSE,
        TOK_LIT, TOK_NEG,
        TOK_END, TOK_ERROR
    } type;
    int64_t value;
};

DECLARE_RING(token, struct token)

struct ring_token *tokenize(char *str);
bool is_binop(struct token);
bool is_unop(struct token);

extern const char *TOKENS_STR[];

#endif
