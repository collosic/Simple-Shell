#!/bin/bash
# quick shell script to compile our mini programs

gcc -o ../foo7 foo7.c
gcc -o ../foo15 foo15.c
gcc -o ../hello hello.c
gcc -std=c99 -o ../record record.c; cp test.rec ../
