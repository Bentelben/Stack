#include "../byteio.h"

#include "stack.h"
#include "processor.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    if (argc == 1)
        printf("Error: no input file\n");
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return -1;
    }

    char const *const filename = argv[1];

    processor_t processor = {};
    ProcessorInitialize(&processor, filename);
    
    if (processor.error != 0) {
        return -1;
    }

    while (CanRead(processor.reader)) {
        ExecuteInstruction(&processor);
        //StackDump(stdout, &stack);
        if (processor.error != 0)
            break;
    }

    ProcessorFinalize(&processor);
}
