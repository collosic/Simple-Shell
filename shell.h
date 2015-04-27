#ifndef SHELL
#define SHELL value

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Misc constants */
#define MAXLINE     8192
#define MAXARGS     128
#define MAXLINES    25 // screen display with fixed # of lines (25)
#define MAXPROCESS  10

/* Type Definitions */
typedef void handler_t(int);

struct process {
	pid_t pid;
	char cond;
};

/* External variables */
extern int h_errno;    /* Defined by BIND for DNS errors */ 
extern char **environ; /* Defined by libc */

/* Global Variables */
struct process all_proc[MAXPROCESS];

/* function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 
void create_proc(struct process *all_proc);
int addprocess(struct process *all_proc, pid_t pid, char cond);
int deleteprocess(struct process *all_proc, pid_t pid);
void show_all(struct process *all_proc);

/* SIG Handlers */
void int_handler(int sig);
void tstp_handler(int sig);
void child_handler(int sig);
handler_t *Signal(int signum, handler_t *handler);// wrapper for handlers

/* Our own error-handling functions */
void unix_error(char *msg);
void app_error(char *msg);

/* Process control wrappers */
pid_t Fork(void);
char *Fgets(char *ptr, int n, FILE *stream);

#endif
