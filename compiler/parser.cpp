#include "parser.h"

#include "instructions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR1_(x) #x
#define STR2_(x) STR1_(x)
#define TOKEN_SPEC "%" STR2_(MAX_TOKEN_LENGTH) "s"

bool CanParseNextToken(char **const line) {
    char buffer[MAX_TOKEN_LENGTH + 1] = "";
    int scanf_result = sscanf(*line, TOKEN_SPEC, buffer);
    return scanf_result == 1;
}

static bool IsTokenSeparator(char c) {
    return c == ' ';
}

parser_error_t ParseToken(char **const line, parser_function_t const func, void *const result) {
    char buffer[MAX_TOKEN_LENGTH + 1] = "";

    int chars_read_count = 0;
    int scanf_result = sscanf(*line, TOKEN_SPEC"%n", buffer, &chars_read_count);

    if (scanf_result != 1)
        return PARSER_EOF_ERROR;

    char next_char = (*line)[chars_read_count];
    if (!IsTokenSeparator(next_char) && next_char != '\0')
        return PARSER_TOO_LONG_TOKEN_ERROR;

    size_t buffer_length = strlen(buffer);
    int token_length = func(buffer, buffer_length, result);
    if (token_length != (int)buffer_length)
        return PARSER_INVALID_TOKEN_ERROR;

    *line += chars_read_count;
    sscanf(*line, " %n ", &chars_read_count);
    *line += chars_read_count;

    return PARSER_NO_ERROR;
}

int IntegerParserFunction(char const str[], size_t str_length, void *const result) {
    (void)str_length;
    int result_length = 0;
    int scanf_result = sscanf(str, "%d%n", (int *)result, &result_length);

    return scanf_result == 1 ? result_length : -1;
}

static instruction_t const *GetInstruction(char const *const instruction_name) {
    for (size_t i = 0; i < INSTRUCTION_COUNT; i++)
        if (strcmp(INSTRUCTIONS[i].name, instruction_name) == 0)
            return INSTRUCTIONS + i;

    return NULL;
}

int InstructionParserFunction(char const str[], size_t str_length, void *const result) {
    *((instruction_t const **)result) = GetInstruction(str);
    
    if (*(instruction_t const **)result == NULL)
        return -1;
    
    return (int)str_length;
}

#undef STR2_
#undef STR1_
