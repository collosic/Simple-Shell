/*  Name: Christian Collosi
 *  ID: 11233529
 *  ICS 53
 *  NOTE: Please use the make file to compile or compile with node.h header file
 */

// Include any additional header files here
#include "node.h"

int main(void) {
    char buf[LEN];
    char *result;

    // Initialize the head to NULL
    head = NULL;
    
    // extract the command entered into the console
    while(true) {
        printf("%s", "> ");
        fflush(stdout);
        result = fgets(buf, LEN, stdin);
        if((NULL == result) || ('\n' == *result)) {
            printf("%s\n", "nothing was entered.");
            continue;
        }
        // inject a null termination at the end of buf
        buf[strlen(buf) - 1] = '\0';

        // the string now needs to be parsed using whitespace as a delimiter
        char *command = NULL;
        char *argument = NULL;
        command = strtok(buf, SPACE);
       
        // lets verify the command isn't empty
        if (command != NULL) {
            argument = strtok(NULL, SPACE);
        }

        // send the command for further processing 
        runCommand(command, argument);
    }
    return 0;
}

// Initialze the head of the Link List
void initHead() {
    head = malloc(sizeof(LNode));
    if (head == NULL) {
        // something went wrong
        puts("System was unable to allocate memory, existing");
        exit(EXIT_SUCCESS);
    }
    head->next = NULL;
}


// Function used to run the command
void runCommand(char *command, char *argument) {
    int type = matchType(command);
     switch (type) {
        case READ:      readRecords(argument);
                        break;
        case WRITE:     writeRecords(argument);
                        break;
        case PRINT:     printRecords();
                        break;
        case DELETE:    deleteRecord(argument);
                        break;
        case QUIT:      quit(); 
                        break;
        default:        puts("Invalid command entered.");
                        break;
     }
}


// Will attempt to match the command entered to a valid console command
int matchType(char *command) {
    if (strcmp(command, "read") == 0) {
        return READ;
    } else if (strcmp(command, "write") == 0) {
       return WRITE;
    } else if (strcmp(command, "print") == 0) {
        return PRINT;
    } else if (strcmp(command, "delete") == 0) {
        return DELETE;
    } else if (strcmp(command, "quit") == 0) {
        return QUIT;
    } else {
        return -1;
    }
}


// Reads in the contents of a file into memory
void readRecords(char *file_name) {
    if (file_name == NULL) {
        printf("%s\n", "No file name given.");
    } else {
        FILE *fptr = fopen(file_name, "r");
        if (NULL == fptr) {
            printf("%s\n", "Invalid file name.");
        } else {
            // delete the entire contents from memory
            deleteMemory();
            
            // initialize the memory
            initHead();

            // read in all the records and store them into memory
            storeIntoMemory(fptr);

            // close file
            fclose(fptr);
        }
    }
}

// This function will delete all contents in memory
void deleteMemory() {
    LNode *current = head;
    LNode *next = (current != NULL) ? current->next : NULL;
    while (current != NULL) {
        free(current);
        current = next;
        next = (current != NULL) ? current->next : NULL;
    }
    head = NULL;
}

// This function will copy records from file to memory
int storeIntoMemory(FILE *fptr) {
    // extract each line, and place into memory
    LNode *current = head;
    char buf[LEN];
    char *result = NULL;
    result = fgets(buf, LEN, fptr);

    // check for an empty file
    if (result == NULL) {
        deleteMemory();
        return -1; 
    }
    buf[strlen(buf) - 1] = '\0';

    // we can test to make sure we got TABS instead of SPACES
    char *test = strchr(buf, '\t');
    if (test == NULL) {
        // there's no TAB character
        deleteMemory();
        puts("The file being read does not contain the TAB character");
        return -1;
    }
    while (result != NULL) {
        // extract name, address and phone number
        strcpy(current->name, strtok(buf, TAB));
        strcpy(current->address, strtok(NULL, TAB));
        strcpy(current->phone, strtok(NULL, TAB));
        
        // get the next line and create a new node
        result = fgets(buf, LEN, fptr);
        buf[strlen(buf) - 1] = '\0';
        if (result != NULL) {
            current->next = malloc(sizeof(LNode));
            current = current->next;
            current->next = NULL;
        }
    } // end of while loop
    return 0;
} // end of storeIntomemory function


// This function will print all the contents in memory
void printRecords() {
    LNode *current = head;
    int count = 1;
    while (current != NULL) {
        char *n = current->name;
        char *a = current->address;
        char *p = current->phone;
        printf("%d %s\t%s\t%s\n", count++, n, a, p);
        
        // move to the node
        current = current->next;
    }
}


// Function to write the contents of memory to a file
void writeRecords(char *file_name) {
    // first lets verify that file name is valid
    if (NULL == file_name) {
        puts("No file name entered");
    } else {
        FILE *fptr = fopen(file_name, "w");
        if (NULL == fptr) {
            puts("Invalid file name");
        } else {
            // go through all the records and write to disk
            writeToFile(fptr);
            fclose(fptr);     
        }
    }
}


// Function that write to a file
void writeToFile(FILE *fptr) {
    LNode *current = head;
    LNode *next = (current != NULL) ? current->next : NULL;
    while (current != NULL) {
        char *n = current->name;
        char *a = current->address;
        char *p = current->phone;
        fprintf(fptr, "%s\t%s\t%s\n", n, a, p);
        
        // move to the next node
        current = next;
        next = (current != NULL) ? current->next : NULL;
    }
}


// function to delete a record from memory 
int deleteRecord(char *rec_num) {
    // first we must check and see if the argument passed is an integer
    if (rec_num == NULL || !(isInteger(rec_num))) {
        puts("Argument supplied is not an valid integer");
        return -1;
    } 
    int num = atoi(rec_num);
    int counter = 1;
    LNode *prev = NULL;
    LNode *current = head;
    LNode *next = (current != NULL) ? current->next : NULL;

    // cycle through the link list until we find the correct location
    while (current != NULL) {
        if (num == counter++) {
            if (current == head) {
                head = next;
                free(current);
            } else {
                prev->next = next;
                free(current);     
            }
            return 0; 
        }
        // move along in the list
        prev = current;
        current = next;
        next = (current != NULL) ? current->next : NULL;
    } // end of while loop 
    puts("Record was not found");
    return -1;
}

// function to quit the program
void quit() {
    // free all the dynamically allocted nodes and exit
    deleteMemory();
    exit(EXIT_SUCCESS);
}


// function that checks whether a string is all digits
int isInteger(char *arg) {
    while(*arg) {
        if (isdigit(*arg++) == 0) {
            return 0;
        }
    }
    return 1;
}
