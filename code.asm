; a*x^2 + b*x + c = 0
;
; D = b*b - 4 * a * c

PUSH 2 ; a
PUSH 5 ; b
PUSH 10 ; c

POPR 2
POPR 1
POPR 0

PUSHR 1
PUSHR 1
MUL

PUSH 4
PUSHR 0
PUSHR 2
MUL
MUL
SUB

OUT
HLT
