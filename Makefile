C = gcc
CFLAGS = -Wall -g -pedantic

all: minls minget
	false

minls: minls.o minfs.h
	$(CC) $(CFLAGS) -o minls minls.o

minget: minget.o minfs.h
	$(CC) $(CFLAGS) -o minget minls.o

minls.o: minls.c
	$(CC) $(CFLAGS) -c minls.c

minget.o: minget.c
	$(CC) $(CFLAGS) -c minget.c

clean:
	rm minls.o minget.o

