encoder: encoder.o
		gcc -g -Wall -o encoder  encoder.o

encoder.o: encoder.c
		gcc -g -Wall  -c -o encoder.o encoder.c

.PHONY: clean

clean:
		rm -f *.o encoder


