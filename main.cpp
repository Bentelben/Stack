#include "stack.h"

#include <stdio.h>
#include <assert.h>

int main() {
    stack_t stack = {};
    stack_t *stk = &stack;

    StackInitialize(stk);

    StackPush(stk, 11);
    StackPush(stk, 123);

    for (int i = 0; i < 23; i++)
        StackPush(stk, i);

    StackPop(stk);
    StackPop(stk);
    StackPop(stk);
   
    STACK_DUMP(stderr, stk);

    StackFinalize(stk);
    StackFinalize(stk);
    StackInitialize(stk);
    StackFinalize(stk);
    StackFinalize(stk);
}
