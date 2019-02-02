.text
mov X10, 10
mov X11, 15
mov X12, 20
mov X1, X10
mov X2, X11
mov X3, 0

oloop:
cmp x1, x12
bgt fin
cmp x2, x12
bgt fin

iloop:
cmp x2, x1
bgt greater

cmp x2, x1
beq equal

add x2, x2, 1
cmp x1, x2
bgt iloop

greater:
add x1, x1, 5
sub x2, x2, 5
b incloop

equal:
mov x1, x10
mov x2, x11

incloop:
add x3, x3, 1
cmp x12, x3
bgt oloop

fin:
HLT 0
