myshell: myshell.o LineParser.o
	gcc -g -m32 -Wall -o myshell myshell.o LineParser.o

myshell.o: myshell.c
	gcc -g -m32 -Wall -c -o myshell.o myshell.c

LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

mypipe: mypipe.o mypipe.o
	gcc -g -m32 -Wall -o mypipe mypipe.o

mypipe.o: mypipe.c
	gcc -g -m32 -Wall -c -o mypipe.o mypipe.c

.PHONYmyshell: cleanmyshell

clean:
	rm -f *.o mypipe