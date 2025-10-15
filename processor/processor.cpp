#include "processor.h"

#define ERROR_SOURCE_ processor
#define ERROR_SOURCE_TYPE_ processor_t*
#define ERROR_TYPE_ processor_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION
HANDLE_ERROR(PROCESSOR_READING_ERROR)
HANDLE_ERROR(PROCESSOR_UNKNOWN_INSTRUCTION_ERROR)
HANDLE_ERROR(PROCESSOR_STACK_ERROR)
HANDLE_ERROR(PROCESSOR_DIVISION_BY_ZERO_ERROR)
HANDLE_ERROR(PROCESSOR_SQRT_OF_NEGATIVE_ERROR)
END_PRINT_ERROR_FUNCTION

#include "../instruction.h"

#include "stack.h"

#include <math.h>
#include <stdint.h>
#include <assert.h>

bool ProcessorVerify(processor_t *const processor) {
    if (processor == NULL)
        return false;

    if (processor->reader.error != 0)
        SET_ERROR(PROCESSOR_READING_ERROR);

    if (processor->stack.error != 0)
        SET_ERROR(PROCESSOR_STACK_ERROR);

    return processor->error == 0;
}

#ifdef PROCESSOR_VERIFIER
    #define RETURN_IF_ERROR                \
    do {                                   \
        if (!ProcessorVerify(processor)) { \
            LOG_ERROR();                   \
            return;                        \
        }                                  \
    } while (0)
#else
    #define RETURN_IF_ERROR
#endif

void ProcessorInitialize(processor_t *const processor, char const *const filename) {
    ReaderInitialize(&processor->reader, filename);
    RETURN_IF_ERROR;

    StackInitialize(&processor->stack);
    RETURN_IF_ERROR;
}

void ProcessorFinalize(processor_t *const processor) {
    assert(processor);

    ReaderFinalize(&processor->reader);
    StackFinalize(&processor->stack);
}

#define PROCESSOR_FUNCTION_NAME(name) Run ## name
#define PROCESSOR_FUNCTION_TEMPLATE(name) static void name (processor_t *processor)
#define DECLARE_PROCESSOR_FUNCTION(name) PROCESSOR_FUNCTION_TEMPLATE( PROCESSOR_FUNCTION_NAME(name) )

DECLARE_PROCESSOR_FUNCTION(HLT) {
    SetReaderPosition(&processor->reader, processor->reader.size);
}

DECLARE_PROCESSOR_FUNCTION(PUSH) {
    int value = 0;

    ReadElement(&processor->reader, &value, sizeof(value));
    RETURN_IF_ERROR;

    StackPush(&processor->stack, value);
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(OUT) {
    int value = 0;

    StackPop(&processor->stack, &value);
    RETURN_IF_ERROR;

    printf("%d\n", value);
}

#define OPERATOR(name, symbol, ...)          \
DECLARE_PROCESSOR_FUNCTION(name) {           \
    int a = 0, b = 0;                        \
    StackPop(&processor->stack, &b);          \
    RETURN_IF_ERROR;                         \
    StackPop(&processor->stack, &a);          \
    RETURN_IF_ERROR;                         \
    __VA_ARGS__                              \
    StackPush(&processor->stack, a symbol b); \
    RETURN_IF_ERROR;                         \
}

OPERATOR(ADD, +)
OPERATOR(SUB, -)
OPERATOR(MUL, *)
OPERATOR(DIV, /,
    if (b == 0)
        RAISE_ERROR(PROCESSOR_DIVISION_BY_ZERO_ERROR);
)

#undef OPERATOR

DECLARE_PROCESSOR_FUNCTION(SQRT) {
    int x = 0;
    StackPop(&processor->stack, &x);
    RETURN_IF_ERROR;

    ERROR_ASSERT(x >= 0, PROCESSOR_SQRT_OF_NEGATIVE_ERROR);

    StackPush(&processor->stack, (int)sqrt(x));
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(PUSHR) {
    int register_index = 0;

    ReadElement(&processor->reader, &register_index, sizeof(register_index));
    RETURN_IF_ERROR;

    StackPush(&processor->stack, processor->registers[register_index]);
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(POPR) {
    int register_index = 0;

    ReadElement(&processor->reader, &register_index, sizeof(register_index));
    RETURN_IF_ERROR;

    StackPop(&processor->stack, &processor->registers[register_index]);
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(JMP) {
    int jump_destination = 0;

    ReadElement(&processor->reader, &jump_destination, sizeof(jump_destination));
    RETURN_IF_ERROR;

    SetReaderPosition(&processor->reader, (size_t)jump_destination);
}

void ExecuteInstruction(processor_t *const processor) {
    uint8_t instruction = 0;
    
    ReadElement(&processor->reader, &instruction, sizeof(instruction));
    RETURN_IF_ERROR;

    switch (instruction) {

#define INSTRUCTION_(name, argument_count) \
        case name ## _code : \
            PROCESSOR_FUNCTION_NAME(name) (processor); \
            break;

#include "../instruction_list.h"

#undef INSTRUCTION_

        default:
            RAISE_ERROR(PROCESSOR_UNKNOWN_INSTRUCTION_ERROR);
            break;
    }
}

#undef RETURN_IF_ERROR
