#
# Make file for Lab 2 - ICS 53
#

C = gcc
CFLAGS = -I.
DEPS =  shell.h
OBJ = shell.o 

%.o: %.c $(DEPS)
	$(CC) -g -c -Wall -o $@ $< $(CFLAGS)

shell: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:	
	rm -f shell *.o
