#include "stack.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


int PUSH_cmd(stack_t *stk) {
    int value = 0;
    if (scanf("%d", &value) != 1)
        return -1;
    StackPush(stk, value);
    return 0;
}

int OUT_cmd(stack_t *stk) {
    int value = StackPop(stk);
    printf("%d\n", value);
    return 0;
}

int ADD_cmd(stack_t *stk) {
    int value1 = StackPop(stk);
    int value2 = StackPop(stk);

    StackPush(stk, value1 + value2);

    return 0;
}

int SUB_cmd(stack_t *stk) {
    int value1 = StackPop(stk);
    int value2 = StackPop(stk);

    StackPush(stk, value1 - value2);

    return 0;
}

int MUL_cmd(stack_t *stk) {
    int value1 = StackPop(stk);
    int value2 = StackPop(stk);

    StackPush(stk, value1 * value2);

    return 0;
}

int DIV_cmd(stack_t *stk) {
    int value1 = StackPop(stk);
    int value2 = StackPop(stk);

    StackPush(stk, value1 / value2);

    return 0;
}

int SQRT_cmd(stack_t *stk) {
    int value = StackPop(stk);

    StackPush(stk, (int)sqrt(value));

    return 0;
}

// TODO error handling

int main() {
    stack_t stack = {};
    stack_t *stk = &stack;
    StackInitialize(stk);

    while (1) {
        char command[256] = {};
        if (scanf("%255s", command) != 1)
            return -1; 
        int c = getchar();
        if (c != EOF && c != ' ' && c != '\n')
            return -1;       

#define CHECK_CMD(cmd) if (strcmp(command, #cmd) == 0) cmd ## _cmd(stk); else

        CHECK_CMD(PUSH)
        CHECK_CMD(OUT)
        CHECK_CMD(ADD)
        CHECK_CMD(SUB)
        CHECK_CMD(MUL)
        CHECK_CMD(DIV)
        if (strcmp(command, "HLT") == 0)
            break;
        else
            return -1;
#undef CHECK_CMD
        StackDump(stdout, stk);
    }

    StackFinalize(stk);
}
