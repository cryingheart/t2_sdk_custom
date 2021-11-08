#pragma once

enum big_json_token {
    JSON_NONE,                  /* no token */
    JSON_LITERAL,               /* literal string (without quotes) */
    JSON_STRING,                /* quoted string */
    JSON_NUMBER,                /* number */
    JSON_UNKNOWN,               /* unknown token */
    JSON_FALSE,
    JSON_TRUE,
    JSON_NULL,
    JSON_END,                   /* end of file */
    JSON_BEGIN_OBJECT     = '{',
    JSON_END_OBJECT       = '}',
    JSON_BEGIN_ARRAY      = '[',
    JSON_END_ARRAY        = ']',
    JSON_VALUE_SEPARATOR  = ',',
    JSON_MEMBER_SEPARATOR = ':',
};

struct big_json_parser {
    unsigned int (*tokenizer)(struct big_json_parser*, int);
    char *endp;
    unsigned int state;
    int ungetc;
    char token[130];
};

/* BEGIN GENERATED PROTOS */

void big_json_init(struct big_json_parser* parser);

void big_json_finish(struct big_json_parser* parser);

enum big_json_token big_json_tokenizer(struct big_json_parser* parser, int ch);

/* END GENERATED PROTOS */
