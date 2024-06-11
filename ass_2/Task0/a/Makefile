
all: myShell

# Tool invocations
# Executable "hello" depends on the files numbers.o and main.o and add.s.
myShell: myshell.o LineParser.o
	gcc -g -m32 -Wall -o myShell myshell.o LineParser.o

myshell.o: myshell.c
	gcc -g -m32 -Wall -c -o myshell.o myshell.c

LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

.PHONY: clean

clean:
	rm -f *.o myShell