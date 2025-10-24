#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../byteio/writer.h"
#include "../instruction.h"

#include "parser.h"

#include <stdint.h>

enum assembler_error_offset_t {
    ASSEMBLER_PARSER_ERROR,
    ASSEMBLER_WRITER_ERROR,
    ASSEMBLER_ALLOCATION_ERROR,
    ASSEMBLER_TOO_FEW_ARGUMENTS_ERROR,
    ASSEMBLER_EXPECTED_INSTRUCTION_ERROR,
    ASSEMBLER_EXPECTED_LABEL_OR_ADDRESS_ERROR,
    ASSEMBLER_EXPECTED_NUMBER_ERROR,
    ASSEMBLER_EXPECTED_REGISTER_ERROR,
    ASSEMBLER_UNKNOWN_LABEL_ERROR,
    ASSEMBLER_LABEL_REDEFINITION_ERROR
};

typedef uint16_t assembler_error_t;

struct label_t {
    char const *name;
    size_t length;
    instruction_pointer_t jump_destination;
};

enum assembler_stage_t {
    ASSEMBLER_LABEL_LINKING_STAGE,
    ASSEMBLER_MAIN_STAGE
};

struct assembler_t {
    assembler_error_t error;

    char const *inputFileName;
    char const *outputFileName;

    token_t *tokens;
    size_t nTokens;
    size_t tokenCapacity;

    label_t *labels;
    size_t nLabels;
    size_t labelCapacity;

    size_t tokenIndex;
    size_t byteArrayIndex;

    writer_t writer;
    parser_t parser;
};

void AssemblerInitialize(assembler_t *assembler, char const *inputFileName, char const *outputFileName);
void AssemblerRun(assembler_t *assembler);
void AssemblerRunStage(assembler_t *assembler, assembler_stage_t stage);
void AssemblerFinalize(assembler_t *assembler);

#endif
