all: exec

exec: virusDetector.c
	gcc -g -c -o virusDetector.o virusDetector.c
	gcc -g virusDetector.o  -o virusDetector
	rm virusDetector.o

.PHONY: clean
clean:
	rm -rf ./*.o virusDetector
