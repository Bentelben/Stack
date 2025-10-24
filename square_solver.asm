IN
POPR AX
IN
POPR BX
IN
POPR CX

PUSHR AX
CALL :IS_ZERO
PUSH 1
JE :A_ZERO
    ; a != 0
    CALL :CALC_DISCRIMINANT
    POPR DX
    PUSHR DX
    CALL :IS_ZERO
    PUSH 1
    JE :D_ZERO
        ; a != 0 && D != 0
        PUSHR DX
        PUSH 0
        JB :D_NEGATIVE
            ; a != 0 && D > 0
            PUSH 2
            OUT
            PUSHR DX
            SQRT
            PUSH 2
            PUSHR AX
            MUL
            DIV
            POPR DX ; DX = sqrt(D)/(2*a)
            PUSHR BX
            PUSH -2
            PUSHR AX
            MUL
            DIV
            POPR EX ; EX = b/(-2*a)
            PUSHR EX
            PUSHR DX
            ADD
            OUT
            PUSHR EX
            PUSHR DX
            SUB
            OUT
            HLT

        :D_NEGATIVE
            ; a != 0 && D < 0
            PUSH 0
            OUT
            HLT

    :D_ZERO
        ; a != 0 && D == 0
        PUSH 1
        OUT
        PUSHR BX
        PUSHR AX
        PUSH -2
        MUL
        DIV
        OUT
        HLT

:A_ZERO
    ; a == 0
    PUSHR BX
    CALL :IS_ZERO
    PUSH 1
    OUT
    PUSH 1
    JE :A_ZERO_B_ZERO
        ; a == 0 && b != 0
        PUSH 1
        OUT
        PUSH -1
        PUSHR CX
        PUSHR BX
        DIV
        MUL
        OUT
        HLT

    :A_ZERO_B_ZERO
        ; a == 0 && b == 0
        PUSHR CX
        CALL :IS_ZERO
        PUSH 1
        JE :A_ZERO_B_ZERO_C_ZERO
            ; a == 0 && b == 0 && c != 0
            PUSH 0
            OUT
            HLT

        :A_ZERO_B_ZERO_C_ZERO
            ; a == 0 && b == 0 && c == 0
            PUSH -1
            OUT
            HLT

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:CALC_DISCRIMINANT
PUSHR BX
PUSHR BX
MUL
PUSH 4
PUSHR AX
PUSHR CX
MUL
MUL
SUB
RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:IS_ZERO
POPR FX
PUSHR FX
PUSH 1e-14

JBE :IS_ZERO_1
PUSH 0
RET

:IS_ZERO_1
PUSHR FX
PUSH -1e-14
JAE :IS_ZERO_2
PUSH 0
RET

:IS_ZERO_2
PUSH 1
RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;
