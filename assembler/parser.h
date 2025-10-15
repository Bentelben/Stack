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
    char *line_cursor;
};

enum token_type_t {
    NUMBER_TOKEN,
    INSTRUCTION_TOKEN,
    REGISTER_TOKEN,
    LABEL_TOKEN
};

struct token_t {
    enum token_type_t type;
    union {
        int number_data;
        instruction_code_t instruction_data;
        register_code_t register_data;
        struct {
            size_t label_code; // TODO string
            //char *label_string;
            //size_t label_length;
        } label_data;
    } data;
};

void ParserInitialize(parser_t *parser, char const *filename);
void ParseToken(parser_t *parser, token_t *token);
void ParserFinalize(parser_t *parser);

#endif
