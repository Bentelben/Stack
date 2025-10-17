#include <stdio.h>

#include "assembler.h"

#define ERROR_TYPE_ assembler_error_t
#include "../error_handler.h"

static void HandleError(assembler_t *const assembler, char const *const input_filename) {
    if (IS_OTHER_ERROR(assembler, ASSEMBLER_PARSER_ERROR))
        printf("Occured error while parsing code\n");
    else if (IS_OTHER_ERROR(assembler, ASSEMBLER_WRITER_ERROR))
        printf("Occured error while writing bytecode to file\n");
    else
        printf("Occured syntax error\n");
    

    printf("%s:%zu\n", input_filename, assembler->parser.line_index);
    for (size_t i = 0; assembler->parser.line_cursor[i] != '\n' && assembler->parser.line_cursor[i] != '\0'; i++)
        printf("%c", assembler->parser.line_cursor[i]);
    printf("\n");
    size_t const error_symbol_index = (size_t)(assembler->parser.last_token_cursor - assembler->parser.line_cursor);
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
    AssemblerInitialize(&assembler, input_filename, output_filename, true);
    Assemble(&assembler);

    if (assembler.error != 0) {
        HandleError(&assembler, input_filename);
        AssemblerFinalize(&assembler);
        return -1;
    }

    assembler_t assembler2;
    AssemblerInitialize(&assembler2, input_filename, output_filename);
    assembler2.labels = assembler.labels;

    Assemble(&assembler2);

    if (assembler2.error != 0)
        HandleError(&assembler2, input_filename);
    

    assembler2.labels = NULL;
    AssemblerFinalize(&assembler2);
    AssemblerFinalize(&assembler);

    return 0;
}
