#pragma once

enum json_token {
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

struct json_parser {
    unsigned int (*tokenizer)(struct json_parser*, int);
    char *endp;
    unsigned int state;
    int ungetc;
    char token[80];
};

/* BEGIN GENERATED PROTOS */

void json_init(struct json_parser* parser);

void json_finish(struct json_parser* parser);

enum json_token json_tokenizer(struct json_parser* parser, int ch);

/* END GENERATED PROTOS */
