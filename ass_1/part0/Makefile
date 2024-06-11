program: count-words.o
		gcc -m32 -g -Wall -o program  count-words.o

count-words.o: count-words.c
		 gcc -g -Wall -m32 -c -o count-words.o count-words.c

.PHONY: clean

clean:
		rm -f *.o program

