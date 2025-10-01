#include "stack.h"

#include <assert.h>

#ifndef NDEBUG
    #define CHECK_ERROR(stk) (((stk) == NULL || ((stk)->error = StackVerify(stk)) != STACK_NO_ERROR) && (StackDump(stderr, stk), 1))
#else
    #define CHECK_ERROR(stk)
#endif

#define SET_ERROR(stk, error_code) {if ((stk) != NULL) (stk)->error = (error_code); StackDump(stderr, stk);}

// TODO RESET ERROR

#ifndef NDEBUG
static stack_error_t StackVerify(const stack_t *const stk) {
    if (stk == NULL)
        return STACK_IS_NULL_ERROR;
    if (stk->error != STACK_NO_ERROR) // TODO multiple errors
        return stk->error;
    if (stk->data == NULL)
        return STACK_DATA_IS_NULL_ERROR;

    if (stk->size > stk->capacity)
        return STACK_CAPACITY_LESS_SIZE_ERROR;
    if (stk->capacity < MIN_STACK_CAPACITY)
        return STACK_CAPACITY_LESS_MIN_CAPACITY_ERROR;
    if (stk->capacity > MAX_STACK_CAPACITY)
        return STACK_CAPACITY_BIGGER_MAX_CAPACITY_ERROR;

    if (stk->capacity > MAX_STACK_CAPACITY)
        return STACK_KANAREYKA_DAMAGED_ERROR;
    if (stk->data[-1] != KANAREYKA_STACK_VALUE || stk->data[stk->capacity] != KANAREYKA_STACK_VALUE)
        return STACK_KANAREYKA_DAMAGED_ERROR;

    for (size_t i = stk->size; i < stk->capacity; i++) {
        if (stk->data[i] != POISON_STACK_VALUE)
            return STACK_POISON_DAMAGED_ERROR;
    }
    
    // TODO multiple errors
    return STACK_NO_ERROR;
}
#endif


static void ResizeStack(stack_t *const stk, size_t const new_capacity) {
    assert(stk);
    assert(stk->size <= new_capacity);

    if (new_capacity > MAX_STACK_CAPACITY || new_capacity < MIN_STACK_CAPACITY) {
        SET_ERROR(stk, STACK_ALLOCATION_ERROR);
        return;
    }

    if (stk->data != NULL)
        stk->data = stk->data - 1;

    int *new_data = (int *)realloc(stk->data, (1 + new_capacity + 1)*sizeof(*stk->data));
    if (new_data == NULL) {
        SET_ERROR(stk, STACK_ALLOCATION_ERROR);
        return;
    }

    new_data = new_data + 1;

    if (stk->data == NULL)
        new_data[-1] = KANAREYKA_STACK_VALUE;
    new_data[new_capacity] = KANAREYKA_STACK_VALUE;

#ifndef NDEBUG
    if (new_capacity > stk->capacity)
        for (size_t i = stk->capacity; i < new_capacity; i++)
            new_data[i] = POISON_STACK_VALUE;
#endif

    stk->data = new_data;
    stk->capacity = new_capacity;
}

