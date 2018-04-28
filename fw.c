#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "fw.h"
#define HASHSIZE 1000000;

/* HEY DUMMY, NEXT TIME YOU WORK ON THIS, IMPLEMENT YOUR BINARY SEARCH
 * AND THEN MAKE SURE SUM ONLY KEEPS TRACK OF UNIQUE WORDS*/

int main(int argc, char *argv[])
{
    int finalC = 0;
    int i = 0;
    int i2 = 0;
    int j = 0;
    int sum = 0;
    FILE *file;
    struct Node *tree = NULL;
    struct Node *node = NULL;
    struct Node *high = (struct Node *)malloc(sizeof(struct Node));
    
    if (argc > 1) 
    {
        if (strcmp(argv[1], "-n") == 0)
        {
            if (argc == 2)
            {
                fprintf(stderr, "usage: fw [-n num] [ file1 [ file 2 ..] ]\n");
                return 1;
            }
            if ((finalC = atoi((argv[2]))))
            {
                i = 3;
            }
            else
            {
                fprintf(stderr, "usage: fw [-n num] [ file1 [ file 2 ..] ]\n");
                return 1;
            }
        }
        else
        {
            finalC = 10;
            i = 1;
        }
        for (i2 = i; i2 < argc; i2++)
        {
            if ((file = fopen(argv[i2], "r")))
            {
                tree = fileTree(tree, file);
                sum = sumTree(tree);
            }
            else
            {
                printf("/home/pn-cs357/lib/asgn2//Tests/Data/nosuchfile: No such file or directory\n");
            }
        }
    }
    else
    {
        tree = sInTree(tree);
        sum += sumTree(tree);
        finalC = 10;
    }
    printf("The top %d words (out of %d) are:\n", finalC, sum);
    if (sum != 0)
    {
        if (sum >= finalC)
        {
            for (j = 0; j < finalC; j++)
            {
                node = highestCount(tree, high);
                printf("%9d %s\n", node -> count, node -> word);
                node -> count = 0;
                high = tree;
            }
        }
        else
        {
            for (j = 0; j < sum; j++)
            {
                node = highestCount(tree, high);
                printf("%9d %s\n", node -> count, node -> word);
                node -> count = 0;
                high = tree;
            }

        }
    }
    freeTree(tree);
    return 0;
}

void freeTree(struct Node *tree)
{
    if (tree == NULL)
    {
        return;
    }
    freeTree(tree -> right);
    freeTree(tree -> left);
    free(tree -> word);
    free(tree);
}

struct Node *addTree(struct Node *p, char *w)
{
    if (p == NULL) 
    { 
        p = (struct Node *)malloc(sizeof(Node)); 
        p -> word = strdup(w);
        p -> count = 1;
        p -> left = p -> right =NULL;
    }   
    else if (strcmp(w, p->word) == 0)
    {
        p -> count++; 
    }
    else if (strcmp(w, p->word) < 0) 
    {
        p -> left = addTree(p -> left, w);
    }
    else 
    {
        p -> right = addTree(p -> right, w); 
    }
    return p;
}
/* from the c programming language*/

char *strdup(char *s)
{
    char *p = NULL;
    p = (char *)malloc(strlen(s) + 1);
    if (p != NULL)
    {
        strcpy(p, strlower(s));
    }
    return p;
}

struct Node *highestCount(struct Node *tree, struct Node *high)
{
    struct Node *lmost;
    struct Node *rmost;

    if (tree == NULL)
    {
        return high;
    }
    if ((tree -> count > high -> count) ||
            ((tree -> count == high -> count) && 
            strcmp(tree -> word, high -> word) > 0))  
    {   
        high = tree;
    }
    lmost = highestCount(tree -> left, high);
    rmost = highestCount(tree -> right, high); 
    if ((lmost -> count > high -> count) ||
            ((lmost -> count == high -> count) && 
            strcmp(lmost -> word, high -> word) > 0))
    {
        high = lmost;
    }
    if ((rmost -> count > high -> count) ||
            ((rmost -> count == high -> count) && 
            strcmp(rmost -> word, high -> word) > 0))
    {
        high = rmost;
    }
    return high;
}

/* from the c programming language*/

int isInt(char str[])
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (isdigit(str[i]))
        {
            return 1;
        }
        i++;
    }
    return 0;
}

int sumTree(struct Node *tree)
{
    int sum = 0;
    if (tree != NULL && (isWord(tree -> word)))
    {
        sum = 1;  
        sum += sumTree(tree -> left); 
        sum += sumTree(tree -> right);
    }
    return sum;
}

char *strlower(char *word)
{
    int i = 0;

    for (i = 0; word[i]; i++)
    {
        word[i] = tolower(word[i]);
    } 
    return word;
}


char *read_long_line(FILE *file)
{
    char *line = NULL;
    int i = 0;
    char c = NULL;
    int size = 1;
    
    line = (char *)malloc(sizeof(char));
    
    if (file == NULL)
    {
        c = getchar();
        while((c != EOF))
        {   
            if (sizeof(line) + sizeof(c) < strlen(line))
            {
                line = realloc(line, strlen(line) + 1);
            }
            if (isalpha((unsigned char)c))
            {
                line[i] = (unsigned char)tolower(c);
                i++;
            }
            else
            {
                line[i] = 32;
                i++;
            }
            c = getchar();
        }
    }
    else
    {
        while ((c = getc(file)) != '\n')
        {
            if (c == EOF)
            {
                return NULL;
            }
            size++;
            line = realloc(line, size);
            if (isalpha((unsigned char) c))
            {
                line[i] = (unsigned char)tolower(c);
                i++;
            }
            else
            {
                line[i] = 32;
                i++;
            }
        }
    }
    size++;
    line = realloc(line, i + sizeof(c));
    line[i] = 32;
    return line;
}

struct Node *fileTree(struct Node *tree, FILE *file)
{
    char *token = NULL;
    char *line = NULL;

    while ((line = read_long_line(file)))
    {
        token = strtok(line, " 0123456789'\?\a?`/=|_\'\"\r\v\f+<>@#$%&*^;:(){}[]-,.\\!\t\b\n");
        while(token != NULL)
        {
            if (isWord(token))
            {
                tree = addTree(tree, token);
            }
                token = strtok(NULL, " 0123456789'\?\a?`/=|_\'\"\r\v\f+<>@#$%&*^;:(){}[]-,.\\!\t\b\n");
        }
    }
    if (line != NULL)
    { 
        free(line);
    }
    fclose(file);
    return tree;
}

struct Node *sInTree(struct Node *tree)
{
    char *token = NULL;
    char *line = NULL;

    line = read_long_line(NULL);
    token = strtok(line, " -,.!\0\n");
    while (token != NULL)
    {
        tree = addTree(tree, strlower(token));
        token = strtok(NULL, " -,.!\0\n");
    }
    return tree;
}


void treeprint(struct Node *tree)
{
    if (tree != NULL) 
    {
        treeprint(tree -> left);
        printf("%d, %d %s\n", isWord(tree -> word), tree -> count, tree->word); 
        treeprint(tree->right);
    } 
}


int isWord(char str[])
{
    int i = 0;
    while (isalpha(str[i]))
    {
        i++;
    } 
    if (str[i] == '\0')
    {
        return 1;
    }
    return 0;

}



