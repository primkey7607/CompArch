.text
add X3, X3, 5
add X4, X4, 5
cbz X3, foo
add X2, X0, 1000

foo:
add X8, X0, 1000

HLT 0
