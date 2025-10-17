#include "processor.h"

#define ERROR_SOURCE_ processor
#define ERROR_SOURCE_TYPE_ processor_t*
#define ERROR_TYPE_ processor_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION
HANDLE_ERROR(PROCESSOR_READING_ERROR)
HANDLE_ERROR(PROCESSOR_UNKNOWN_INSTRUCTION_ERROR)
HANDLE_ERROR(PROCESSOR_STACK_ERROR)
HANDLE_ERROR(PROCESSOR_CALL_STACK_ERROR)
HANDLE_ERROR(PROCESSOR_DIVISION_BY_ZERO_ERROR)
HANDLE_ERROR(PROCESSOR_SQRT_OF_NEGATIVE_ERROR)
HANDLE_ERROR(PROCESSOR_WRONG_REGISTER_INDEX_ERROR)
HANDLE_ERROR(PROCESSOR_WRONG_USER_INPUT_ERROR)
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

    if (processor->call_stack.error != 0)
        SET_ERROR(PROCESSOR_CALL_STACK_ERROR);

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

    StackInitialize(&processor->call_stack);
    RETURN_IF_ERROR;
}

void ProcessorFinalize(processor_t *const processor) {
    assert(processor);

    ReaderFinalize(&processor->reader);
    StackFinalize(&processor->stack);
    StackFinalize(&processor->call_stack);
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

DECLARE_PROCESSOR_FUNCTION(IN) {
    int value = 0;

    int code = scanf("%d", &value);
    ERROR_ASSERT(code == 1, PROCESSOR_WRONG_USER_INPUT_ERROR);

    StackPush(&processor->stack, value);
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(OUT) {
    int value = 0;

    StackPop(&processor->stack, &value);
    RETURN_IF_ERROR;

    printf("%d\n", value);
}

#define OPERATOR_(name, symbol, ...)          \
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

OPERATOR_(ADD, +)
OPERATOR_(SUB, -)
OPERATOR_(MUL, *)
OPERATOR_(DIV, /,
    ERROR_ASSERT(b != 0, PROCESSOR_DIVISION_BY_ZERO_ERROR);
)

#undef OPERATOR_

DECLARE_PROCESSOR_FUNCTION(SQRT) {
    int x = 0;
    StackPop(&processor->stack, &x);
    RETURN_IF_ERROR;

    ERROR_ASSERT(x >= 0, PROCESSOR_SQRT_OF_NEGATIVE_ERROR);

    StackPush(&processor->stack, (int)sqrt(x));
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(PUSHR) {
    register_code_t register_index = 0;

    ReadElement(&processor->reader, &register_index, sizeof(register_index));
    RETURN_IF_ERROR;

#ifdef STACK_VERIFIER
    ERROR_ASSERT(register_index < 8, PROCESSOR_WRONG_REGISTER_INDEX_ERROR);
#endif

    StackPush(&processor->stack, processor->registers[register_index]);
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(POPR) {
    register_code_t register_index = 0;

    ReadElement(&processor->reader, &register_index, sizeof(register_index));
    RETURN_IF_ERROR;

#ifdef STACK_VERIFIER
    ERROR_ASSERT(register_index < 8, PROCESSOR_WRONG_REGISTER_INDEX_ERROR);
#endif

    StackPop(&processor->stack, &processor->registers[register_index]);
    RETURN_IF_ERROR;
}

static void DoJump(processor_t *const processor, instruction_pointer_t destination) {
    SetReaderPosition(&processor->reader, (size_t)destination);
}

#define JUMPER_(name, condition) \
DECLARE_PROCESSOR_FUNCTION(name) { \
    instruction_pointer_t jump_destination = 0; \
    ReadElement(&processor->reader, &jump_destination, sizeof(jump_destination)); \
    RETURN_IF_ERROR; \
    int a = 0, b = 0; \
    StackPop(&processor->stack, &b); \
    RETURN_IF_ERROR; \
    StackPop(&processor->stack, &a); \
    RETURN_IF_ERROR; \
    if (condition) \
        DoJump(processor, jump_destination); \
    RETURN_IF_ERROR; \
}

JUMPER_(JMP, true)
JUMPER_(JB,  a <  b)
JUMPER_(JBE, a <= b)
JUMPER_(JA,  a >  b)
JUMPER_(JAE, a >= b)
JUMPER_(JE,  a == b)
JUMPER_(JNE, a != b)


DECLARE_PROCESSOR_FUNCTION(CALL) {
    instruction_pointer_t jump_destination = 0;

    ReadElement(&processor->reader, &jump_destination, sizeof(jump_destination));
    RETURN_IF_ERROR;

    StackPush(&processor->call_stack, (int)processor->reader.index); // TODO stack with another type
    RETURN_IF_ERROR;

    DoJump(processor, jump_destination);
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(RET) {
    int jump_destination = 0;

    StackPop(&processor->call_stack, &jump_destination);
    RETURN_IF_ERROR;
    
    DoJump(processor, (instruction_pointer_t)jump_destination);
    RETURN_IF_ERROR;
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
