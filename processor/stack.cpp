#define STACK_CPP
#include "stack.h"
#undef STACK_CPP

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static void SetKanareyka(stack_elem_t *const dest) {
    unsigned char *const dest_uc = (unsigned char *)dest;
    for (size_t i = 0; i < sizeof(stack_elem_t); i++)
        dest_uc[i] = KANAREYKA_STACK_VALUE;
}

static bool IsKanareyka(stack_elem_t const *const dest) {
    unsigned char const *const dest_uc = (unsigned char const *)dest;
    for (size_t i = 0; i < sizeof(stack_elem_t); i++)
        if (dest_uc[i] != KANAREYKA_STACK_VALUE)
            return false;
    return true;
}

#define TRUE_BIT ((stack_error_t)1)

#define GET_ERROR_CODE(error_name) (STACK_ ## error_name ## _ERROR)

#define ADD_ERROR(error_name) \
    stk->error |= (TRUE_BIT << GET_ERROR_CODE(error_name));

#define SET_ERROR(error_name, value) \
    if (value) stk->error |= (TRUE_BIT << GET_ERROR_CODE(error_name)); \
    else stk->error = stk->error & (stack_error_t)(~(TRUE_BIT << GET_ERROR_CODE(error_name)));

#define RETURN_ERROR_IF(value, error_name)       \
    if (value) {                                 \
        ADD_ERROR(error_name)                    \
        StackDump_(stderr, stk, filename, line); \
    }                                            \
    if (value) return


#ifndef NDEBUG
    #define CHECK_RETURN if (!StackVerify(stk) && (StackDump(stderr, stk), 1)) return

    static bool StackVerify(stack_t *const stk) {
        if (stk == NULL)
            return false;

        // Reset temporary errors
        SET_ERROR(ALLOCATION, 0);
        SET_ERROR(POP_NO_ITEMS, 0);
        SET_ERROR(PUSH_MAX_CAPACITY_SIZE, 0);

        SET_ERROR(DATA_IS_NULL, stk->data == NULL)

        SET_ERROR(CAPACITY_LESS_SIZE, stk->size > stk->capacity)
        SET_ERROR(CAPACITY_LESS_MIN_CAPACITY, stk->capacity < MIN_STACK_CAPACITY)
        SET_ERROR(CAPACITY_BIGGER_MAX_CAPACITY, stk->capacity > MAX_STACK_CAPACITY)

        if (
            stk->capacity > MAX_STACK_CAPACITY ||
            !IsKanareyka(stk->data - 1) ||
            !IsKanareyka(stk->data + stk->capacity)
        )
            ADD_ERROR(KANAREYKA_DAMAGED)

        for (size_t i = stk->size; i < stk->capacity; i++)
            if (stk->data[i] != POISON_STACK_VALUE) {
                ADD_ERROR(POISON_DAMAGED);
                break;
            }
        
        return stk->error == 0;
    }
#else
    #define CHECK_RETURN
#endif

static void ResizeStack(stack_t *const stk, size_t const new_capacity, char const *const filename, size_t const line) {
    assert(stk);
    assert(stk->size <= new_capacity);

    RETURN_ERROR_IF(new_capacity > MAX_STACK_CAPACITY || new_capacity < MIN_STACK_CAPACITY, ALLOCATION);

    if (stk->data != NULL)
        stk->data = stk->data - 1;

    stack_elem_t *new_data = (stack_elem_t *)realloc(stk->data, (1 + new_capacity + 1)*sizeof(*stk->data));
    RETURN_ERROR_IF(new_data == NULL, ALLOCATION);

    new_data = new_data + 1;

    if (stk->data == NULL)
        SetKanareyka(new_data - 1);
    SetKanareyka(new_data + new_capacity);

#ifndef NDEBUG
    if (new_capacity > stk->capacity)
        for (size_t i = stk->capacity; i < new_capacity; i++)
            new_data[i] = POISON_STACK_VALUE;
#endif

    stk->data = new_data;
    stk->capacity = new_capacity;
}


