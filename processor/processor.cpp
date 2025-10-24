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
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(DRAW) {
    stack_elem_t height_ = 0, width_ = 0;
    StackPop(&processor->stack, &width_);
    RETURN_IF_ERROR;
    StackPop(&processor->stack, &height_);
    RETURN_IF_ERROR;

    size_t height = (size_t)height_, width = (size_t)width_;

    assert(height <= VSCREEN_HEIGHT); // FIXME remove govnokod yobanij
    assert(width <= VSCREEN_WIDTH);

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            printf("%c", processor->vram[y * width + x]);
        }
        printf("\n");
    }
}

DECLARE_PROCESSOR_FUNCTION(PUSH) {
    stack_elem_t value = 0;

    ReadElement(&processor->reader, &value, sizeof(value));
    RETURN_IF_ERROR;

    StackPush(&processor->stack, value);
    RETURN_IF_ERROR;
}

#ifdef STACK_VERIFIER
    #define CMD_REG_SUBSCRIPT(command_name, task) \
        DECLARE_PROCESSOR_FUNCTION(command_name) { \
            register_code_t register_index = 0; \
            ReadElement(&processor->reader, &register_index, sizeof(register_index)); \
            RETURN_IF_ERROR; \
            ERROR_ASSERT(register_index < REGISTER_COUNT, PROCESSOR_WRONG_REGISTER_INDEX_ERROR); \
            task \
            RETURN_IF_ERROR; \
        }
#else
    #define CMD_REG_SUBSCRIPT(command_name, task) \
        DECLARE_PROCESSOR_FUNCTION(command_name) { \
            register_code_t register_index = 0; \
            ReadElement(&processor->reader, &register_index, sizeof(register_index)); \
            RETURN_IF_ERROR; \
            task \
            RETURN_IF_ERROR; \
        }
#endif

CMD_REG_SUBSCRIPT(PUSHR,
    StackPush(&processor->stack, processor->registers[register_index]);
)

CMD_REG_SUBSCRIPT(PUSHM, 
    size_t index = (size_t)processor->registers[register_index];
    StackPush(&processor->stack, processor->ram[index]);
)

CMD_REG_SUBSCRIPT(PUSHV, 
    size_t index = (size_t)processor->registers[register_index];
    StackPush(&processor->stack, (stack_elem_t)processor->vram[index]);
)

CMD_REG_SUBSCRIPT(POPR, 
    StackPop(&processor->stack, processor->registers + register_index);
)

CMD_REG_SUBSCRIPT(POPM, 
    size_t index = (size_t)processor->registers[register_index];
    StackPop(&processor->stack, processor->ram + index);
)

CMD_REG_SUBSCRIPT(POPV, 
    stack_elem_t symbol = 0;
    StackPop(&processor->stack, &symbol);
    RETURN_IF_ERROR;
    size_t index = (size_t)processor->registers[register_index];
    processor->vram[index] = (char)symbol;
)

#undef CMD_USE_REG

static void DoJump(processor_t *const processor, instruction_pointer_t destination) {
    SetReaderPosition(&processor->reader, (size_t)destination);
}

DECLARE_PROCESSOR_FUNCTION(IN) {
    stack_elem_t value = 0;

    int code = FScanStackElement(stdin, &value);
    ERROR_ASSERT(code == 1, PROCESSOR_WRONG_USER_INPUT_ERROR);

    StackPush(&processor->stack, value);
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(DUB) {
    stack_elem_t value = 0;

    StackPop(&processor->stack, &value);
    RETURN_IF_ERROR;

    StackPush(&processor->stack, value);
    RETURN_IF_ERROR;

    StackPush(&processor->stack, value);
    RETURN_IF_ERROR;
}


DECLARE_PROCESSOR_FUNCTION(OUT) {
    stack_elem_t value = 0;

    StackPop(&processor->stack, &value);
    RETURN_IF_ERROR;

    FPrintStackElement(stdout, value);
    printf("\n");
}

#define OPERATOR_(name, symbol, ...) \
DECLARE_PROCESSOR_FUNCTION(name) { \
    stack_elem_t a = 0, b = 0; \
    StackPop(&processor->stack, &b); \
    RETURN_IF_ERROR; \
    StackPop(&processor->stack, &a); \
    RETURN_IF_ERROR; \
    __VA_ARGS__ \
    StackPush(&processor->stack, a symbol b); \
    RETURN_IF_ERROR; \
}

OPERATOR_(ADD, +)
OPERATOR_(SUB, -)
OPERATOR_(MUL, *)
OPERATOR_(DIV, /,
    ERROR_ASSERT(fabs(b) >= 1e-14, PROCESSOR_DIVISION_BY_ZERO_ERROR);
)

#undef OPERATOR_

DECLARE_PROCESSOR_FUNCTION(SQRT) {
    stack_elem_t x = 0;
    StackPop(&processor->stack, &x);
    RETURN_IF_ERROR;

    ERROR_ASSERT(x >= 0, PROCESSOR_SQRT_OF_NEGATIVE_ERROR);

    StackPush(&processor->stack, sqrt(x));
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(JMP) {
    instruction_pointer_t jumpDestination = 0;
    ReadElement(&processor->reader, &jumpDestination, sizeof(jumpDestination));
    RETURN_IF_ERROR;
    DoJump(processor, jumpDestination);
    RETURN_IF_ERROR;
}

#define JUMPER_(name, condition) \
DECLARE_PROCESSOR_FUNCTION(name) { \
    instruction_pointer_t jump_destination = 0; \
    ReadElement(&processor->reader, &jump_destination, sizeof(jump_destination)); \
    RETURN_IF_ERROR; \
    stack_elem_t a = 0, b = 0; \
    StackPop(&processor->stack, &b); \
    RETURN_IF_ERROR; \
    StackPop(&processor->stack, &a); \
    RETURN_IF_ERROR; \
    if (condition) \
        DoJump(processor, jump_destination); \
    RETURN_IF_ERROR; \
}

JUMPER_(JB,  a <  b)
JUMPER_(JBE, a <= b)
JUMPER_(JA,  a >  b)
JUMPER_(JAE, a >= b)
JUMPER_(JE,  fabs(a - b) < 1e-14)
JUMPER_(JNE, fabs(a - b) >= 1e-14)


DECLARE_PROCESSOR_FUNCTION(CALL) {
    instruction_pointer_t jump_destination = 0;

    ReadElement(&processor->reader, &jump_destination, sizeof(jump_destination));
    RETURN_IF_ERROR;

    StackPush(&processor->call_stack, (stack_elem_t)processor->reader.index);
    RETURN_IF_ERROR;

    DoJump(processor, jump_destination);
    RETURN_IF_ERROR;
}

DECLARE_PROCESSOR_FUNCTION(RET) {
    stack_elem_t jump_destination = 0;

    StackPop(&processor->call_stack, &jump_destination);
    RETURN_IF_ERROR;
    
    DoJump(processor, (instruction_pointer_t)jump_destination);
    RETURN_IF_ERROR;
}


void ExecuteInstruction(processor_t *const processor) {
    uint8_t instruction = 0;
    
    ReadElement(&processor->reader, &instruction, sizeof(instruction));
    RETURN_IF_ERROR;

    //printf("processing %d\n", instruction);

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
