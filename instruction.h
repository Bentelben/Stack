#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <stdlib.h>

static size_t const REGISTER_COUNT = 8;
static char const COMMENT_SYMBOL = ';';

#ifdef PROCESSOR
    #include "processor/instruction_declaration.h"
    #define INSTRUCTION_(name, argument_count) {#name, argument_count, PROCESSOR_FUNCTION_NAME(name)},
    typedef PROCESSOR_FUNCTION_TEMPLATE((*processor_function_t));
#else
    #define INSTRUCTION_(name, argument_count) {#name, argument_count},
#endif

struct instruction_t {
    char const *name;
    size_t argument_count;
#ifdef PROCESSOR
    processor_function_t func;
#endif
};

static const instruction_t INSTRUCTIONS[] = {
    #include "instruction_list.h"
};

#undef INSTRUCTION_

static uint8_t const INSTRUCTION_COUNT = (uint8_t)(sizeof(INSTRUCTIONS)/sizeof(*INSTRUCTIONS));

#endif
