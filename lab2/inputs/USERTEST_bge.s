.text

movz X3, 3
movz X4, 4
cmp X3, X3
bge foo

movz X5, 100
movz X6, 100

foo:
movz X7, 7
HLT 0
