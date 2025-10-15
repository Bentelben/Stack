#include <stdio.h>

#include "assembler.h"

#define ERROR_TYPE_ assembler_error_t
#include "../error_handler.h"

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
    Assemble(&assembler);

    if (assembler.error != 0) {
        if (IS_OTHER_ERROR(&assembler, ASSEMBLER_PARSER_ERROR)) {
            printf("Occured error while parsing code\n");
        } else if (IS_OTHER_ERROR(&assembler, ASSEMBLER_WRITER_ERROR)) {
            printf("Occured error while writing bytecode to file\n");
        } else {
            printf("Occured syntax error:\n");
            for (size_t i = 0; assembler.parser.line_cursor[i] != '\n' && assembler.parser.line_cursor[i] != '\0'; i++)
                printf("%c", assembler.parser.line_cursor[i]);
            printf("\n");
            size_t const error_symbol_index = (size_t)(assembler.parser.cursor - assembler.parser.line_cursor);
            for (size_t i = 0; i < error_symbol_index; i++)
                printf(" ");
            printf("^\n");
            printf("\n");
        }
        AssemblerFinalize(&assembler);
        return -1;
    }
    AssemblerFinalize(&assembler);

    assembler_t assembler2;
    AssemblerInitialize(&assembler2, input_filename, output_filename);
    for (size_t i = 0; i < 256; i++) {
        assembler2.labels[i] = assembler.labels[i];
    }
    Assemble(&assembler2);
    AssemblerFinalize(&assembler2);

    return 0;
}
