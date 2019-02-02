.text
add X3, X3, 5
add X4, X4, 5
cmp X3, X4
ble bar
add X2, X0, 10

foo:
add X8, X9, 11
b bar

bar:
add X10, X2, X8
HLT 0

