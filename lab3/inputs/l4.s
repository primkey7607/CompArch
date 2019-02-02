.text
mov X1, 1000
mov X2, 0
mov X3, 0
mov X4, 0
mov X5, 0
mov X6, 0
mov X7, 500

foo:
cmp X3, X5
beq goo

add X6, X6, 1
cmp X6, X5
bgt hoo

goo:
cmp X4, X5
beq hoo


add X6, X6, 1
cmp X6, X5
bgt hoo

hoo:
cmp X3, X4
bne ioo

cmp X2, X7
bne ioo

add X3, X3, 1
add X4, X4, 1


ioo:
add X2, X2, 1
cmp X1, X2
bgt foo

HLT 0
