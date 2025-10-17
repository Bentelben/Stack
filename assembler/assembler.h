#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../byteio/writer.h"
#include "../instruction.h"

#include "parser.h"

#include <stdint.h>

enum assembler_error_offset_t {
    ASSEMBLER_PARSER_ERROR,
    ASSEMBLER_WRITER_ERROR,
    ASSEMBLER_SYNTAX_ERROR,
    ASSEMBLER_ALLOCATION_ERROR
};

typedef uint8_t assembler_error_t;

struct label_t {
    char const *name;
    size_t length;
    instruction_pointer_t jump_destination;
    label_t *next;
};

struct assembler_t {
    assembler_error_t error;
    parser_t parser;
    writer_t writer;
    label_t *labels;
};

void AssemblerInitialize(assembler_t *assembler, char const *input_filename, char const *output_filename, bool silent = false);
void Assemble(assembler_t *assembler);
void AssemblerFinalize(assembler_t *assembler);

#endif
