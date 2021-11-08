#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <kernel/os.h>
#include <kernel/bitops.h>

#include "json.h"


/* flags returned by tokenizer functions */
#define AGAIN    0
#define CONSUMED BIT(31)

static inline bool isjsonspace(int ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

static inline void addc(struct json_parser *parser, int ch)
{
    if(parser->endp < &parser->token[sizeof(parser->token)-1])
        *parser->endp++ = ch;
}

/* string state flags */
#define STRING_ESCAPE        BIT(0)

static unsigned int
json_token_string(struct json_parser *parser, int ch)
{
    if(ch == EOF) {
        *parser->endp = '\0';
        return JSON_STRING;
    }
    if(parser->state & STRING_ESCAPE) {
        parser->state &= ~STRING_ESCAPE;
        if(ch == '\\') {
            addc(parser, ch);
            return JSON_NONE|CONSUMED;
        }
        if(ch == 'b') {
            addc(parser, '\b');
            return JSON_NONE|CONSUMED;
        }
        if(ch == 'f') {
            addc(parser, '\f');
            return JSON_NONE|CONSUMED;
        }
        if(ch == 'n') {
            addc(parser, '\n');
            return JSON_NONE|CONSUMED;
        }
        if(ch == 'r') {
            addc(parser, '\r');
            return JSON_NONE|CONSUMED;
        }
        if(ch == 't') {
            addc(parser, '\t');
            return JSON_NONE|CONSUMED;
        }
        if(ch == 'u')
            /* preserve unicode quoting as-is */
            addc(parser, '\\');
        /* as-is */
        addc(parser, ch);
        return JSON_NONE|CONSUMED;
    }
    if(ch == '\\') {
        parser->state |= STRING_ESCAPE;
        return JSON_NONE|CONSUMED;
    }
    if(ch == '"') {
        *parser->endp = '\0';
        return JSON_STRING|CONSUMED;
    }

    addc(parser, ch);
    return JSON_NONE|CONSUMED;
}

/* This basically accepts integer constants, instead what it should
 * accept is as follows:
 *
 * RFC7159 specifies the following number format:
 *
 * -?(0|[1-9][0-0]*)(\.[0-9]+)?(e[-+][0-9]+)?
 *
 * It seems more reasonable to accept the following (basically what
 * strtod would accept):
 *
 * decimal floats:
 * [-+]?[0-9]+(\.[0-9]*)?(e[-+]?[0-9]+)?
 * [-+]?\.[0-9]+(e[-+]?[0-9]+)?
 *
 * perhaps also:
 *
 * hex floats:
 * [-+]?0x[0-9a-f]+(\.[0-9a-f]*)?(p[-+]?[0-9]+)?
 * [-+]?0x\.[0-9a-f]+(p[-+]?[0-9]+)?
 *
 * specials:
 * [-+]inf(inity)?
 * [-+]nan(\([0-9a-z_]*\))?
 */
static unsigned int
json_token_number(struct json_parser *parser, int ch)
{
    const int states[] = {
        [0] = 4|2|1,
        [1] = 4|2,
        [2] = 8|4,
        [3] = 4,
        [4] = 16,
    };

    int m = states[parser->state];
    if((m & 1) && ch == '-') {
        parser->state = 1;
    } else if((m & 2) && ch == '0') {
        parser->state = 2;
    } else if((m & 4) && isdigit(ch)) {
        parser->state = 3;
    } else if((m & 8) && tolower(ch) == 'x') {
        parser->state = 4;
    } else if((m & 16) && isxdigit(ch)) {
        parser->state = 4;
    } else {
        *parser->endp = '\0';
        return JSON_NUMBER|AGAIN;
    }

    addc(parser, ch);
    return JSON_NONE|CONSUMED;
}

/* unquoted string */
static unsigned int
json_token_literal(struct json_parser *parser, int ch)
{
    bool good = false;

    good = isalpha(ch);

    if(!good && parser->endp > parser->token)
        good = isdigit(ch) || ch == '_';

    if(good) {
        addc(parser, ch);
        return JSON_NONE|CONSUMED;
    }
    *parser->endp = '\0';

    if(strcmp(parser->token, "false") == 0)
        return JSON_FALSE|AGAIN;

    if(strcmp(parser->token, "true") == 0)
        return JSON_TRUE|AGAIN;

    if(strcmp(parser->token, "null") == 0)
        return JSON_NULL|AGAIN;

    return JSON_LITERAL|AGAIN;
}

static unsigned int
json_token_start(struct json_parser *parser, int ch)
{
    if(ch == EOF)
        return JSON_END|CONSUMED;

    if(isjsonspace(ch))
        return JSON_NONE|CONSUMED;

    parser->endp = parser->token;
    if(strchr("[]{},:", ch) != NULL) {
        addc(parser, ch);
        *parser->endp = '\0';
        return ch|CONSUMED;
    }

    if(ch == '"') {
        parser->tokenizer = json_token_string;
        parser->state = 0;
        return JSON_NONE|CONSUMED;
    }
    if(isdigit(ch) || ch == '-') {
        parser->tokenizer = json_token_number;
        parser->state = 0;
        return JSON_NONE|AGAIN;
    }
    if(isalpha(ch)) {
        parser->tokenizer = json_token_literal;
        parser->state = 0;
        return JSON_NONE|AGAIN;
    }
    addc(parser, ch);
    *parser->endp = '\0';

    return JSON_UNKNOWN|CONSUMED;
}

void
json_init(struct json_parser *parser)
{
    parser->tokenizer = json_token_start;
    parser->endp = parser->token;
    parser->state = 0;
    parser->ungetc = EOF;
}

void
json_finish(struct json_parser *parser)
{
}

/* This accepts one character at a time, and returns a JSON token
 * descriptor, or JSON_NONE for unfinished tokens, the string
 * comprising the token is available in parser->token (this has a
 * restricted size).
 *
 * Strings are returned with no particular coding. Unicode escapes
 * (\uXXXX) are passed through as-is, this causes ambiguity with
 * strings like "\\u1234" vs "\u1234".
 *
 * Numbers are limited to simple integers with optional sign, and
 * hex-prefix. The number itself is not parsed, use strtol or similar.
 *
 * true/false/null are handled properly, other unquoted symbols are
 * returned as JSON_LITERAL.
 *
 * JSON_UNKNOWN is returned for anything not matching a known lexical
 * token.
 *
 * JSON_END is returned at end of file.
 *
 * Since this parses JSON files on a lexical level only, there
 * is no requirement, or validation, that a parsed file is well formed.
 */
enum json_token
json_tokenizer(struct json_parser *parser, int ch)
{
    unsigned int result;

    if(parser->ungetc != EOF)
        swap(parser->ungetc, ch);

    do {
        result = (*parser->tokenizer)(parser, ch);
        bool consumed = result&CONSUMED;
        result &= ~CONSUMED;
        if(result == JSON_NONE) {
            if(consumed) {
                ch = EOF;
                swap(parser->ungetc, ch);
            }
        } else {
            if(!consumed) {
                assert(parser->ungetc == EOF);
                parser->ungetc = ch;
            }
            parser->tokenizer = json_token_start;
            return result;
        }
    } while(ch != EOF);

    return JSON_NONE;
}
