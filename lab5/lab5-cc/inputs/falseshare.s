.text
mov X1, 0

mov X28, 0

mov X30, 1
eret
cmp X29, X1
bne cpu1

mov X8, 3
mov X4, 4
mov X2, 0x1000
lsl X2, X2, 16

mov X11, 100
mov X12, 0

foo:
stur X8, [X2, 0x80]
mov X1, 0
mov X1, 0
mov X1, 0

stur X8, [X2, 0x80]
mov X1, 0
mov X1, 0
mov X1, 0


stur X8, [X2, 0x82]
mov X1, 0
mov X1, 0
mov X1, 0

add X12, X12, 1

stur X7, [X2, 0x82]
mov X1, 0
mov X1, 0
mov X1, 0
stur X8, [X2, 0x84]
mov X1, 0
mov X1, 0
mov X1, 0
ldur X7, [X2, 0x84]

cmp X11, X12
bgt foo

mov X1, 0
mov X1, 0
mov X1, 0
mov X1, 0
mov X1, 0

b start

cpu1:
#thread ID 1
    mov X28, 1
  
    mov X2, 0x1000
    lsl X2, X2, 16	

mov X11, 100
mov X12, 0

foo2:
	
    stur X6, [X2, 0x88]  
mov X1, 0
mov X1, 0
mov X1, 0
    stur X8, [X2, 0x88]  
mov X1, 0
mov X1, 0
mov X1, 0
    stur X6, [X2, 0x88]  
mov X1, 0
mov X1, 0
mov X1, 0
    stur X8, [X2, 0x88]  
mov X1, 0
mov X1, 0
mov X1, 0
add X12, X12, 1
    stur X6, [X2, 0x90]  
mov X1, 0
mov X1, 0
mov X1, 0
    stur X8, [X2, 0x90]  
mov X1, 0
mov X1, 0
mov X1, 0

cmp X11, X12
bgt foo2


    b start

start:
    # all four CPUs come here, with thread ID (0 to 3)
    # console-out syscall
    mov X30, 11
    add X29, X28, 0
    eret

    hlt 0

