.text
add X4, X4, 1
lsl X4, X4, 22
add X4, X4, 0x24
add X5, X5, 1
lsl X5, X5, 28
stur X4, [X5, 0x0]
ldur X1, [X5, 0x0]
br X1
add X3, X3, 3
add X2, X3, 1
HLT 0
