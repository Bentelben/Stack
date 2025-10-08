#define STACK_CPP
#include "stack.h"
#undef STACK_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void SetKanareyka(stack_elem_t *const dest) {
    memset(dest, KANAREYKA_STACK_VALUE, sizeof(stack_elem_t));
 }

static bool IsKanareyka(stack_elem_t const *const dest) {
    unsigned char const *const dest_uc = (unsigned char const *)dest;
    for (size_t i = 0; i < sizeof(stack_elem_t); i++)
        if (dest_uc[i] != KANAREYKA_STACK_VALUE)
            return false;
    return true;
}

#define ERROR_SOURCE_ stack
#define ERROR_SOURCE_TYPE_ stack_t*
#define ERROR_TYPE_ stack_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION()
HANDLE_ERROR(STACK_IS_NULL_ERROR)
HANDLE_ERROR(STACK_DATA_IS_NULL_ERROR)
HANDLE_ERROR(STACK_ALLOCATION_ERROR)
HANDLE_ERROR(STACK_CAPACITY_LESS_SIZE_ERROR)
HANDLE_ERROR(STACK_CAPACITY_LESS_MIN_CAPACITY_ERROR)
HANDLE_ERROR(STACK_POP_NO_ITEMS_ERROR)
HANDLE_ERROR(STACK_PUSH_MAX_CAPACITY_SIZE_ERROR)
HANDLE_ERROR(STACK_KANAREYKA_DAMAGED_ERROR)
END_PRINT_ERROR_FUNCTION()

// TODO enalbe/disable canary MACROS
static bool StackVerify(stack_t *const stack) {
    if (stack == NULL)
        return false;

    RESET_ERROR(STACK_ALLOCATION_ERROR);
    RESET_ERROR(STACK_POP_NO_ITEMS_ERROR);
    RESET_ERROR(STACK_PUSH_MAX_CAPACITY_SIZE_ERROR);

    UPDATE_ERROR_VALUE(STACK_CAPACITY_LESS_SIZE_ERROR, stack->size > stack->capacity);
    UPDATE_ERROR_VALUE(STACK_CAPACITY_LESS_MIN_CAPACITY_ERROR, stack->capacity < MIN_STACK_CAPACITY);
    UPDATE_ERROR_VALUE(STACK_CAPACITY_BIGGER_MAX_CAPACITY_ERROR, stack->capacity > MAX_STACK_CAPACITY);

    if (stack->data == NULL) {
        SET_ERROR(STACK_DATA_IS_NULL_ERROR);
        return false;
    }

    if (
        stack->capacity > MAX_STACK_CAPACITY || // TODO create another error
        !IsKanareyka(stack->data - 1) ||
        !IsKanareyka(stack->data + stack->capacity)
    )
        SET_ERROR(STACK_KANAREYKA_DAMAGED_ERROR);

    for (size_t i = stack->size; i < stack->capacity; i++)
        if (stack->data[i] != POISON_STACK_VALUE) {
            SET_ERROR(STACK_POISON_DAMAGED_ERROR);
            break;
        } // TODO check not poison 0 to size (poison val in ctor)

    return stack->error == 0;
}

#define PRINT_TABBED_(tab_count, format, ...) fprintf(file, "%*s" format, (int)((tab_count)*TAB_SIZE), "", ##__VA_ARGS__)
void StackDump_(FILE *file, stack_t const *const stack, char const *const filename, size_t const line) {
    const size_t TAB_SIZE = 4;

    if (file == NULL) {
        file = stderr;
        PRINT_TABBED_(0, "Dump output file not specified!\n");
    }

    PRINT_TABBED_(0, "StackDump called from from %s:%zu\n", filename, line);
    PRINT_TABBED_(0, "Stack [%p]\n" // TODO add born place
              "{\n", stack);

    if (stack != NULL) {
        PRINT_TABBED_(1, "size     = %zu\n", stack->size);
        PRINT_TABBED_(1, "capacity = %zu (+2)\n", stack->capacity);
        PRINT_TABBED_(1, "error    = ");
        FPrintError(file, stack);
        PRINT_TABBED_(1, "data [%p]\n", stack);
        PRINT_TABBED_(1, "{\n");

        if (stack->data != NULL) {

            PRINT_TABBED_(2, " [-1] = %d (KANAREYKA)\n", stack->data[-1]);

            for (size_t i = 0; i < stack->capacity; i++) {
                PRINT_TABBED_(2, "");
                if (i < stack->size)
                    fprintf(file, "*");
                else
                    fprintf(file, " ");

                fprintf(file, "[%zu] = ", i);
                FPrintStackElement(file, stack->data[i]);

                if (i >= stack->size && stack->data[i] == POISON_STACK_VALUE)
                    fprintf(file, " (POISON)");

                fprintf(file, "\n");
            }

            PRINT_TABBED_(2, " [%zu] = %d (KANAREYKA)\n", stack->capacity, stack->data[stack->capacity]);
        }
        PRINT_TABBED_(1, "}\n");
    }
    PRINT_TABBED_(0, "}\n");
}
#undef PRINT_TABBED_


