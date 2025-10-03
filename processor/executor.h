#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "../byteio.h"
#include "stack.h"

int ExecuteInstruction(reader_t *reader, stack_t *stk);

#define DECLARE(function_name) int Run ## function_name (reader_t *, stack_t *);

DECLARE(HLT);
DECLARE(PUSH);
DECLARE(ADD);
DECLARE(SUB);
DECLARE(MUL);
DECLARE(DIV);
DECLARE(OUT);

#undef DECLARE

#endif
