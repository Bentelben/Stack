PUSH 30
POPR SCREEN_HEIGHT
PUSH 90
POPR SCREEN_WIDTH

PUSH 15
POPR TOP_Y
PUSH -15
POPR LEFT_X

CALL :fill_canvas
HLT

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:fill_canvas
    PUSH 0
    POPR CX
    CALL :cycle_y
    PUSHR SCREEN_HEIGHT
    PUSHR SCREEN_WIDTH
    DRAW 
    RET

:cycle_y
    PUSH 0
    POPR DX
    CALL :cycle_x

    PUSHR CX
    PUSH 1
    ADD
    POPR CX
    PUSHR CX
    PUSHR SCREEN_HEIGHT
    JB :cycle_y
    RET

:cycle_x
    CALL :calc_coords
    CALL :function__X
    JB :cycle_x_set_dot
    PUSH ' '
    CALL :set_dot
    JMP :cycle_x_continue

:cycle_x_set_dot
    PUSH '#'
    CALL :set_dot
    
:cycle_x_continue
    PUSHR DX
    PUSH 1
    ADD
    POPR DX
    PUSHR DX
    PUSHR SCREEN_WIDTH
    JB :cycle_x
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:set_dot
    CALL :calc_screen_index
    POPR EX
    POPV EX
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:calc_screen_index
    PUSHR CX
    PUSHR SCREEN_WIDTH
    MUL
    PUSHR DX
    ADD
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:calc_coords
    PUSHR CX
    PUSH -1
    MUL
    PUSHR TOP_Y
    ADD
    PUSHR DX
    PUSHR LEFT_X
    ADD
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;
:function__X
    CALL :abs__X
    RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:abs__X
    POPR HX
    PUSH HX
    PUSH 0
    JB :abs_negative
    PUSH HX
    RET

:abs_negative
    PUSH HX
    PUSH -1
    MUL
    RET