#define PRINT_ENUM_(error_name)                       \
        case GET_ERROR_CODE(error_name):              \
            fprintf(file, #error_name "_ERROR"); \
            break;

static void FPrintStackError(FILE *const file, stack_error_t error) {
    assert(file);

    if (error == 0) {
        fprintf(file, "NO_ERROR\n");
        return;
    }

    size_t i = sizeof(error)*8 - 1;
    while(error > 0) {
        if (error >= (TRUE_BIT << i)) {

            error = error - (stack_error_t)(TRUE_BIT << i);

            switch (i) {
                PRINT_ENUM_(IS_NULL)
                PRINT_ENUM_(DATA_IS_NULL)

                PRINT_ENUM_(ALLOCATION)

                PRINT_ENUM_(CAPACITY_LESS_SIZE)
                PRINT_ENUM_(CAPACITY_LESS_MIN_CAPACITY)
                PRINT_ENUM_(CAPACITY_BIGGER_MAX_CAPACITY)

                PRINT_ENUM_(POP_NO_ITEMS)
                PRINT_ENUM_(PUSH_MAX_CAPACITY_SIZE)

                PRINT_ENUM_(KANAREYKA_DAMAGED)
                PRINT_ENUM_(POISON_DAMAGED)
                default:
                    fprintf(file, "%zu", i);
                    break;
            }
            if (error != 0)
                fprintf(file, ", ");
        }

        if (i == 0)
            break;
        i--;
    }

    fprintf(file, "\n");
}
#undef PRINT_ENUM_

#define PRINT_TABBED_(tab_count, format, ...) fprintf(file, "%*s" format, (int)((tab_count)*TAB_SIZE), "", ##__VA_ARGS__)

void StackDump_(FILE *file, stack_t const *const stk, char const *const filename, size_t const line) {
    const size_t TAB_SIZE = 4;

    if (file == NULL) {
        file = stderr;
        PRINT_TABBED_(0, "Dump output file not specified!\n");
    }

    PRINT_TABBED_(0, "StackDump called from from %s:%zu\n", filename, line);
    PRINT_TABBED_(0, "Stack [%p]\n"
              "{\n", stk); 

    if (stk != NULL) {
        PRINT_TABBED_(1, "size     = %zu\n", stk->size);
        PRINT_TABBED_(1, "capacity = %zu (+2)\n", stk->capacity);
        PRINT_TABBED_(1, "error    = ");
        FPrintStackError(file, stk->error);
        PRINT_TABBED_(1, "data [%p]\n", stk->data);
        PRINT_TABBED_(1, "{\n");

        if (stk->data != NULL) {
        
            PRINT_TABBED_(2, " [-1] = %d (KANAREYKA)\n", stk->data[-1]);

            for (size_t i = 0; i < stk->capacity; i++) {
                PRINT_TABBED_(2, "");
                if (i < stk->size)
                    fprintf(file, "*");
                else
                    fprintf(file, " ");
        
                fprintf(file, "[%zu] = ", i);
                FPrintStackElement(file, stk->data[i]);
        
                if (i >= stk->size && stk->data[i] == POISON_STACK_VALUE)
                    fprintf(file, " (POISON)");
                
                fprintf(file, "\n");
            }

            PRINT_TABBED_(2, " [%zu] = %d (KANAREYKA)\n", stk->capacity, stk->data[stk->capacity]);
        }
        PRINT_TABBED_(1, "}\n");
    }
    PRINT_TABBED_(0, "}\n");
}
#undef PRINT_TABBED_


void StackInitialize_(stack_t *const stk, char const *const filename, size_t const line) {
    RETURN_ERROR_IF(stk == NULL, IS_NULL);

    stk->size = 0;
    stk->capacity = 0;
    stk->error = 0;
    stk->data = NULL;
    ResizeStack(stk, INIT_STACK_CAPACITY, filename, line);

    CHECK_RETURN;
}

void StackPush_(stack_t *const stk, stack_elem_t const elem, char const *const filename, size_t const line) {
    CHECK_RETURN;

    if (stk->size == stk->capacity) {
        RETURN_ERROR_IF(stk->capacity == MAX_STACK_CAPACITY, PUSH_MAX_CAPACITY_SIZE);

        if (stk->capacity > MAX_STACK_CAPACITY/2)
            ResizeStack(stk, MAX_STACK_CAPACITY, filename, line);
        else
            ResizeStack(stk, stk->capacity * 2, filename, line);

        CHECK_RETURN;
    }

    stk->data[stk->size++] = elem;

    CHECK_RETURN;
}

int StackPop_(stack_t *const stk, char const *const filename, size_t const line) {
    CHECK_RETURN 0;

    RETURN_ERROR_IF(stk->size == 0, POP_NO_ITEMS) 0;

    if (stk->size <= stk->capacity/4 && stk->capacity != MIN_STACK_CAPACITY) {
        if (stk->capacity / 2 < MIN_STACK_CAPACITY)
            ResizeStack(stk, MIN_STACK_CAPACITY, filename, line);
        else
            ResizeStack(stk, stk->capacity/2, filename, line);
    }

    stk->size--;
    int value = stk->data[stk->size];
#ifndef NDEBUG
    stk->data[stk->size] = POISON_STACK_VALUE;
#endif

    CHECK_RETURN 0;
    return value;
}

void StackFinalize_(stack_t *const stk, char const *const filename, size_t const line) {
    RETURN_ERROR_IF(stk == NULL, IS_NULL);

#ifndef NDEBUG
    stk->data[-1] = POISON_STACK_VALUE;
    stk->data[stk->capacity] = POISON_STACK_VALUE;
    for (size_t i = 0; i < stk->capacity; i++)
        stk->data[i] = POISON_STACK_VALUE;
#endif

    free(stk->data-1);
    stk->data = NULL;
    stk->size = 0;
    stk->capacity = 0;
}

#undef RETURN_ERROR_IF
#undef SET_ERROR
#undef ADD_ERROR
#undef GET_ERROR_CODE
#undef TRUE_BIT
#undef CHECK_RETURN
