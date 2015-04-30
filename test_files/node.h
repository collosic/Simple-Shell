#ifndef NODE_H
#define NODE_H 

// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// Program Constants
#define LEN 256
#define READ 1
#define WRITE 2
#define PRINT 3
#define DELETE 4
#define QUIT 5 

// Constant Ascii characters 
const char SPACE[2] = " ";
const char TAB[2] = "\t";

// Function Definitions
void readRecords(char *file_name);
void writeRecords(char *file_name);
void printRecords();
void quit();
int deleteRecord(char *rec_num);

// Helper Functions
void runCommand(char *command, char *argument); 
void initHead();
void deleteMemory();
void writeToFile(FILE *fptr);
int matchType(char *command);
int storeIntoMemory(FILE *fptr);
int isInteger(char *arg);

// Struct Definition
typedef struct node {
    char name[LEN];
    char address[LEN];
    char phone[LEN];
    struct node *next;
} LNode;

// Head of the Link List
LNode *head;

#endif
