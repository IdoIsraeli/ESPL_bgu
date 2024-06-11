all: task0

task0: task0.o 
	gcc -m32 -g -Wall -o task0 task0.o 

task0.o: task0.c
	gcc -g -Wall -m32 -c -o task0.o task0.c

.PHONY: clean

clean: 
	rm -f *.o task0