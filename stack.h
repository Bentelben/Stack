#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const size_t INIT_STACK_CAPACITY = 16;
const size_t MIN_STACK_CAPACITY = 8;
const size_t MAX_STACK_CAPACITY = SIZE_MAX - 2;

const int POISON_STACK_VALUE = -1189;
const int KANAREYKA_STACK_VALUE = 0xEDAA; // TODO fix size uint64
// TODO char kanareyka value filling int

enum stack_error_offset_t {
    STACK_IS_NULL_ERROR,
    STACK_DATA_IS_NULL_ERROR,

    STACK_ALLOCATION_ERROR,

    STACK_CAPACITY_LESS_SIZE_ERROR,
    STACK_CAPACITY_LESS_MIN_CAPACITY_ERROR,
    STACK_CAPACITY_BIGGER_MAX_CAPACITY_ERROR,

    STACK_POP_NO_ITEMS_ERROR,
    STACK_PUSH_MAX_CAPACITY_SIZE_ERROR,

    STACK_KANAREYKA_DAMAGED_ERROR,
    STACK_POISON_DAMAGED_ERROR
};

typedef uint32_t stack_error_t;

struct stack_t {
    int *data;
    size_t size;
    size_t capacity;
    stack_error_t error;
};


#define StackDump(logfile, stk) _stackDump(logfile, stk, __FILE__, __LINE__)
void _stackDump(FILE *file, stack_t const *stk, char const *filename, size_t line);

void StackInitialize(stack_t *stk);

void StackPush(stack_t *stk, int elem);

int StackPop(stack_t *stk);

void StackFinalize(stack_t *stk);

#endif
