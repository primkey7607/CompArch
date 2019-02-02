.text
mov X3, 0x1
mov X4, 0x2
cmp X3, X3
bne bntake
add X19, X19, 1
add X19, X19, 2
bntake:
add X19, X19, 3
add X19, X19, 4
HLT 0
