myshell: myshell.o LineParser.o
	gcc -g -m32 -Wall -o myshell myshell.o LineParser.o

myshell.o: myshell.c
	gcc -g -m32 -Wall -c -o myshell.o myshell.c

LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

mypipeline: mypipeline.o mypipeline.o
	gcc -g -m32 -Wall -o mypipeline mypipeline.o

mypipeline.o: mypipeline.c
	gcc -g -m32 -Wall -c -o mypipeline.o mypipeline.c

.PHONYmyshell: cleanmyshell

clean:
	rm -f *.o mypipeline