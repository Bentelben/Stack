#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../instructions.h"
#include "text_utils.h"
#include "parser.h"

enum compiler_error_t {
    COMPILER_NO_ERROR = 0,
    COMPILER_TOO_LONG_TOKEN_ERROR,
    COMPILER_INVALID_COMMAND_ERROR,
    COMPILER_INVALID_ARGUMENT_ERROR,
    COMPILER_TOO_MANY_ARGUMENTS_ERROR,
    COMPILER_TOO_FEW_ARGUMENTS_ERROR
};

static void WriteData(FILE *const file, int const value) {
    fprintf(file, "%d ", value);
}

static compiler_error_t TranslateLine(FILE *const file, char **line) {
    assert(file);
    assert(line);

    size_t instruction = 0;
    parser_error_t error = ParseToken(line, InstructionParserFunction, &instruction);
    switch (error) {
        case PARSER_TOO_LONG_TOKEN_ERROR:
            return COMPILER_TOO_LONG_TOKEN_ERROR;
        case PARSER_INVALID_TOKEN_ERROR:
            return COMPILER_INVALID_COMMAND_ERROR;
        case PARSER_EOF_ERROR:
            return COMPILER_NO_ERROR;
        case PARSER_NO_ERROR:
            break;
        default:
            assert(0);
            return COMPILER_INVALID_COMMAND_ERROR;
    }

    WriteData(file, instruction);
    
    size_t i = 0;
    for (; i < INSTRUCTIONS[instruction].argument_count; i++) {
        int value = 0;
        error = ParseToken(line, IntegerParserFunction, &value);
        switch (error) {
            case PARSER_TOO_LONG_TOKEN_ERROR:
                return COMPILER_TOO_LONG_TOKEN_ERROR;
            case PARSER_INVALID_TOKEN_ERROR:
                return COMPILER_INVALID_ARGUMENT_ERROR;
            case PARSER_EOF_ERROR:
                return COMPILER_TOO_FEW_ARGUMENTS_ERROR;
            case PARSER_NO_ERROR:
                break;
            default:
                assert(0);
                return COMPILER_INVALID_ARGUMENT_ERROR;
        }

        WriteData(file, value);
    }

    if (CanParseNextToken(line))
        return COMPILER_TOO_MANY_ARGUMENTS_ERROR;
    
    return COMPILER_NO_ERROR;
}

static char *CompileLine(FILE *file, line_t line, compiler_error_t *const error) {
    line.str[line.length] = '\0';

    char *comment_symbol_ptr = strchr(line.str, COMMENT_SYMBOL);
    if (comment_symbol_ptr != NULL)
        *comment_symbol_ptr = '\0';

    
    *error = TranslateLine(file, &line.str);

    return line.str;
}

static void HandleCompilerError(compiler_error_t const error, char const *const source_filename, size_t line_index, line_t const line, char const *const compile_progress) {
    printf("Error while compilation occured!\n");
    printf("In source file `%s:%zu`\n\n", source_filename, line_index + 1);
    printf("> %s\n  ", line.str);
    for (size_t i = 0; i < (size_t)(compile_progress - line.str); i++)
        printf(" ");
    printf("^\n");

    switch (error) {
        case COMPILER_TOO_LONG_TOKEN_ERROR:
            printf("Token is too long! Maximum token length is %d\n", MAX_TOKEN_LENGTH);
            break;
        case COMPILER_INVALID_COMMAND_ERROR:
            printf("Invalid command!\n");
            break;
        case COMPILER_INVALID_ARGUMENT_ERROR:
            printf("Invalid argument! Argument must be an integer\n");
            break;
        case COMPILER_TOO_MANY_ARGUMENTS_ERROR:
            printf("Too many arguments for this command!\n");
            break;
        case COMPILER_TOO_FEW_ARGUMENTS_ERROR:
            printf("Too few arguments for this command\n");
            break;

        case COMPILER_NO_ERROR:
        default:
            assert(0);
            break;
    }
}

int main() {
    // TODO command line arguments
    char const *const input_filename = "code.asm";
    char const *const output_filename = "bytecode.txt";

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        printf("Unable open file `%s` for writing\n", output_filename);
        return -1;
    }

    char *const text = ReadFile(input_filename);
    if (text == NULL) {
        printf("Unable to read file `%s`\n", input_filename);
        fclose(output_file);
        return -1;
    }

    size_t line_count = 0;
    line_t *const lines = GetLineArray(text, &line_count);
    if (lines == NULL) {
        printf("Internal compiler error occured while creating line array\n");
        fclose(output_file);
        free(text);
        return -1;
    }

    size_t i = 0;
    char *compile_progress = NULL;
    compiler_error_t error = COMPILER_NO_ERROR;
    for (; i < line_count; i++) {
        compile_progress = CompileLine(output_file, lines[i], &error);
        if (error != COMPILER_NO_ERROR)
            break;
    }

    if (error != COMPILER_NO_ERROR) {
        HandleCompilerError(error, input_filename, i, lines[i], compile_progress);
        fclose(output_file);
        output_file = fopen(output_filename, "w");
    }
    
    fclose(output_file);
    free(text);
    free(lines);

    return 0;
}
