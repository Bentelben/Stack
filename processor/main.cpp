#include "../byteio.h"

#include "stack.h"
#include "executor.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main() {
    char const *filename = "bytecode.txt";

    reader_t reader = {};
    if (InitializeReader(&reader, filename) == -1) {
        printf("Unable to open bytecode `%s`\n", filename);
        return -1;
    }

    stack_t stack = {};
    StackInitialize(&stack);

    int error = 0;
    while (CanRead(&reader)) {
        error = ExecuteInstruction(&reader, &stack);
        //StackDump(stdout, &stack);
        if (error != 0)
            break;
    }

    printf("Program finished with code `%d`\n", error);
    
    FinalizeReader(&reader);
    StackFinalize(&stack);
}
