#define MAX_LENGTH 512
#define MAX_COM 10
#define MAX_ARG 10


int argcount(char *);

void stage(char *, int);

int count_pipes(char *);

int count_left(char *);

int count_right(char *);

int count_ops(int, int, int, char*);

int pline();

void setmem(char *argv[512]);

void pipeit(char *, char *, int);

void mycd(char *);

void int_handler(int);

void three(char *);

void delfirst(char *);
