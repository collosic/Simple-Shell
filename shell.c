/* Group Project for ICS 53
 * Project Name:    Simple Shell
 *
 * Instructor:      Ian Harris
 *
 * Student Names:   Scott Shin
 *                  Christin Collosi
 *                  Anna Alexander Lambrix
 *                  Cody Bryce Sauer
 
 */

// Include header files here
#include "shell.h"

int main()
{
    char cmdline[MAXLINE]; /* Command line */
    create_proc(all_proc);
    
    //Signal handlers with wrapper Signal  installed
    Signal(SIGINT, int_handler); // for kbd interrupt (for foreground?)
    //Signal(SIGTSTP, tstp_handler); // for terminal z (for foreground?)
    Signal(SIGCHLD, child_handler); // when child is terminated 
    
    while (1) {
        /* Read */
        printf("shell53> ");
        Fgets(cmdline, MAXLINE, stdin);
        if (feof(stdin)) {
            exit(0);
        }
        /* Evaluate */
        eval(cmdline);
        fflush(stdout); // A: I added that crap bc the shell was acting weird after FG commands... 
    }
    exit(0);
}
/* $end shellmain */


/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline)
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
    //A: this is from textbok for parent-child synchronization - signal set
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    //A.
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
        return;   /* Ignore empty lines */
    
    if (!builtin_command(argv)) {
        sigprocmask(SIG_BLOCK, &mask, 0);
        //A.
    
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            // A: added this one bc bg was killed when fg was running and ctrl-c was pressed
            setpgid(0,0); 
            sigprocmask(SIG_UNBLOCK, &mask, 0); // unblock sigchld signals
            
            // check and see if we are attempting to call shell itself and return an error
            if (!strcmp(argv[0], "shell") || execv(argv[0], argv) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }
        
        if(pid < 0) {
            printf("something wrong with fork\n");
            return;
        }
        
        /* Parent waits for foreground job to terminate */
        if (!bg) {
        printf("adding to fg\n");
            addprocess(all_proc, pid, 'F'); // A: this is a foreground process
            int status;
            if (waitpid(pid, &status, 0) < 0)
                unix_error("waitfg: waitpid error");
            deleteprocess(all_proc, pid);
        }
        else {
        printf("adding to bg\n");
            addprocess(all_proc, pid, 'B'); // A: this is a background process
        }
            
        //A:
        sigprocmask(SIG_UNBLOCK, &mask, 0); // unblock sigchld signals
        //A.
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv)
{
    if (!strcmp(argv[0], "quit")) { /* quit command */
        //A: killing bg processes before quit
        int i;
        for(i =0; i<MAXPROCESS; i++){
            if(all_proc[i].cond == 'B'){
                printf("before quit killing bg process # %d \n", all_proc[i].pid);
                pid_t pid = all_proc[i].pid;
                kill(pid, SIGKILL);
                all_proc[i].cond = 'N';
                all_proc[i].pid = 0;
            }
        }
        exit(0);
    }
    //A: just a bookkeeping built in to see what the heck we have in our struct array...
    if (!strcmp(argv[0], "all")) {
        show_all(all_proc);
        return 1; 
    }
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
        return 1;
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv)
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */
    
    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
        buf++;
    
    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
        return 1;
    
    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
        argv[--argc] = NULL;
    
    return bg;
}
/* $end parseline */

/* $begin forkwrapper */
pid_t Fork(void) 
{
    pid_t pid;
    if ((pid = fork()) < 0)
        unix_error("Fork error");
    return pid;
}
/* $end forkwrapper */


char *Fgets(char *ptr, int n, FILE *stream) 
{
    char *rptr;
    if (((rptr = fgets(ptr, n, stream)) == NULL) && ferror(stream))
        app_error("Fgets error");
    return rptr;
}
/* $begin wait */


pid_t Wait(int *status) 
{
    pid_t pid;
    if ((pid  = wait(status)) < 0)
        unix_error("Wait error");
    return pid;
}
/* $end wait */

pid_t Waitpid(pid_t pid, int *iptr, int options) 
{
    pid_t retpid;
    if ((retpid  = waitpid(pid, iptr, options)) < 0) 
        unix_error("Waitpid error");
    return(retpid);
}

/* $begin unixerror */
void unix_error(char *msg) /* Unix-style error */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}
/* $end unixerror */


void app_error(char *msg) /* Application error */
{
    fprintf(stderr, "%s\n", msg);
    exit(0);
}
/* $end errorfuns */

/* start signal handler fx's*/
// this is SIGINT
void int_handler(int sig){
    pid_t pid;
    int i;
    for(i = 0; i<MAXPROCESS; i++) {
        if(all_proc[i].cond== 'F'){
            pid = all_proc[i].pid;
            if(pid != 0) {
                all_proc[i].cond = 'N';
                all_proc[i].pid = 0;
            }
        }
    }
    if(pid != 0)
        kill(pid, SIGINT);
    return;
}


void child_handler(int sig){
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        printf("Process # %d is reaped\n", (int)pid);
        //A: this is status exit check from textbook... section 8.4
        if(WIFEXITED(status) | WIFSIGNALED(status)) deleteprocess(all_proc, pid);
        if(WIFSTOPPED(status)) printf("we gotta do smth with stopped process?\n");
    }

    if(pid < 0 && errno != ECHILD)
        unix_error("waitpid err");
    return;
}

handler_t *Signal(int signum, handler_t *handler){
    struct sigaction action, old_action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    
    if(sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal Error");
    return(old_action.sa_handler);
}
        
int addprocess(struct process *all_proc, pid_t pid, char cond){
    if(pid < 1) return 0; // not adding nothing
    int i;
    //we have a fg process record and just need to change it to another pid
    if(cond == 'F' && has_fgprocess(all_proc) != 0){
        for(i = 0; i<MAXPROCESS; i++){ 
            if(all_proc[i].cond == 'F') { // adding something
                all_proc[i].pid = pid;
                return 1;
            }
        } // end of for loop
    } else {
        for(i = 0; i<MAXPROCESS; i++){ 
            if(all_proc[i].pid == 0) { // adding something
                all_proc[i].pid = pid;
                all_proc[i].cond = cond;
                return 1;
            }
        } // end of for loop
    }
    return 0; // not adding, some prob
}

int deleteprocess(struct process *all_proc, pid_t pid) {
    if(pid < 1) return 0;
    int i;
    for(i = 0; i<MAXPROCESS; i++) {
        if(all_proc[i].pid == pid) {
            all_proc[i].pid = 0;
            all_proc[i].cond = 'N';
            return 1;
        }
    }
    return 0;
}


//creating empty array of structs with 'N' aka None condition
void create_proc(struct process *all_proc) {
    int i;
    for(i = 0; i<MAXPROCESS; i++) {
        all_proc[i].pid = 0;
        all_proc[i].cond = 'N';
    }
}

//just a bookkeeping fx to see all we have 
void show_all(struct process *all_proc){
    int i;
    for(i =0; i<MAXPROCESS; i++){
        //if(all_proc[i].pid != 0){
        printf("we got process  %d  - %c\n", all_proc[i].pid, all_proc[i].cond);
        //}
    }
}

pid_t has_fgprocess(struct process *all_proc) {
    int i;
    pid_t pid;
    for(i = 0; i<MAXPROCESS; i++) {
        if(all_proc[i].cond == 'F') {
            pid = all_proc[i].pid;
            return pid;
        }
    }
    return 0;
}
