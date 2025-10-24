#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>


size_t const REGISTER_COUNT = 8;
size_t const RAM_SIZE = 200;
size_t const VSCREEN_HEIGHT = 20;
size_t const VSCREEN_WIDTH = 20;


typedef uint8_t register_code_t;
typedef uint8_t instruction_code_t;
typedef size_t instruction_pointer_t;

#define INSTRUCTION_(name, argument_count) name ## _code ,
enum instruction_enum_t {
#include "instruction_list.h"
};
#undef INSTRUCTION


#endif
