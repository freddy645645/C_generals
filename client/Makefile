all: client


client: client.o data_format.o globals.o cliaction.o cliscreen.o  clistate.o
	gcc -o $@ $^

client.o: client.c
	gcc -o $@ -c $^

data_format.o: data_format.c
	gcc -o $@ -c $^

globals.o: globals.c
	gcc -o $@ -c $^

cliaction.o: cliaction.c
	gcc -o $@ -c $^

cliscreen.o: cliscreen.c
	gcc -o $@ -c $^

clistate.o: clistate.c
	gcc -o $@ -c $^

test: test.c
	gcc -o $@ $^

clean:
	rm -f *.o
