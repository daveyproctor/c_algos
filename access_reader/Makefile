CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall

reader: main.o flash.o
	${CC} ${CFLAGS} -o reader main.o flash.o

main.o flash.o: flash.h

clean:
	rm -f main.o flash.o reader