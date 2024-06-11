CFLAGS=-m32 -Wall -ansi -c -nostdlib -fno-stack-protector

task0: start.o main.o util.o
	ld -m elf_i386 start.o main2.o util.o -o task0

start.o: start.s
	nasm -f elf32 start.s -o start.o

util.o: util.c
	gcc $(CFLAGS) util.c -o util.o

main.o: main.c
	gcc $(CFLAGS) main.c -o main.o

.PHONY: clean

clean:
	rm -f *.o task0
