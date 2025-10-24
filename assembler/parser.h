#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdint.h>

#include "../instruction.h"

enum parser_error_offset_t {
    PARSER_FILE_READING_ERROR,
    PARSER_INVALID_TOKEN_ERROR
};

typedef uint8_t parser_error_t;

struct parser_t {
    parser_error_t error;
    bool isEOF;
    char *text;
    char *cursor;
};

enum token_type_t {
    UNKNOWN_TOKEN,
    NUMBER_TOKEN,
    INSTRUCTION_TOKEN,
    REGISTER_TOKEN,
    LABEL_TOKEN
};

struct token_t {
    enum token_type_t type;
    union {
        double number_data;
        instruction_code_t instruction_data;
        register_code_t register_data;
        struct {
            char const *name;
            size_t length;
        } label_data;
    } data;
    char *text;
    size_t text_length;
};

void ParserInitialize(parser_t *parser, char const *filename);
void ParseToken(parser_t *parser, token_t *token);
void ParserFinalize(parser_t *parser);

#endif
