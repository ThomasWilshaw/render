CC = gcc
CFLAGS = -Wall -g
EXECUTE = drawmain.exe

drawmain: matmult.o stack.o main.o
	${CC} ${CFLAGS} matmult.o stack.o draw_2.o -o drawmain

matmult : matmult.c matrix.h
	${CC} ${CFLAGS} -c matmult.c
	
stack : stack.c stack.h matrix.h
	${CC} ${CFLAGS} -c stack.c

main.o : draw_1.c matrix.h stack.h objects.h
	${CC} ${CFLAGS} -c draw_2.c

.PHONY: clean

clean:
	rm -f ${EXECUTE} *.o