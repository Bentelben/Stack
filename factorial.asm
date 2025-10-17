PUSH 3
CALL :factorial(x)wd
OUT
HLT

; Factorial(x)
:factorial(x)
POPR AX
PUSHR AX
PUSH 1
JA :AX_BIGGER_ONE

; else
PUSH 1
RET

:AX_BIGGER_ONE
PUSHR AX
PUSHR AX
PUSH 1
SUB
CALL :factorial(x)
MUL
RET
