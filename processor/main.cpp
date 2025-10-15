#include "processor.h"

#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error: no input file\n");
        printf("Usage: %s <input_file>\n", argv[0]);
        return -1;
    }

    char const *const filename = argv[1];

    processor_t processor = {};
    ProcessorInitialize(&processor, filename);
    
    if (processor.error != 0) {
        return -1;
    }

    while (CanRead(&processor.reader)) {
        ExecuteInstruction(&processor);
        //StackDump(stdout, &processor.stack);
        if (processor.error != 0)
            break;
    }

    ProcessorFinalize(&processor);
}
