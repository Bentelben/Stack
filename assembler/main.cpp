#include "assembler.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

#define ERROR_TYPE_ assembler_error_t
#include "../error_handler.h"

static char *FindLineBeginning(char *start, char *element) {
    assert(start);
    assert(element);
    assert(element >= start);

    char *line_start = (char *)memrchr(start, '\n', (size_t)(element - start));
    if (line_start == NULL)
        line_start = start;

    return line_start + 1;
}

static char *FindLineEnding(char *element) {
    assert(element);

    char *line_end = strchr(element, '\n');
    if (line_end == NULL)
        line_end = strchr(element, '\0');

    return line_end;
}

static size_t GetLineIndex(char *text, char *const lineBeginning) {
    assert(text);
    assert(lineBeginning);

    size_t i = 0;
    for (; text < lineBeginning; i++) {
        text = strchr(text, '\n');
        assert(text);
        text = text + 1;
    }

    return i;
}

static void HandleError(assembler_t *const assembler, char const *const input_filename) {
    if (IS_OTHER_ERROR(assembler, ASSEMBLER_PARSER_ERROR)) {
        printf("Occured error while parsing code\n");
        assembler->tokenIndex = assembler->nTokens;
    } else if (IS_OTHER_ERROR(assembler, ASSEMBLER_WRITER_ERROR))
        printf("Occured error while writing bytecode to file\n");
    else
        printf("Occured syntax error\n");
    

    token_t *token = assembler->tokens + assembler->tokenIndex;
    char *lineBeginning = FindLineBeginning(assembler->parser.text, token->text);
    size_t lineLength = (size_t)(FindLineEnding(token->text) - lineBeginning);
    size_t lineIndex = GetLineIndex(assembler->parser.text, lineBeginning);
    printf("%s:%zu\n", input_filename, lineIndex+1);

    for (size_t i = 0; i < lineLength; i++)
        printf("%c", token->text[i]);
    printf("\n");

    size_t const error_symbol_index = (size_t)(token->text - lineBeginning);
    for (size_t i = 0; i < error_symbol_index; i++)
        printf(" ");
    printf("^\n");
    printf("\n");
    
}

int main(int argc, char *argv[]) {
    if (argc == 1)
        printf("Error: no input file\n");
    else if (argc == 2)
        printf("Error: no output file\n");
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return -1;
    }

    char const *const input_filename = argv[1];
    char const *const output_filename = argv[2];
    
    assembler_t assembler;
    AssemblerInitialize(&assembler, input_filename, output_filename);
    AssemblerRun(&assembler);

    if (assembler.error != 0) {
        HandleError(&assembler, input_filename);
        AssemblerFinalize(&assembler);
        return -1;
    }

    AssemblerFinalize(&assembler);

    return 0;
}
