#include "parser.h"

#define ERROR_SOURCE_ parser
#define ERROR_SOURCE_TYPE_ parser_t*
#define ERROR_TYPE_ parser_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION
HANDLE_ERROR(PARSER_FILE_READING_ERROR)
HANDLE_ERROR(PARSER_INVALID_TOKEN_ERROR)
END_PRINT_ERROR_FUNCTION

#include "../instruction.h"

#include "text_utils.h"
#include "settings.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void ParserInitialize(parser_t *const parser, char const *const filename) {
    assert(parser);
    assert(filename);

    parser->error = 0;
    parser->isEOF = false;
    parser->text = ReadFile(filename);
    parser->cursor = parser->text;
    ERROR_ASSERT(parser->text != NULL, PARSER_FILE_READING_ERROR);
}

static inline bool IsSeparator(char const symbol) {
    return isspace(symbol) || symbol == '\0' || symbol == COMMENT_SYMBOL;
}

static void SkipSpaces(parser_t *const parser) {
    assert(parser);

    for (; *parser->cursor != '\0'; parser->cursor++) {
        char const c = *parser->cursor;
        if (c == ';') {
            char *next_line = strchr(parser->cursor, '\n');
            if (next_line == NULL)
                next_line = strchr(parser->cursor, '\0');
            parser->cursor = next_line - 1;
        } else if (c == '\n') {
        } else if (!IsSeparator(c))
            return;
    }
}

static bool TryParseNumber(char const *const buffer, size_t const buffer_length, token_t *const token) {
    token->type = NUMBER_TOKEN;

    if (buffer_length == 3 && buffer[0] == '\'' && buffer[2] == '\'') {
        token->data.number_data = (double)buffer[1];
        return true;
    }

    int bytes_read = 0;
    if (sscanf(buffer, "%lf%n", &token->data.number_data, &bytes_read) != 1)
        return false;
    return (size_t)bytes_read == buffer_length;
}

static bool TryParseLabel(char const *const buffer, size_t const buffer_length, token_t *const token) {
    if (buffer[0] != LABEL_SYMBOL)
        return false;
    token->type = LABEL_TOKEN;
    token->data.label_data.name = buffer + 1;
    token->data.label_data.length = buffer_length - 1;
    return true;
}

static bool TryParseRegister(char const *const buffer, size_t const buffer_length, token_t *const token) {
    token->type = REGISTER_TOKEN;
    for (register_code_t i = 0; i < REGISTER_COUNT; i++) {
        if (strncmp(REGISTERS[i], buffer, buffer_length) == 0) {
            token->data.register_data = i;
            return true;
        }
    }

    return false;
}

static bool TryParseInstruction(char const *const buffer, size_t const buffer_length, token_t *const token) {
    token->type = INSTRUCTION_TOKEN;
    for (instruction_code_t i = 0; i < INSTRUCTION_COUNT; i++) {
        if (strncmp(INSTRUCTIONS[i].name, buffer, buffer_length) == 0 && INSTRUCTIONS[i].name[buffer_length] == '\0') {
            token->data.instruction_data = i;
            return true;
        }
    }

    return false;
}

void ParseToken(parser_t *const parser, token_t *const token) {
    assert(parser);
    assert(token);

    SkipSpaces(parser);

    char *buffer = parser->cursor;
    size_t buffer_length = 0;
    if (buffer[0] == '\'') {
        buffer_length = 1;
        while (buffer[buffer_length] != '\'') buffer_length++;
        buffer_length += 1;
    } else
        while (!IsSeparator(buffer[buffer_length])) buffer_length++;

    token->type = UNKNOWN_TOKEN;
    token->text = buffer;
    token->text_length = buffer_length;

    if (buffer_length == 0) {
        parser->isEOF = true;
    }
    else if (*parser->cursor == ':') {
        ERROR_ASSERT(TryParseLabel(buffer, buffer_length, token), PARSER_INVALID_TOKEN_ERROR);
    } else if (TryParseNumber(buffer, buffer_length, token)) {
    } else if (TryParseRegister(buffer, buffer_length, token)) {
    } else if (TryParseInstruction(buffer, buffer_length, token)) {
    } else {
        RAISE_ERROR(PARSER_INVALID_TOKEN_ERROR);
    }
    
    parser->cursor = parser->cursor + buffer_length;
}

void ParserFinalize(parser_t *const parser) {
    assert(parser);

    free(parser->text);
}

