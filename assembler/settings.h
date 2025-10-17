#ifndef ASSEMBLER_SETTINGS_H
#define ASSEMBLER_SETTINGS_H

#include "../instruction.h"

static char const COMMENT_SYMBOL = ';';
static char const LABEL_SYMBOL = ':';

static char const *REGISTERS[] = {
    "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX"
};

static register_code_t const REGISTER_COUNT = (register_code_t)(sizeof(REGISTERS)/sizeof(*REGISTERS));

struct instruction_t {
    char const *name;
    size_t argument_count;
};

#define INSTRUCTION_(name, argument_count) {#name, argument_count},
static const instruction_t INSTRUCTIONS[] = {
    #include "../instruction_list.h"
};
#undef INSTRUCTION_

static instruction_code_t const INSTRUCTION_COUNT = (instruction_code_t)(sizeof(INSTRUCTIONS)/sizeof(*INSTRUCTIONS));

#endif
