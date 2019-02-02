.text
cbz X8, foo
movz X3, 8
HLT 0

foo:
add X9, X8, 11
b bar

bar:
cbz X9, foo
HLT 0
