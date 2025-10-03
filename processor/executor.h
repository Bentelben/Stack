#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "../byteio.h"
#include "stack.h"

enum executor_error_t {
    EXECUTOR_NO_ERROR,
    EXECUTOR_READING_ERROR,
    EXECUTOR_UNKNOWN_INSTRUCTION_ERROR,
    EXECUTOR_STACK_ERROR,
    EXECUTOR_DIVISION_BY_ZERO_ERROR,
    EXECUTOR_SQRT_OF_NEGATIVE_ERROR
};

executor_error_t ExecuteInstruction(reader_t *reader, stack_t *stk);

#define DECLARE(function_name) executor_error_t Run ## function_name (reader_t *, stack_t *);

DECLARE(HLT);
DECLARE(PUSH);
DECLARE(OUT);
DECLARE(ADD);
DECLARE(SUB);
DECLARE(MUL);
DECLARE(DIV);

DECLARE(SQRT);

#undef DECLARE

#endif
