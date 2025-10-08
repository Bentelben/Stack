#include "processor.h"

#include "../byteio/reader.h"
#include "stack.h"

#include <math.h>
#include <stdint.h>
#include <assert.h>

#define PROCESSOR
#include "../instruction.h"
#undef PROCESSOR

#define ERROR_SOURCE_ processor
#define ERROR_SOURCE_TYPE_ processor_t*
#define ERROR_TYPE_ processor_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION()
HANDLE_ERROR(PROCESSOR_ALLOCATION_ERROR)
HANDLE_ERROR(PROCESSOR_READING_ERROR)
HANDLE_ERROR(PROCESSOR_UNKNOWN_INSTRUCTION_ERROR)
HANDLE_ERROR(PROCESSOR_STACK_ERROR)
HANDLE_ERROR(PROCESSOR_DIVISION_BY_ZERO_ERROR)
HANDLE_ERROR(PROCESSOR_SQRT_OF_NEGATIVE_ERROR)
END_PRINT_ERROR_FUNCTION()

void ProcessorInitialize(processor_t *processor, char const *filename) {
    processor->reader = (reader_t *)calloc(1, sizeof(reader_t));
    ERROR_ASSERT(processor->reader != NULL, PROCESSOR_ALLOCATION_ERROR)

    processor->stack = (stack_t *)calloc(1, sizeof(stack_t));
    ERROR_ASSERT(processor->stack != NULL, PROCESSOR_ALLOCATION_ERROR)

    // TODO reader error enum
    ReaderInitialize(processor->reader, filename);
    if (processor->reader->error != 0)
        RAISE_ERROR(PROCESSOR_READING_ERROR)

    StackInitialize(processor->stack);
    ERROR_ASSERT(processor->stack->error == 0, PROCESSOR_STACK_ERROR)
}

void ExecuteInstruction(processor_t *processor) {
    uint8_t instruction = 0;
    
    ReadElement(processor->reader, &instruction, sizeof(instruction));
    if (processor->reader->error != 0)
        RAISE_ERROR(PROCESSOR_READING_ERROR)

    if (instruction >= INSTRUCTION_COUNT)
        RAISE_ERROR(PROCESSOR_UNKNOWN_INSTRUCTION_ERROR)

    return INSTRUCTIONS[instruction].func(processor);
}

void ProcessorFinalize(processor_t *processor) {
    assert(processor);

    // TODO set proc values 0
    
    ReaderFinalize(processor->reader);
    StackFinalize(processor->stack);
    free(processor->reader);
    free(processor->stack);
}

// TODO processor verifyier
#define RETURN_IF_ERROR if (processor->stack->error != 0) RAISE_ERROR(PROCESSOR_STACK_ERROR);

DECLARE_PROCESSOR_FUNCTION(HLT) {
    SetReaderPosition(processor->reader, processor->reader->size);
}

DECLARE_PROCESSOR_FUNCTION(PUSH) {
    int value = 0;

    ReadElement(processor->reader, &value, sizeof(value));
    if (processor->reader->error != 0)
        RAISE_ERROR(PROCESSOR_READING_ERROR)

    StackPush(processor->stack, value);

    RETURN_IF_ERROR;
    return;
}

DECLARE_PROCESSOR_FUNCTION(OUT) {
    int value = 0;
    StackPop(processor->stack, &value);
    RETURN_IF_ERROR;
    printf("%d\n", value);
    return;
}

// TODO sasha posmortri

#define OPERATOR(name, symbol, ...)          \
DECLARE_PROCESSOR_FUNCTION(name) {           \
    int a = 0, b = 0;                        \
    StackPop(processor->stack, &b);          \
    RETURN_IF_ERROR;                         \
    StackPop(processor->stack, &a);          \
    RETURN_IF_ERROR;                         \
    __VA_ARGS__                              \
    StackPush(processor->stack, a symbol b); \
    RETURN_IF_ERROR;                         \
    return;                                  \
}

OPERATOR(ADD, +)
OPERATOR(SUB, -)
OPERATOR(MUL, *)
OPERATOR(DIV, /,
    if (b == 0)
        RAISE_ERROR(PROCESSOR_DIVISION_BY_ZERO_ERROR)
)

#undef OPERATOR

DECLARE_PROCESSOR_FUNCTION(SQRT) {
    int x = 0;
    StackPop(processor->stack, &x);

    RETURN_IF_ERROR;

    if (x < 0)
        RAISE_ERROR(PROCESSOR_SQRT_OF_NEGATIVE_ERROR)

    StackPush(processor->stack, (int)sqrt(x));
    RETURN_IF_ERROR;
    return;
}

DECLARE_PROCESSOR_FUNCTION(PUSHR) {
    int register_index = 0;

    ReadElement(processor->reader, &register_index, sizeof(register_index));
    if (processor->reader->error != 0)
        RAISE_ERROR(PROCESSOR_READING_ERROR)

    StackPush(processor->stack, processor->registers[register_index]);

    RETURN_IF_ERROR;
    return;
}

DECLARE_PROCESSOR_FUNCTION(POPR) {
    int register_index = 0;

    ReadElement(processor->reader, &register_index, sizeof(register_index));
    if (processor->reader->error != 0)
        RAISE_ERROR(PROCESSOR_READING_ERROR)

    StackPop(processor->stack, &processor->registers[register_index]);

    RETURN_IF_ERROR;
    return;
}

#undef RETURN_IF_ERROR
