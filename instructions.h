#ifndef INSTRUCTION_H
#define INSTRUCTION_H

static char const COMMENT_SYMBOL = ';';

#ifdef PROCESSOR
    #include "executor.h"
    #include "stack.h"
    typedef int (*executor_function_t)(code_reader_t *, *stk)
#endif

struct instruction_t {
    char const *name;
    size_t argument_count;
#ifdef PROCESSOR
    executor_function_t func;
#endif
};

#ifdef PROCESSOR
    #define EXECUTOR(function_name) , function_name
#else
    #define EXECUTOR(function_name)
#endif

#define INSTRUCTION(name, argument_count) {#name, argument_count EXECUTOR(name)}

static const instruction_t INSTRUCTIONS[] = {
    INSTRUCTION(HLT,  0),
    INSTRUCTION(PUSH, 1),
    INSTRUCTION(ADD,  0),
    INSTRUCTION(SUB,  0),
    INSTRUCTION(MUL,  0),
    INSTRUCTION(DIV,  0),
    INSTRUCTION(OUT,  0)
};

#undef INSTRUCTION
#undef EXECUTOR

static size_t const INSTRUCTION_COUNT = sizeof(INSTRUCTIONS)/sizeof(*INSTRUCTIONS);

#endif
