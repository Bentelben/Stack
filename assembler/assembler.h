#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>
#include "parser.h"
#include "../byteio/writer.h"

enum assembler_error_offset_t {
    ASSEMBLER_PARSER_ERROR,
    ASSEMBLER_WRITER_ERROR,
    ASSEMBLER_SYNTAX_ERROR
};

typedef uint8_t assembler_error_t;

struct assembler_t {
    assembler_error_t error;
    parser_t parser;
    writer_t writer;
    int labels[256]; // TODO realloc
};

void AssemblerInitialize(assembler_t *assembler, char const *input_filename, char const *output_filename);
void Assemble(assembler_t *assembler);
void AssemblerFinalize(assembler_t *assembler);

#endif
