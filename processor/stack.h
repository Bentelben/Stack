#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdint.h>

// Element settings
typedef int stack_elem_t;
static const stack_elem_t POISON_STACK_VALUE = -1189;

// Capacity settings
static const size_t INIT_STACK_CAPACITY = 16;
static const size_t MAX_STACK_CAPACITY = SIZE_MAX - 2;
static const size_t MIN_STACK_CAPACITY = 8;

// Kanareyka settings
static const unsigned char KANAREYKA_STACK_VALUE = 0xED;

#ifdef STACK_CPP
void FPrintStackElement(FILE *file, stack_elem_t value);

void FPrintStackElement(FILE *const file, stack_elem_t const value) {
    fprintf(file, "%d", value);
}
#endif


typedef uint16_t stack_error_t;

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

struct stack_t {
    stack_elem_t *data;
    size_t size;
    size_t capacity;
    stack_error_t error;
};


void FPrintStackError(FILE *const file, stack_error_t error);

#define StackDump(logfile, stk) StackDump_(logfile, stk, __FILE__, __LINE__)
void StackDump_(FILE *file, stack_t const *stk, char const *filename, size_t line);

#define StackInitialize(stk) StackInitialize_(stk, __FILE__, __LINE__)
void StackInitialize_(stack_t *stk, char const *filename, size_t line);

#define StackPush(stk, elem) StackPush_(stk, elem, __FILE__, __LINE__)
void StackPush_(stack_t *stk, stack_elem_t elem, char const *filename, size_t line);

#define StackPop(stk) StackPop_(stk, __FILE__, __LINE__)
stack_elem_t StackPop_(stack_t *stk, char const *filename, size_t line);

#define StackFinalize(stk) StackFinalize_(stk, __FILE__, __LINE__)
void StackFinalize_(stack_t *stk, char const *filename, size_t line);

#endif
