#ifndef INSTRUCTION_H
#define INSTRUCTION_H

static char const COMMENT_SYMBOL = ';';

#ifdef PROCESSOR
    #include "byteio.h"
    #include "processor/stack.h"
    typedef int (*executor_function_t)(reader_t *, stack_t *);
#endif

struct instruction_t {
    char const *name;
    size_t argument_count;
#ifdef PROCESSOR
    executor_function_t func;
#endif
};

#ifdef PROCESSOR
    #define EXECUTOR(function_name) , Run ## function_name
#else
    #define EXECUTOR(function_name)
#endif

#define INSTRUCTION(name, argument_count) {#name, argument_count EXECUTOR(name)}

static const instruction_t INSTRUCTIONS[] = {
    INSTRUCTION(HLT,  0),
    INSTRUCTION(PUSH, 1),
    INSTRUCTION(OUT,  0),
    INSTRUCTION(ADD,  0),
    INSTRUCTION(SUB,  0),
    INSTRUCTION(MUL,  0),
    INSTRUCTION(DIV,  0),

    INSTRUCTION(SQRT,  0)
};

#undef INSTRUCTION
#undef EXECUTOR

static unsigned char const INSTRUCTION_COUNT = (unsigned char)(sizeof(INSTRUCTIONS)/sizeof(*INSTRUCTIONS));

#endif
