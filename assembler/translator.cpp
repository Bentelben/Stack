#include "translator.h"

#include "../instruction.h"
#include "../byteio/writer.h"

#include "text_utils.h"
#include "parser.h"

#include <string.h>
#include <assert.h>

static void RemoveComment(char *str) {
    char *comment_symbol_ptr = strchr(str, COMMENT_SYMBOL);
    if (comment_symbol_ptr != NULL)
        *comment_symbol_ptr = '\0';
}

translator_error_t TranslateText(writer_t *const writer, line_t *lines, size_t line_count, size_t *text_progress, size_t *line_progress) {
    for (*text_progress = 0; *text_progress < line_count; (*text_progress)++) {
        line_t line = lines[*text_progress];
        line.str[line.length] = '\0';
        
        RemoveComment(line.str);
        translator_error_t error = TranslateLine(writer, &line.str);
        *line_progress = (size_t)(line.str - lines[*text_progress].str);

        if (error != TRANSLATOR_NO_ERROR)
            return error;
    }
    return TRANSLATOR_NO_ERROR;
}

translator_error_t TranslateLine(writer_t *const writer, char **const line) {
    assert(writer);
    assert(line);

    unsigned char instruction = 0; // TODO use error handler
    parser_error_t error = ParseToken(line, InstructionParserFunction, &instruction);
    switch (error) {
        case PARSER_TOO_LONG_TOKEN_ERROR:
            return TRANSLATOR_TOO_LONG_TOKEN_ERROR;
        case PARSER_INVALID_TOKEN_ERROR:
            return TRANSLATOR_INVALID_COMMAND_ERROR;
        case PARSER_EOF_ERROR:
            return TRANSLATOR_NO_ERROR;
        case PARSER_NO_ERROR:
            break;
        default:
            assert(0);
            return TRANSLATOR_INVALID_COMMAND_ERROR;
    }

    WriteElement(writer, &instruction, 1);
    
    for (size_t i = 0; i < INSTRUCTIONS[instruction].argument_count; i++) {
        int value = 0;
        error = ParseToken(line, IntegerParserFunction, &value);
        switch (error) {
            case PARSER_TOO_LONG_TOKEN_ERROR:
                return TRANSLATOR_TOO_LONG_TOKEN_ERROR;
            case PARSER_INVALID_TOKEN_ERROR:
                return TRANSLATOR_INVALID_ARGUMENT_ERROR;
            case PARSER_EOF_ERROR:
                return TRANSLATOR_TOO_FEW_ARGUMENTS_ERROR;
            case PARSER_NO_ERROR:
                break;
            default:
                assert(0);
                return TRANSLATOR_INVALID_ARGUMENT_ERROR;
        }

        WriteElement(writer, &value, sizeof(int));
    }

    if (CanParseNextToken(line))
        return TRANSLATOR_TOO_MANY_ARGUMENTS_ERROR;
    
    return TRANSLATOR_NO_ERROR;
}
