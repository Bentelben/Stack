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
    
    struct {
        char const *function_name;
        char const *file_name;
        size_t line;
    } born_place;
};


void FPrintStackError(FILE *const file, stack_t *stack);
#define StackDump(file, stack) StackDump_(file, stack, __func__, __FILE__, __LINE__)
void StackDump_(FILE *file, stack_t const *stack, char const *function_name, char const *file_name, size_t line);
#define StackInitialize(stack) StackInitialize_(stack, __func__, __FILE__, __LINE__)
void StackInitialize_(stack_t *stack, char const *function_name, char const *file_name, size_t line);
void StackPush(stack_t *stack, stack_elem_t elem);
void StackPop(stack_t *stack, stack_elem_t *value);
bool StackVerify(stack_t *stack);
void StackFinalize(stack_t *stack);

#endif
