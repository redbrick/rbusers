        CC = gcc
    CFLAGS = -O -pedantic  -g
       OBJ = rbusers.o utmp_lib.o extra_io.o alpha.o


BINARY = rbusers

all: ${OBJ}
	${CC} ${CFLAGS} -o ${BINARY} ${OBJ} #-lkvm


rbusers.o: rbusers.c
	${CC} ${CFLAGS} -c rbusers.c

extra_io.o: extra_io.c extra_io.h
	${CC} ${CFLAGS} -c extra_io.c
		
	
alpha.o: alpha.c alpha.h
	${CC} ${CFLAGS} -c alpha.c

utmp_lib.o: utmp_lib.c utmp_lib.h
	${CC} ${CFLAGS} -c utmp_lib.c 


