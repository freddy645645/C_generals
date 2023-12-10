include ../Make.defines

PROGS =	server

all:	${PROGS}
server: server.o server_comm.o data_format.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBS} 
	
data_format.o: data_format.c
	${CC} ${CFLAGS} -o $@  -c $^ ${LIBS} 
server_comm.o: server_comm.c
	${CC} ${CFLAGS} -o $@ -c $^ ${LIBS} 
clean:
		rm -f ${PROGS} ${CLEANFILES}
