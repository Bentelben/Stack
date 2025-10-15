// Before using this header you MUST define INSTRUCTION_(a, b) macro

//            Name  Argument
//                   count
INSTRUCTION_(  HLT,    0)
INSTRUCTION_(  PUSH,   1)
INSTRUCTION_(  OUT,    0)
INSTRUCTION_(  ADD,    0)
INSTRUCTION_(  SUB,    0)
INSTRUCTION_(  MUL,    0)
INSTRUCTION_(  DIV,    0)
INSTRUCTION_(  SQRT,   0)

INSTRUCTION_(  POPR,   1)
INSTRUCTION_(  PUSHR,  1)

INSTRUCTION_(  JMP,    1)

#undef INSTRUCTION_
