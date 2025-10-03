#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../byteio.h"

#include "text_utils.h"
#include "parser.h"
#include "translator.h"


static void HandleTranslatorError(translator_error_t const error, char const *const source_filename, line_t const *lines, size_t const text_progress, size_t const line_progress) {
    printf("Error occured while compilation process!\n");
    printf("In source file `%s:%zu`\n\n", source_filename, text_progress + 1);
    printf("> %s\n  ", lines[text_progress].str);
    for (size_t i = 0; i < line_progress; i++)
        printf(" ");
    printf("^\n");

    switch (error) {
        case TRANSLATOR_TOO_LONG_TOKEN_ERROR:
            printf("Token is too long! Maximum token length is %d\n", MAX_TOKEN_LENGTH);
            break;
        case TRANSLATOR_INVALID_COMMAND_ERROR:
            printf("Invalid command!\n");
            break;
        case TRANSLATOR_INVALID_ARGUMENT_ERROR:
            printf("Invalid argument! Argument must be an integer\n");
            break;
        case TRANSLATOR_TOO_MANY_ARGUMENTS_ERROR:
            printf("Too many arguments for this command!\n");
            break;
        case TRANSLATOR_TOO_FEW_ARGUMENTS_ERROR:
            printf("Too few arguments for this command\n");
            break;

        case TRANSLATOR_NO_ERROR:
        default:
            assert(0);
            break;
    }
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
    
    writer_t writer = {};
    if (InitializeWriter(&writer, output_filename) == -1) {
        printf("Unable open file `%s` for writing\n", output_filename);
        return -1;
    }

    char *const text = ReadFile(input_filename);
    if (text == NULL) {
        printf("Unable to read file `%s`\n", input_filename);
        FinalizeWriter(&writer);
        return -1;
    }

    size_t line_count = 0;
    line_t *const lines = GetLineArray(text, &line_count);
    if (lines == NULL) {
        printf("Internal compiler error occured while creating line array\n");
        FinalizeWriter(&writer);
        free(text);
        return -1;
    }
    
    size_t text_progress = 0;
    size_t line_progress = 0;
    translator_error_t error = TranslateText(&writer, lines, line_count, &text_progress, &line_progress);

    if (error != TRANSLATOR_NO_ERROR) {
        HandleTranslatorError(error, input_filename, lines, text_progress, line_progress);
        FinalizeWriter(&writer);
        free(text);
        free(lines);
        fclose(fopen(output_filename, "w"));
        return -1;
    }
    
    FinalizeWriter(&writer);
    free(text);
    free(lines);

    return 0;
}
