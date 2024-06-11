program: base.o
		gcc -m32 -g -Wall -o program  base.o

base.o: base.c
		gcc -g -Wall -m32 -c -o base.o base.c

.PHONY: clean

clean:
		rm -f *.o program

