#include "stack.h"

#include <assert.h>
#include <stdint.h>


#ifndef NDEBUG
    #define CHECK_ERROR(stk) (((stk) == NULL || ((stk)->error = StackVerify(stk)) != STACK_NO_ERROR) && (StackDump(stderr, stk), 1))
#else
    #define CHECK_ERROR(stk)
#endif

#define SET_ERROR(stk, error_code) {if ((stk) != NULL) (stk)->error = (error_code); StackDump(stderr, stk);}

static void FPrintStackError(FILE *const file, stack_error_t const error) {
    assert(file);

    switch (error) {
#define _PRINT_ERROR_ENUM(value)                          \
        case STACK_ ## value ## _ERROR:                   \
            fprintf(file, #value "_ERROR" "\n"); \
            break;
        _PRINT_ERROR_ENUM(NO)
        _PRINT_ERROR_ENUM(ALLOC)
        _PRINT_ERROR_ENUM(NULL)
        _PRINT_ERROR_ENUM(DATA_NULL)
        _PRINT_ERROR_ENUM(POP_NO_ITEMS)
        _PRINT_ERROR_ENUM(SIZE_OVERFLOW_CAPACITY)
        _PRINT_ERROR_ENUM(PUSH_MAX_CAPACITY_SIZE)
#undef _PRINT_ERROR_ENUM
        default:
            fprintf(file, "%d\n", error);
            break;
    }
}

void _StackDump(FILE *file, stack_t const *const stk, char const *const filename, size_t const line) {
    if (file == NULL) {
        file = stderr;
        fprintf(file, "Dump output file not specified!\n");
    }

    fprintf(file, "StackDump called from from %s:%zu\n", filename, line);
    fprintf(file, "Stack [%p]\n", stk);

    fprintf(file, "{\n");

    if (stk == NULL)
        goto CLOSE_STACK_BRACE;

    fprintf(file, "    size     = %zu\n", stk->size);
    fprintf(file, "    capacity = %zu\n", stk->capacity);
    fprintf(file, "    error    = ");
    FPrintStackError(file, stk->error);
    fprintf(file, "    data [%p]\n", stk->data);
    fprintf(file, "    {\n");

    if (stk->data == NULL)
        goto CLOSE_DATA_BRACE;

    for (size_t i = 0; i < stk->capacity; i++) {
        fprintf(file, "       ");
        if (i < stk->size)
            fprintf(file, "*");
        else
            fprintf(file, " ");

        fprintf(file, "[%zu] = %d", i, stk->data[i]);

        if (i >= stk->size && stk->data[i] == DEFAULT_STACK_VALUE)
            fprintf(file, " (unset)");
        fprintf(file, "\n");
    }

    CLOSE_DATA_BRACE:
    fprintf(file, "    }\n");

    CLOSE_STACK_BRACE:
    fprintf(file, "}\n");
}

static stack_error_t StackVerify(const stack_t *const stk) {
    if (stk == NULL)
        return STACK_NULL_ERROR;
    if (stk->error != STACK_NO_ERROR)
        return stk->error;
    if (stk->data == NULL)
        return STACK_DATA_NULL_ERROR;
    if (stk->size > stk->capacity)
        return STACK_SIZE_OVERFLOW_CAPACITY_ERROR;

    return STACK_NO_ERROR;
}

void StackInitialize(stack_t *const stk) {
    size_t const INIT_STACK_CAPACITY = 8;

    if (stk == NULL) {
        SET_ERROR(stk, STACK_NULL_ERROR);
        return;
    }

    stk->size = 0;
    stk->capacity = INIT_STACK_CAPACITY;
    stk->error = STACK_NO_ERROR;
    stk->data = (int *)calloc(stk->capacity, sizeof(*stk->data));
    if (stk->data == NULL) {
        SET_ERROR(stk, STACK_ALLOC_ERROR);
        return;
    }

#ifndef NDEBUG
    for (size_t i = 0; i < stk->capacity; i++)
        stk->data[i] = DEFAULT_STACK_VALUE;
#endif

    if (CHECK_ERROR(stk)) return;
}

void StackPush(stack_t *const stk, int const elem) {
    if (CHECK_ERROR(stk)) return;

    if (stk->capacity == 0 || stk->size > stk->capacity - 1) {
        if (stk->capacity == SIZE_MAX) {
            SET_ERROR(stk, STACK_PUSH_MAX_CAPACITY_SIZE_ERROR);
            return;
        }
        if (stk->capacity > SIZE_MAX / 2)
            stk->capacity = SIZE_MAX;
        else
            stk->capacity *= 2;

        stk->data = (int *)realloc(stk->data, stk->capacity*sizeof(stk->data));
        if (stk->data == NULL) {
            SET_ERROR(stk, STACK_ALLOC_ERROR);
            return;
        }
#ifndef NDEBUG
        for (size_t i = stk->capacity/2; i < stk->capacity; i++)
            stk->data[i] = DEFAULT_STACK_VALUE;
#endif
    }

    stk->data[stk->size++] = elem;

    if (CHECK_ERROR(stk)) return;
}

int StackPop(stack_t *const stk) {
    if (CHECK_ERROR(stk)) return 0;

    if (stk->size == 0) {
        SET_ERROR(stk, STACK_POP_NO_ITEMS_ERROR);
        return 0;
    }

    int value = stk->data[(stk->size--) - 1];

    if (CHECK_ERROR(stk)) return 0;
    return value;
}

void StackFinalize(stack_t *const stk) {
    if (stk == NULL) {
        SET_ERROR(stk, STACK_NULL_ERROR);
        return;
    }
    free(stk->data);
    stk->data = NULL;
    stk->size = 0;
    stk->capacity = 0;
}

#undef CHECK_ERROR
#undef SET_ERROR
