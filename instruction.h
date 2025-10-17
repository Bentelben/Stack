#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

typedef uint8_t register_code_t;
typedef uint8_t instruction_code_t;
typedef size_t instruction_pointer_t;

#define INSTRUCTION_(name, argument_count) name ## _code ,
enum instruction_enum_t {
#include "instruction_list.h"
};
#undef INSTRUCTION


#endif
