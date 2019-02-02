.text
mov X3, 0x1
mov X4, 0x2
b foo
add X13, X0, 10
foo:
add X14, X9, 11
b bar
bar:
add X15, X2, X8



cmp X3, X4
beq bcnot
add X16, X16, 1
add X16, X16, 2

bcnot:
add X16, X16, 3
add X16, X16, 4

HLT 0
