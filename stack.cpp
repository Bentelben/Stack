#include "stack.h"

#include <assert.h>


#ifndef NDEBUG
    #define CHECK_STACK(stk) ((stk) != NULL && ((stk)->error = StackVerify(stk)) != STACK_NO_ERROR && (StackDump(stderr, stk), 1))
#else
    #define CHECK_STACK(stk)
#endif

#define SET_ERROR(stk, error_code) {if ((stk) != NULL) (stk)->error = (error_code); StackDump(stderr, stk);}

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
    fprintf(file, "    error    = %d\n", stk->error);
    fprintf(file, "    data [%p]\n", stk->data);
    fprintf(file, "    {\n");

    if (stk->data == NULL)
        goto CLOSE_DATA_BRACE;

    for (size_t i = 0; i < stk->capacity; i++) {
        fprintf(file, "       %c[%zu] = %d%s\n", (i < stk->size) ? '*' : ' ', i, stk->data[i], ( i >= stk->size && stk->data[i] == DEFAULT_STACK_VALUE) ? " (unset)" : "");
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
        return STACK_SIZE_OVERFLOW_CAPACITY;

    return STACK_NO_ERROR;
}

void StackInitialize(stack_t *const stk) {
    if (stk == NULL)
        return;

    size_t const INIT_STACK_CAPACITY = 8;

    stk->error = STACK_NO_ERROR;
    stk->size = 0;
    stk->capacity = INIT_STACK_CAPACITY;
    stk->data = (int *)calloc(stk->capacity, sizeof(*stk->data));
    if (stk->data == NULL) {
        SET_ERROR(stk, STACK_ALLOC_ERROR);
        return;
    }

#ifndef NDEBUG
    for (size_t i = 0; i < stk->capacity; i++)
        stk->data[i] = DEFAULT_STACK_VALUE;
#endif

    if (CHECK_STACK(stk)) return;
}

void StackPush(stack_t *const stk, int const elem) {
    if (CHECK_STACK(stk)) return;

    if (stk->size + 1 > stk->capacity) {
        stk->capacity *= 2; // TODO check for size_t overflow
        stk->data = (int *)realloc(stk->data, stk->capacity*sizeof(stk->data));
        if (stk->data == NULL) {
            SET_ERROR(stk, STACK_ALLOC_ERROR);
            return;
        }
#ifndef NDEBUG
        for (size_t i = stk->capacity/2; i < stk->capacity; i++)
            stk->data[i] = DEFAULT_STACK_VALUE;
#endif
        // TODO memset zero
    }

    stk->data[stk->size++] = elem;

    if (CHECK_STACK(stk)) return;
}

int StackPop(stack_t *const stk) {
    if (CHECK_STACK(stk)) return 0;

    if (stk->size == 0) {
        SET_ERROR(stk, STACK_POP_NO_ITEMS_ERROR);
        return 0;
    }

    int value = stk->data[(stk->size--) - 1];

    if (CHECK_STACK(stk)) return 0;
    return value;
}

void StackFinalize(stack_t *const stk) {
    if (stk == NULL)
        return;
    free(stk->data);
    stk->data = NULL;
    stk->size = 0;
    stk->capacity = 0;
}

#undef CHECK_STACK
#undef SET_ERROR
