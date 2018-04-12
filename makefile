CC = gcc
RM = rm -f *.o

all: execute

execute.o: execute.c
	$(CC) -c $^

execute: execute.o
	$(CC) -o $@ $^
	$(RM)

clean:
	$(RM)
