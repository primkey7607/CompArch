.text
mov X1, 0x1000
lsl X1, X1, 16
mov X10, 0x1234
stur X10, [X1, 0x0]
ldur X4, [X1,0x0]
sub X2, X4, X1
cbz X2, skip
stur X2, [X1,0x0]
sub X4, X5, X6
skip:
orr X7, X6, X8
add X1, X2, X7
HLT 0
