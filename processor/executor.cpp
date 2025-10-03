#include "executor.h"
#include "stack.h"

#include <math.h>

#define PROCESSOR
#include "../instructions.h"
#undef PROCESSOR

executor_error_t ExecuteInstruction(reader_t *const reader, stack_t *const stk) {
    unsigned char instruction = 0;

    if (ReadElement(reader, &instruction, sizeof(instruction)) == -1)
        return EXECUTOR_READING_ERROR;

    if ((size_t)instruction >= INSTRUCTION_COUNT)
        return EXECUTOR_UNKNOWN_INSTRUCTION_ERROR;
    
    return INSTRUCTIONS[instruction].func(reader, stk);
}

executor_error_t RunHLT(reader_t *reader, stack_t *stk) {
    (void)stk;
    reader->index = reader->size;
    //printf("Halting\n");
    return EXECUTOR_NO_ERROR;
}

#define RETURN_IF_ERROR() if (stk->error != 0) return EXECUTOR_STACK_ERROR

executor_error_t RunPUSH(reader_t *const reader, stack_t *const stk) {
    int value = 0;
    
    if (ReadElement(reader, &value, sizeof(value)) == -1)
        return EXECUTOR_READING_ERROR;

    StackPush(stk, value);

    RETURN_IF_ERROR();

    return EXECUTOR_NO_ERROR;
}

executor_error_t RunOUT(reader_t *const reader, stack_t *const stk) {
    (void)reader;

    int value = StackPop(stk);
    RETURN_IF_ERROR();
    printf("%d\n", value);
    return EXECUTOR_NO_ERROR;
}

#define OPERATOR(name, symbol, ...)                            \
executor_error_t Run ## name (reader_t *const reader, stack_t *const stk) { \
    (void)reader;                                              \
    int b = StackPop(stk);                                     \
    RETURN_IF_ERROR();                                         \
    int a = StackPop(stk);                                     \
    RETURN_IF_ERROR();                                         \
    __VA_ARGS__                                                \
    StackPush(stk, a symbol b);                                \
    RETURN_IF_ERROR();                                         \
    return EXECUTOR_NO_ERROR;                                  \
}

OPERATOR(ADD, +)
OPERATOR(SUB, -)
OPERATOR(MUL, *)
OPERATOR(DIV, /, if (b == 0) return EXECUTOR_DIVISION_BY_ZERO_ERROR; )

#undef OPERATOR

executor_error_t RunSQRT(reader_t *const reader, stack_t *const stk) {
    (void)reader;
    int x = StackPop(stk);
    RETURN_IF_ERROR();

    if (x < 0)
        return EXECUTOR_SQRT_OF_NEGATIVE_ERROR;

    StackPush(stk, (int)sqrt(x));
    RETURN_IF_ERROR();
    return EXECUTOR_NO_ERROR;
}

#undef RETURN_IF_ERROR
