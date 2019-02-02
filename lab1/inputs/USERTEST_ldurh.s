.text
mov X1, 1024
lsl X1, X1, 18 
mov X3, 0x2345
sturh W3, [X1, 0x0]
ldurh W21, [X1, 0x0]
HLT 0
