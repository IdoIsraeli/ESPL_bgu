all: bubbleSort hexPrint

hexPrint: hexPrint.o
		gcc -g -Wall -o hexPrint  hexPrint.o

hexPrint.o: hexPrint.c
		gcc -g -Wall -c -o hexPrint.o hexPrint.c

bubbleSort: bubbleSort.o
		gcc -g -Wall -o bubbleSort  bubbleSort.o

bubbleSort.o: bubbleSort.c
		gcc -g -Wall -c -o bubbleSort.o bubbleSort.c

.PHONY: clean

clean:
		rm -f *.o bubbleSort hexPrint


