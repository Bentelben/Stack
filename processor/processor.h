#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "../byteio/reader.h"
#include "../instruction.h"
#include "stack.h"

#include <stdint.h>

enum processor_error_offset_t {
    PROCESSOR_READING_ERROR,
    PROCESSOR_UNKNOWN_INSTRUCTION_ERROR,
    PROCESSOR_STACK_ERROR,
    PROCESSOR_CALL_STACK_ERROR,
    PROCESSOR_DIVISION_BY_ZERO_ERROR,
    PROCESSOR_SQRT_OF_NEGATIVE_ERROR,
    PROCESSOR_WRONG_REGISTER_INDEX_ERROR,
    PROCESSOR_WRONG_USER_INPUT_ERROR
};

typedef uint16_t processor_error_t;

struct processor_t {
    processor_error_t error;
    reader_t reader;
    stack_t stack;
    stack_t call_stack;
    int registers[REGISTER_COUNT];
    int ram[RAM_SIZE]; 
    char vram[VSCREEN_HEIGHT*VSCREEN_WIDTH];
};

void ProcessorInitialize(processor_t *processor, char const *filename);
void ExecuteInstruction(processor_t *processor);
void ProcessorFinalize(processor_t *processor);
bool ProcessorVerify(processor_t *processor);

#endif
