all: serial parallel sernopt parnopt 

serial: serial.c
	gcc -o serial -O4 serial.c
parallel: parallel.c
	gcc -o parallel -O4 parallel.c -pthread
sernopt: sernopt.c
	gcc -o sernopt -O4 sernopt.c
parnopt: parnopt.c
	gcc -o parnopt -O4 parnopt.c -pthread
clean: 
	rm -i sernopt serial parnopt parallel


