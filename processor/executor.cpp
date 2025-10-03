#include "executor.h"
#include "stack.h"

#include <math.h>

#define PROCESSOR
#include "../instructions.h"
#undef PROCESSOR

int ExecuteInstruction(reader_t *reader, stack_t *stk) {
    unsigned char instruction = 0;

    if (ReadElement(reader, &instruction, sizeof(instruction)) == -1)
        return -1;

    if ((size_t)instruction >= INSTRUCTION_COUNT)
        return -1;
    
    return INSTRUCTIONS[instruction].func(reader, stk);
}

int RunHLT(reader_t *reader, stack_t *stk) {
    (void)stk;
    reader->index = reader->size;
    //printf("Halting\n");
    return 0;
}

#define RETURN_IF_ERROR() if (stk->error != 0) return -1

int RunPUSH(reader_t *reader, stack_t *stk) {
    int value = 0;
    
    if (ReadElement(reader, &value, sizeof(value)) == -1)
        return -1;

    StackPush(stk, value);

    RETURN_IF_ERROR();

    return 0;
}

int RunOUT(reader_t *reader, stack_t *stk) {
    (void)reader;

    int value = StackPop(stk);
    RETURN_IF_ERROR();
    printf("%d\n", value);
    return 0;
}

#define OPERATOR(name, symbol, ...)                \
int Run ## name (reader_t *reader, stack_t *stk) { \
    (void)reader;                                  \
    int b = StackPop(stk);                         \
    RETURN_IF_ERROR();                             \
    int a = StackPop(stk);                         \
    RETURN_IF_ERROR();                             \
    __VA_ARGS__                                    \
    StackPush(stk, a symbol b);                    \
    RETURN_IF_ERROR();                             \
    return 0;                                      \
}

OPERATOR(ADD, +)
OPERATOR(SUB, -)
OPERATOR(MUL, *)
OPERATOR(DIV, /) // TODO zero division check

#undef OPERATOR

int RunSQRT(reader_t *reader, stack_t *stk) {
    (void)reader;
    int x = StackPop(stk);
    RETURN_IF_ERROR();
    // TODO check x less zero
    StackPush(stk, (int)sqrt(x));
    RETURN_IF_ERROR();
    return 0;
}

#undef RETURN_IF_ERROR