#define PRINT_ERROR_ENUM_(value)                 \
        case STACK_ ## value ## _ERROR:          \
            fprintf(file, #value "_ERROR" "\n"); \
            break;

static void FPrintStackError(FILE *const file, stack_error_t const error) {
    assert(file);

    switch (error) {
        PRINT_ERROR_ENUM_(NO)

        PRINT_ERROR_ENUM_(IS_NULL)
        PRINT_ERROR_ENUM_(DATA_IS_NULL)

        PRINT_ERROR_ENUM_(ALLOCATION)

        PRINT_ERROR_ENUM_(CAPACITY_LESS_SIZE)
        PRINT_ERROR_ENUM_(CAPACITY_LESS_MIN_CAPACITY)
        PRINT_ERROR_ENUM_(CAPACITY_BIGGER_MAX_CAPACITY)

        PRINT_ERROR_ENUM_(POP_NO_ITEMS)
        PRINT_ERROR_ENUM_(PUSH_MAX_CAPACITY_SIZE)

        PRINT_ERROR_ENUM_(KANAREYKA_DAMAGED)
        PRINT_ERROR_ENUM_(POISON_DAMAGED)
        default:
            fprintf(file, "%d\n", error);
            break;
    }
}
#undef PRINT_ERROR_ENUM_

#define PRINT_(tab_count, format, ...) fprintf(file, "%*s" format, (int)((tab_count)*TAB_SIZE), "", ##__VA_ARGS__)

void _stackDump(FILE *file, stack_t const *const stk, char const *const filename, size_t const line) {
    const size_t TAB_SIZE = 4;

    if (file == NULL) {
        file = stderr;
        PRINT_(0, "Dump output file not specified!\n");
    }

    PRINT_(0, "StackDump called from from %s:%zu\n", filename, line);
    PRINT_(0, "Stack [%p]\n"
              "{\n", stk); 

    if (stk != NULL) {
        PRINT_(1, "size     = %zu\n", stk->size);
        PRINT_(1, "capacity = %zu (+2)\n", stk->capacity);
        PRINT_(1, "error    = ");
        FPrintStackError(file, stk->error);
        PRINT_(1, "data [%p]\n", stk->data);
        PRINT_(1, "{\n");

        if (stk->data != NULL) {
        
            PRINT_(2, " [-1] = %d (KANAREYKA)\n", stk->data[-1]);

            for (size_t i = 0; i < stk->capacity; i++) {
                PRINT_(2, "");
                if (i < stk->size)
                    fprintf(file, "*");
                else
                    fprintf(file, " ");
        
                fprintf(file, "[%zu] = %d", i, stk->data[i]);
        
                if (i >= stk->size && stk->data[i] == POISON_STACK_VALUE)
                    fprintf(file, " (POISON)");
                
                fprintf(file, "\n");
            }

            PRINT_(2, " [%zu] = %d (KANAREYKA)\n", stk->capacity, stk->data[stk->capacity]);
        }
        PRINT_(1, "}\n");
    }
    PRINT_(0, "}\n");
}
#undef PRINT_


void StackInitialize(stack_t *const stk) {
    if (stk == NULL) {
        SET_ERROR(stk, STACK_IS_NULL_ERROR);
        return;
    }

    stk->size = 0;
    stk->capacity = 0;
    stk->error = STACK_NO_ERROR;
    stk->data = NULL;
    ResizeStack(stk, INIT_STACK_CAPACITY);

    if (CHECK_ERROR(stk)) return;
}

void StackPush(stack_t *const stk, int const elem) {
    if (CHECK_ERROR(stk)) return;

    if (stk->size == stk->capacity) {
        if (stk->capacity == MAX_STACK_CAPACITY) {
            SET_ERROR(stk, STACK_PUSH_MAX_CAPACITY_SIZE_ERROR);
            return;
        }

        if (stk->capacity > MAX_STACK_CAPACITY/2)
            ResizeStack(stk, MAX_STACK_CAPACITY);
        else
            ResizeStack(stk, stk->capacity * 2);

        if (CHECK_ERROR(stk)) return;
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

    if (stk->size <= stk->capacity/4 && stk->capacity != MIN_STACK_CAPACITY) {
        if (stk->capacity / 2 < MIN_STACK_CAPACITY)
            ResizeStack(stk, MIN_STACK_CAPACITY);
        else
            ResizeStack(stk, stk->capacity/2);
    }

    stk->size--;
    int value = stk->data[stk->size];
#ifndef NDEBUG
    stk->data[stk->size] = POISON_STACK_VALUE;
#endif

    if (CHECK_ERROR(stk)) return 0;
    return value;
}

void StackFinalize(stack_t *const stk) {
    if (stk == NULL) {
        SET_ERROR(stk, STACK_IS_NULL_ERROR);
        return;
    }

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

#undef CHECK_ERROR
#undef SET_ERROR
