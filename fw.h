typedef struct Node
{
    char *word;      
    int count;   
    struct Node *left;
    struct Node *right;
} Node;     

struct Node *addTree(struct Node *, char *);
char *strdup(char *);
int isInt(char []);
struct Node *fileTree(struct Node *, FILE *);
void treeprint(struct Node *);
char *read_long_word(FILE *file);
int sumTree(struct Node *);
struct Node *highestCount(struct Node *, struct Node*);
void freeTree(struct Node *);
struct Node *sInTree(struct Node *);
char *strlower(char *);
int isWord(char []);
