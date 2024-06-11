all: task4

task4: task4.o 
	gcc -m32 -g -Wall -o task4 task4.o 

task4.o: task4.c
	gcc -g -Wall -m32 -fno-pie -c -o task4.o task4.c

.PHONY: clean

clean: 
	rm -f *.o task4