#ifndef NDEBUG // TODO add release verify macros
    #define CHECK_RETURN              \
        if (!StackVerify(stack)) {    \
            StackDump(stderr, stack); \
            LOG_ERROR()               \
            return;                   \
        }
#else
    #define CHECK_RETURN
#endif

static void ResizeStack(stack_t *const stack, size_t const new_capacity) {
    assert(stack);
    assert(stack->size <= new_capacity);

    if (stack->data != NULL) {
        ERROR_ASSERT(new_capacity <= MAX_STACK_CAPACITY && new_capacity >= MIN_STACK_CAPACITY, STACK_ALLOCATION_ERROR);
        stack->data = stack->data - 1;
    }

    stack_elem_t *new_data = (stack_elem_t *)realloc(stack->data, (1 + new_capacity + 1)*sizeof(*stack->data));
    ERROR_ASSERT(new_data != NULL, STACK_ALLOCATION_ERROR);

    new_data = new_data + 1;

    if (stack->data == NULL)
        SetKanareyka(new_data - 1);
    SetKanareyka(new_data + new_capacity);

#ifndef NDEBUG
    if (new_capacity > stack->capacity)
        for (size_t i = stack->capacity; i < new_capacity; i++)
            new_data[i] = POISON_STACK_VALUE;
#endif

    stack->data = new_data;
    stack->capacity = new_capacity;
}

void StackInitialize(stack_t *const stack) {
    if (stack == NULL) {
        LOG_ERROR()
        return;
    }

    stack->size = 0;
    stack->capacity = 0;
    stack->error = 0;
    stack->data = NULL;
    ResizeStack(stack, INIT_STACK_CAPACITY);

    CHECK_RETURN;
}

void StackPush(stack_t *const stack, stack_elem_t const elem) {
    CHECK_RETURN;

    if (stack->size == stack->capacity) {
        ERROR_ASSERT(stack->capacity != MAX_STACK_CAPACITY, STACK_PUSH_MAX_CAPACITY_SIZE_ERROR);

        if (stack->capacity > MAX_STACK_CAPACITY/2)
            ResizeStack(stack, MAX_STACK_CAPACITY);
        else
            ResizeStack(stack, stack->capacity * 2);

        CHECK_RETURN;
    }

    stack->data[stack->size++] = elem;

    CHECK_RETURN;
}

void StackPop(stack_t *const stack, stack_elem_t *value) {
    CHECK_RETURN;

    ERROR_ASSERT(stack->size != 0, STACK_POP_NO_ITEMS_ERROR)

    if (stack->size <= stack->capacity/4 && stack->capacity != MIN_STACK_CAPACITY) {
        if (stack->capacity / 2 < MIN_STACK_CAPACITY)
            ResizeStack(stack, MIN_STACK_CAPACITY);
        else
            ResizeStack(stack, stack->capacity/2);
    }

    stack->size--;
    *value = stack->data[stack->size];
#ifndef NDEBUG
    stack->data[stack->size] = POISON_STACK_VALUE;
#endif

    CHECK_RETURN;
}

void StackFinalize(stack_t *const stack) {
    ERROR_ASSERT(stack != NULL, STACK_IS_NULL_ERROR);

#ifndef NDEBUG
    stack->data[-1] = POISON_STACK_VALUE;
    stack->data[stack->capacity] = POISON_STACK_VALUE;
    for (size_t i = 0; i < stack->capacity; i++)
        stack->data[i] = POISON_STACK_VALUE;
#endif

    free(stack->data-1);
    stack->data = NULL;
    stack->size = 0;
    stack->capacity = 0; // TODO replace to debug
}

#undef CHECK_RETURN
