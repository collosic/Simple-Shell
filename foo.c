#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) 
{
    int i, secs;

    if (argc != 2) {
	exit(0);
    }
    secs = atoi(argv[1]);
    for (i=0; i < secs; i++)
	sleep(1);
    exit(0);
}
