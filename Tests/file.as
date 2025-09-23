;file.as
NAME:.extern H

.define size = -5

L1data: .data +3,-4,6,9,100
cmp L1data[2],#-9
MOVE: add #-6,H
F5: bne r4
prn #size
END: hlt
bne END

mov L1data[2],L1data[0]
NEG: not H
T0:jmp WASD
WASD: .string "def123"

