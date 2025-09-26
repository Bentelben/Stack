#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>

const int DEFAULT_STACK_VALUE = 42;

enum stack_error_t {
    STACK_NO_ERROR = 0,
    STACK_ALLOC_ERROR,
    STACK_NULL_ERROR,
    STACK_DATA_NULL_ERROR,
    STACK_POP_NO_ITEMS_ERROR,
    STACK_SIZE_OVERFLOW_CAPACITY
};

struct stack_t {
    int *data;
    size_t size;
    size_t capacity;
    stack_error_t error;
};


#define StackDump(logfile, stk) _StackDump(logfile, stk, __FILE__, __LINE__)
void _StackDump(FILE *file, stack_t const *stk, char const *filename, size_t line);

void StackInitialize(stack_t *stk);

void StackPush(stack_t *stk, int elem);

int StackPop(stack_t *stk);

void StackFinalize(stack_t *stk);

#endif
