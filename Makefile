crack: crack.c sha256.o
		gcc -Wall crack.c sha256.o -o crack
sha256.o: sha256.c sha256.h
		gcc -c sha256.c
clean:
		rm crack sha256.o
dh:
		gcc -Wall -o dh dh.c
