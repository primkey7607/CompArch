.text
sub X1, X1, 1000
mov X2, 10
mov X3, 40
mov X4, 0

foo:
cmp X1,0
bne deca
cmp X2,0
bne decb

deca:
add X1, X1, 1

decb:
sub X2, X2, 1

cmp X1, X2
beq incloop

incloop:
add X4, X4, 1
cmp X3, X4
bgt foo

HLT 0

