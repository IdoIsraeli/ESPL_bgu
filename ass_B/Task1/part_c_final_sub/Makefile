all: exec

exec: AntiVirus.c
	gcc -g -m32 -c -o AntiVirus.o AntiVirus.c
	gcc -g -m32 AntiVirus.o  -o AntiVirus
	rm AntiVirus.o

.PHONY: clean
clean:
	rm -rf ./*.o AntiVirus
