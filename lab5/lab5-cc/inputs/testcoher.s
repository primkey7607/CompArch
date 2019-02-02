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
ldur X7, [X2, 0x00]
stur X8, [X2, 0x90]
ldur X7, [X2, 0xc0]
ldur X7, [X2, 0x00]
mov X1, 0
mov X1, 0
mov X1, 0
mov X1, 0
mov X1, 0

ldur X7, [X2, 0x00]
ldur X7, [X2, 0x00]

mov X1, 0
mov X1, 0
mov X1, 0
mov X1, 0
mov X1, 0
b start

cpu1:
#thread ID 1
    mov X28, 1
  
    mov X4, 4
    mov X2, 0x1000
    lsl X2, X2, 16	
     
	
    ldur X6, [X2, 0x90]  
    stur X4, [X2, 0x0]
    b start

start:
    # all four CPUs come here, with thread ID (0 to 3)
    # console-out syscall
    mov X30, 11
    add X29, X28, 0
    eret

    hlt 0

