; a*x^2 + b*x + c = 0
;
; D = b*b - 4 * a * c

PUSH 10 ; b
PUSH 10 ; b
MUL
PUSH 4
PUSH 6 ; a
PUSH 4 ; c
MUL
MUL
SUB
OUT
HLT
