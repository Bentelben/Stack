#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "../byteio.h"
#include "instruction_declaration.h"
#include "stack.h"

#include <stdint.h>

enum processor_error_offset_t {
    PROCESSOR_ALLOCATION_ERROR,
    PROCESSOR_READING_ERROR,
    PROCESSOR_UNKNOWN_INSTRUCTION_ERROR,
    PROCESSOR_STACK_ERROR,
    PROCESSOR_DIVISION_BY_ZERO_ERROR,
    PROCESSOR_SQRT_OF_NEGATIVE_ERROR
};

typedef uint16_t processor_error_t;

struct processor_t {
    processor_error_t error;
    reader_t *reader;
    stack_t *stack;
    int registers[8];
};

void ProcessorInitialize(processor_t *processor, char const *filename);
void ExecuteInstruction(processor_t *processor);
void ProcessorFinalize(processor_t *processor);

#define INSTRUCTION_(name, argument_count) DECLARE_PROCESSOR_FUNCTION(name);

#include "../instruction_list.h"

#undef INSTRUCTION_

#endif
