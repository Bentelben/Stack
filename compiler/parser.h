#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

#define MAX_TOKEN_LENGTH 255

enum parser_error_t {
    PARSER_NO_ERROR = 0,
    PARSER_TOO_LONG_TOKEN_ERROR,
    PARSER_EOF_ERROR,

    PARSER_INVALID_TOKEN_ERROR
};

typedef int (*parser_function_t)(char const str[], size_t str_length, void *result);

bool CanParseNextToken(char **const line);
parser_error_t ParseToken(char **line, parser_function_t func, void *result);

int IntegerParserFunction(char const str[], size_t str_length, void *result);
int InstructionParserFunction(char const str[], size_t str_length, void *result);


#endif
