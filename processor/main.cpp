#include "../byteio.h"

#include "stack.h"
#include "executor.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static void HandleExecutorError(executor_error_t const error, stack_t *stk, reader_t *reader) {
    printf("Runtime error: ");
    switch (error) {
        case EXECUTOR_READING_ERROR:
            printf("Program reading error\n");
            break;
        case EXECUTOR_UNKNOWN_INSTRUCTION_ERROR:
            printf("Unknown instruction reached: `%c`\n", reader->array[reader->index-1]);
            break;
        case EXECUTOR_STACK_ERROR:
            printf("Stack error: ");
            FPrintStackError(stdout, stk->error);
            printf("\n");
            break;

        case EXECUTOR_DIVISION_BY_ZERO_ERROR:
            printf("Attempt to divide by zero\n");
            break;
        case EXECUTOR_SQRT_OF_NEGATIVE_ERROR:
            printf("Attemp to get square root of negative value\n");
            break;

        case EXECUTOR_NO_ERROR:
        default:
            assert(0);
            break;
    };
}

int main(int argc, char *argv[]) {
    if (argc == 1)
        printf("Error: no input file\n");
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return -1;
    }

    char const *const filename = argv[1];

    reader_t reader = {};
    if (InitializeReader(&reader, filename) == -1) {
        printf("Unable to open bytecode `%s`\n", filename);
        return -1;
    }

    stack_t stack = {};
    StackInitialize(&stack);

    executor_error_t error = EXECUTOR_NO_ERROR;
    while (CanRead(&reader)) {
        error = ExecuteInstruction(&reader, &stack);
        //StackDump(stdout, &stack);
        if (error != EXECUTOR_NO_ERROR)
            break;
    }

    if (error != EXECUTOR_NO_ERROR)
        HandleExecutorError(error, &stack, &reader);

    FinalizeReader(&reader);
    StackFinalize(&stack);
}
