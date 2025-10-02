#ifndef INSTRUCTION_H
#define INSTRUCTION_H

static char const COMMENT_SYMBOL = ';';

struct instruction_t {
    size_t code;
    char const *name;
    size_t argument_count;
};

static const instruction_t INSTRUCTIONS[] = {
    {0, "HLT",  0},
    {1, "PUSH", 1},
    {2, "ADD",  0},
    {3, "SUB",  0},
    {4, "MUL",  0},
    {5, "DIV",  0},
    {6, "OUT",  0}
};

static size_t const INSTRUCTION_COUNT = sizeof(INSTRUCTIONS)/sizeof(*INSTRUCTIONS);

#endif
