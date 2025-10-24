PUSH 30
POPR SCREEN_HEIGHT
PUSH 90
POPR SCREEN_WIDTH

PUSH 15
POPR TOP_Y
PUSH -45
POPR LEFT_X

PUSH 0
POPR TX
:loop1
    CALL :fill_canvas
    PUSH 1
    PUSH TX
    ADD
    POPR TX
    JMP :loop1
HLT

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:rule
    CALL :rule_RT
    RET

:rule_Z
    CALL :coords
    PUSH 0
    PUSH 0
    PUSH 10
    CALL :func_A(x-B)+C-y
    CALL :above

    CALL :coords
    PUSH 0
    PUSH 0
    PUSH -10
    CALL :func_A(x-B)+C-y
    CALL :below

    CALL :or

    CALL :coords
    PUSH 0.5
    PUSH 0
    PUSH 0.5
    CALL :func_A(x-B)+C-y
    CALL :below

    CALL :coords
    PUSH 0.5
    PUSH 0
    PUSH -0.5
    CALL :func_A(x-B)+C-y
    CALL :above

    CALL :and
    CALL :or

    CALL :coords
    PUSH 0
    PUSH 0
    PUSH 10
    CALL :func_A(x-B)+C-y
    CALL :below

    CALL :and

    CALL :coords
    PUSH 0
    PUSH 0
    PUSH -10
    CALL :func_A(x-B)+C-y
    CALL :above
    
    CALL :and

    CALL :coords
    PUSH 0
    PUSH 0
    PUSH 930
    PUSH 220
    CALL :func_(x-a)^2/c+(y-b)^2/d-1
    CALL :below
    
    CALL :and

    RET


:rule_RT
    CALL :coords
    PUSH 0
    PUSH 0
    PUSH 750
    PUSH 175
    CALL :func_(x-a)^2/c+(y-b)^2/d-1
    CALL :above

    CALL :coords
    PUSH 0
    PUSH 0
    PUSH 930
    PUSH 220
    CALL :func_(x-a)^2/c+(y-b)^2/d-1
    CALL :below
    
    CALL :and


    CALL :coords
    PUSHR TX
    SIN
    PUSH 1
    ADD
    PUSH 0.4
    MUL
    PUSH 3
    PUSH -9
    CALL :func_Amod(x-B)+C-y
    CALL :below

    CALL :coords
    PUSHR TX
    SIN
    PUSH 1
    ADD
    PUSH 0.4
    MUL
    PUSH 3
    PUSH -10
    CALL :func_Amod(x-B)+C-y
    CALL :above

    CALL :and
    CALL :or
    RET

;;;;;;; FUNCTIONS ;;;;;;;;;

:func_A(x-B)+C-y
    POPR CX
    POPR BX
    POPR AX
    PUSHR BX
    SUB
    PUSHR AX
    MUL
    PUSHR CX
    ADD
    SUB

    RET

:func_Amod(x-B)+C-y
    POPR CX
    POPR BX
    POPR AX
    PUSHR BX
    SUB
    CALL :abs__X
    PUSHR AX
    MUL
    PUSHR CX
    ADD
    SUB
    RET

:func_(x-a)^2/c+(y-b)^2/d-1
    POPR DX
    POPR CX
    POPR BX
    POPR AX
    PUSHR AX
    SUB
    DUB
    MUL ; (x-a)^2
    PUSH CX
    DIV ; (x-a)^2 / c
    POPR TMP
    PUSHR BX
    SUB
    DUB
    MUL
    PUSHR DX
    DIV
    PUSHR TMP
    ADD
    PUSH 1
    SUB
    RET

;;;;;;;; L O G I C ;;;;;;;;
:above
    PUSH 0
    JAE :above_1
    PUSH 0
    RET
:above_1
    PUSH 1
    RET

:below
    PUSH 0
    JBE :below_1
    PUSH 0
    RET
:below_1
    PUSH 1
    RET

:or
    ADD
    RET

:and
    MUL
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:fill_canvas
    PUSH 0
    POPR GX
    CALL :cycle_y
    PUSHR SCREEN_HEIGHT
    PUSHR SCREEN_WIDTH
    DRAW 
    RET

:cycle_y
    PUSH 0
    POPR HX
    CALL :cycle_x

    PUSHR GX
    PUSH 1
    ADD
    POPR GX
    PUSHR GX
    PUSHR SCREEN_HEIGHT
    JB :cycle_y
    RET

:cycle_x
    CALL :calc_coords
    POPR EX
    POPR FX
    CALL :rule
    PUSH 0
    JNE :cycle_x_set_dot
    PUSH ' '
    CALL :set_dot
    JMP :cycle_x_continue

:cycle_x_set_dot
    PUSH '#'
    CALL :set_dot
    
:cycle_x_continue
    PUSHR HX
    PUSH 1
    ADD
    POPR HX
    PUSHR HX
    PUSHR SCREEN_WIDTH
    JB :cycle_x
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:set_dot
    CALL :calc_screen_index
    POPR TMP
    POPV TMP
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:calc_screen_index
    PUSHR GX
    PUSHR SCREEN_WIDTH
    MUL
    PUSHR HX
    ADD
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:calc_coords
    PUSHR GX
    PUSH -1
    MUL
    PUSHR TOP_Y
    ADD
    PUSHR HX
    PUSHR LEFT_X
    ADD
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;
:coords
    PUSHR FX
    PUSHR EX
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:abs__X
    POPR TMP
    PUSH TMP
    PUSH 0
    JB :abs_negative
    PUSH TMP
    RET

:abs_negative
    PUSH TMP
    PUSH -1
    MUL
    RET
