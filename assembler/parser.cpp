#include "parser.h"

#define ERROR_SOURCE_ parser
#define ERROR_SOURCE_TYPE_ parser_t*
#define ERROR_TYPE_ parser_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION()
HANDLE_ERROR(PARSER_FILE_READING_ERROR)
HANDLE_ERROR(PARSER_INVALID_TOKEN_ERROR)
END_PRINT_ERROR_FUNCTION()

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "text_utils.h"
#include "settings.h"

#include "../instruction.h"

void ParserInitialize(parser_t *parser, char const *filename) {
    assert(parser);

    parser->error = 0;
    parser->isEOF = false;
    parser->text = ReadFile(filename);
    parser->cursor = parser->text;
    parser->line_cursor = 0;
    ERROR_ASSERT(parser->text != NULL, PARSER_FILE_READING_ERROR);
}

static inline bool IsSeparator(char const symbol) {
    return isspace(symbol) || symbol == '\0' || symbol == COMMENT_SYMBOL;
}

// FIXME ASSERTSSSSSS
static void SkipSpaces(parser_t *parser) {
    for (; *parser->cursor != '\0'; parser->cursor++) {
        char const c = *parser->cursor;
        if (c == ';') {
            char *next_line = strchr(parser->cursor, '\n'); // TODO strpbrk
            if (next_line == NULL)
                next_line = strchr(parser->cursor, '\0');
            parser->cursor = next_line - 1;
        } else if (c == '\n') {
            parser->line_cursor = parser->cursor+1;
        } else if (!IsSeparator(c))
            return;
    }
}

static bool TryParseNumber(char const *buffer, size_t buffer_length, token_t *token) {
    token->type = NUMBER_TOKEN;
    int bytes_read = 0;
    if (sscanf(buffer, "%d%n", &token->data.number_data, &bytes_read) != 1)
        return false;
    return (size_t)bytes_read == buffer_length;
}

static bool TryParseLabel(char const *buffer, size_t buffer_length, token_t *token) {
    bool res = TryParseNumber(buffer + 1, buffer_length-1, token);
    token->type = LABEL_TOKEN;
    return res;
}

static bool TryParseRegister(char const *buffer, size_t buffer_length, token_t *token) {
    token->type = REGISTER_TOKEN;
    for (register_code_t i = 0; i < REGISTER_COUNT; i++) {
        if (strncmp(REGISTERS[i], buffer, buffer_length) == 0) {
            token->data.register_data = i;
            return true;
        }
    }

    return false;
}

static bool TryParseInstruction(char const *buffer, size_t buffer_length, token_t *token) {
    token->type = INSTRUCTION_TOKEN;
    for (instruction_code_t i = 0; i < INSTRUCTION_COUNT; i++) {
        if (strncmp(INSTRUCTIONS[i].name, buffer, buffer_length) == 0) {
            token->data.instruction_data = i;
            return true;
        }
    }

    return false;
}

// TODO strtok
void ParseToken(parser_t *parser, token_t *token) {
    SkipSpaces(parser);

    char const *buffer = parser->cursor;
    size_t buffer_length = 0;
    while (!IsSeparator(buffer[buffer_length])) buffer_length++;

    //for (size_t i = 0; i < buffer_length; i++) printf("%c", buffer[i]);
    //printf("\n");

    if (buffer_length == 0) {
        parser->isEOF = true;
    }
    else if (isdigit(*parser->cursor)) {
        ERROR_ASSERT(TryParseNumber(buffer, buffer_length, token), PARSER_INVALID_TOKEN_ERROR);
    } 
    else if (*parser->cursor == ':') {
        ERROR_ASSERT(TryParseLabel(buffer, buffer_length, token), PARSER_INVALID_TOKEN_ERROR);
    }
    else if (TryParseRegister(buffer, buffer_length, token)) {
    }

    else if (TryParseInstruction(buffer, buffer_length, token)) {
    }
    else {
        RAISE_ERROR(PARSER_INVALID_TOKEN_ERROR);
    }
    
    parser->cursor = parser->cursor + buffer_length;
}

void ParserFinalize(parser_t *parser) {
    assert(parser);

    free(parser->text);
}

