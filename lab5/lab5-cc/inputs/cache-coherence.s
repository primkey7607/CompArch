.text
mov X1, 0

mov X28, 0

mov X30, 1
eret
cmp X29, X1
bne cpu1

mov X30, 2
eret
cmp X29, X1
bne cpu2

mov X30, 3
eret
cmp X29, X1
bne cpu3

mov X2, 0x1000
lsl X2, X2, 16
ldur X7, [X2, 0x90]
ldur X8, [X2, 0xc0]

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
    
	
    ldur X6, [X2, 0x0]  
    stur X4, [X2, 0x0]
    mov X1, 0
    ldur X9, [X2, 0x0]
	
    b start
cpu2:
#thread ID 2
    mov X28, 2
   
    mov X2, 0x1000
    lsl X2, X2, 16
	
	
    ldur X8, [X2, 0x0]
    mov X1, 0
    stur X5, [X2, 0x0]
    ldur X4, [X2, 0x0]

    b start
cpu3:
#thread ID 3
    mov X28, 3
    
    mov X2, 0x1000
    lsl X2, X2, 16
    
    ldur X16, [X2, 0xF0]
	
    ldur X8, [X2, 0x30]
    ldur X4, [X2, 0x30]
    stur X9, [X2, 0x30]
	
    mov X1, 0
    ldur X10, [X2, 0xc0]
    stur X11, [X2, 0x90]
    ldur X12, [X2, 0x90]
    ldur X13, [X2, 0xc0]
	
    mov X1, 0
    mov X1, 0
    mov X1, 0
	
    b start

start:
    # all four CPUs come here, with thread ID (0 to 3)
    # console-out syscall
    mov X30, 11
    add X29, X28, 0
    eret

    hlt 0

