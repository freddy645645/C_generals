include ../Make.defines

PROGS =	server
CXX = g++

all:	${PROGS}
server: server.o server_comm.o data_format.o game.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBS} -lstdc++
	
data_format.o: data_format.c
	${CC} ${CFLAGS} -o $@  -c $^ ${LIBS} 
server_comm.o: server_comm.c
	${CC} ${CFLAGS} -o $@ -c $^ ${LIBS} 
game.o: game.cpp
	${CXX} ${CFLAGS} -o $@ -c $^ ${LIBS} -fpermissive -fsanitize=address
clean:
	rm -f ${PROGS} ${CLEANFILES}
