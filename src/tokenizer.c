/* tokenizer.c */

#include <ctype.h>
#include <string.h>

#include "../include/ring.h"
#include "../include/tokenizer.h"

DEFINE_RING(token, struct token)

static const char *SEPARATORS = " \t\n";
static const int NUM_TOKENS = TOK_LIT;
static const char *TOKENS[] = {
        [TOK_PLUS]  = "+",
        [TOK_MINUS] = "-",
        [TOK_MUL]   = "*",
        [TOK_DIV]   = "/",
        [TOK_MOD]   = "%",
        [TOK_AND]   = "&&",
        [TOK_OR]    = "||",
        [TOK_NEGL]  = "~",
        [TOK_IMPL]  = "->",
        [TOK_BIC]   = "<->",
        [TOK_FACT]  = "!",

        [TOK_OPEN]  ="(",
        [TOK_CLOSE] = ")",

        [TOK_LIT]   = "",
        [TOK_NEG]   = "-"
};

const char *TOKENS_STR[] = {
        [TOK_PLUS]  = "PLUS",
        [TOK_MINUS] = "MINUS",
        [TOK_MUL]   = "MUL",
        [TOK_DIV]   = "DIV",
        [TOK_MOD]   = "MOD",
        [TOK_AND]   = "AND",
        [TOK_OR]    = "OR",
        [TOK_IMPL]  = "IMPLICATION",
        [TOK_BIC]   = "BICONDITION",

        [TOK_NEGL]  = "NEGL",
        [TOK_FACT]  = "FACT",

        [TOK_OPEN]  = "OPEN",
        [TOK_CLOSE] = "CLOSE",

        [TOK_LIT]   = "LIT",
        [TOK_NEG]   = "NEG",

        [TOK_END]   = "END",
        [TOK_ERROR] = "ERROR"
};

char *skip_separators(char *str) {
    while (*str != '\0' && strchr(SEPARATORS, *str) != NULL)
        str++;
    return str;
}

struct token next_token(char **str) {
    int i;
    char *buf = skip_separators(*str);

    if (*buf == '\0') {
        return (struct token) {TOK_END, 0};
    }

    for (i = 0; i < NUM_TOKENS; i++)
        if (strncmp(buf, TOKENS[i], strlen(TOKENS[i])) == 0) {
            *str = buf + strlen(TOKENS[i]);
            return (struct token) {i, 0};
        }

    if (isdigit(*buf)) {
        char *str_end;
        int64_t tmp = strtoll(buf, &str_end, 10);
        *str = str_end;
        return (struct token) {TOK_LIT, tmp};
    }

    return (struct token) {TOK_ERROR, 0};
}

bool is_binop(struct token token) {
    return token.type == TOK_PLUS || token.type == TOK_MINUS ||
           token.type == TOK_MUL || token.type == TOK_DIV || token.type == TOK_MOD || token.type == TOK_AND || token.type == TOK_OR || token.type == TOK_IMPL || token.type == TOK_BIC;
}

bool is_unop(struct token token) {
    return token.type == TOK_NEG || token.type == TOK_FACT || token.type == TOK_NEGL;
}

struct ring_token *tokenize(char *str) {
    struct token token, prev = {TOK_ERROR, 0};
    struct ring_token *tokens = NULL;
    while ((token = next_token(&str)).type != TOK_END) {
        if (token.type == TOK_ERROR) {
            ring_token_free(&tokens);
            return NULL;
        }
        if (token.type == TOK_MINUS &&
            (tokens == NULL || prev.type == TOK_OPEN || is_binop(prev)))
            token.type = TOK_NEG;
        ring_token_push(&tokens, token);
        prev = token;
    }

    return tokens;
}
