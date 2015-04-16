#
# Make file for Lab 2 - ICS 53
#

C = gcc
CFLAGS = -I.
DEPS =  shell.h
OBJ = shell.o 

%.o: %.c $(DEPS)
	$(CC) -g -c -Wall -o $@ $< $(CFLAGS)

record: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:	
	rm -f record *.o
