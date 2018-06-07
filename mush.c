#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>

#include "mush.h"
/* check line 88 */

void int_handler(int signum)
{
    printf("\n"); fflush(stdout);
}

int main(int argc, char *argv[])
{

    printf("8-P ");
    fflush(stdout);
    if (pline() == 2) { return 1; }
    while (1)
    {
        printf("8-P ");
        fflush(stdout);
        if (pline() == 2) { printf("^D\n"); fflush(stdout); return 1;}
    }
    return 0;
}

int pline()
{
    char init[512];
    char buff[512];
    int r = 0;
    int c = 0;
    int com = 0;
    int once = 0;
    char *tok = NULL;
    char temp[512];
    char second[512];
    int final = 0;
    int count = 0; int pipe = 0;
    struct sigaction handler;
    handler.sa_handler = &int_handler;
    sigemptyset(&handler.sa_mask);
    sigaddset(&handler.sa_mask, SIGINT);
    if (sigaction(SIGINT, &handler, NULL) == -1) { perror("signal"); exit(1); }
    memset(init, '\0', 512);
    memset(buff, '\0', 512);
    memset(temp, '\0', 512); memset(second, '\0', 512);
    if ((r = read(0, init, 512)) == 0) { return 2;}
    if (strlen(init) > MAX_LENGTH) 
        { fprintf(stderr, "usage: too long"); exit(1);}
    strcpy(buff, init);
    if ((c = argcount(buff)) == 0) { return 0; }
    if (count_pipes(buff) >= 10) { 
        fprintf(stderr, "Pipeline too deep.\n");
        return 0;
    }
    else if (c >= 10) { fprintf(stderr, "Too many arguments.\n"); return 0; }
    com = count_ops(count_left(buff), count_right(buff), count_pipes(buff),
        buff); 
    if (com > MAX_COM) { fprintf(stderr, "usage: too many commands"); }
    if (com < 0) { return 0; }
    if (count_pipes(buff) == 0)
    {
        stage(buff, count_pipes(buff));
    }
    else if (count_pipes(buff) == 1) {
        memset(temp, '\0', 512);
        strcat(buff, " ");
        tok = strtok(buff, " \0");
        while(tok) {
            if (strcmp(tok, "|") == 0)
            {
                if (pipe == 0) { pipe = 1; }
                else {
                    printf("temp: %s\nsecond: %s\n", temp, second);
                    pipeit(temp, second, 0);
                    memset(temp, '\0', 512); strcpy(temp, second); 
                    memset(second, '\0', 512);
                    once = 1;
                    final = 1;
                }
            }
            else { 
                if (pipe == 0)
                { strcat(temp, tok); strcat(temp, " "); count++; }
                else { strcat(second, tok); strcat(second, " "); count++; }
            }
            if (strcmp(tok, "cd") == 0) { mycd(buff); }
            tok = strtok(NULL, " \0");
            if (once == 1 && tok && final != 1) 
            { strcat(second, tok); strcat(second, " "); }
        }
        if (once == 0 || final == 1) { pipeit(temp, second, 0); }
    else { three(buff); }
    }
    return 0;
}

void pipeit(char *temp, char *second, int on)
{
    int fd[2];  
    pid_t pid; pid_t pid2;
    char buff[512];
    char mytp[512];
    char first[512];
    char reader[512];
    char *tokn = NULL;
    char *tok = NULL;
    char fin[512];
    char *argv[512];
    char *argv2[512];
    int i = 0;
    int j = 0;

    memset(fd, 0, 2); memset(buff, '\0', 512);
    strcpy(buff, second); memset(reader, '\0', 512); 
    memset(argv, '\0', sizeof(argv[10][512]) * 10 * 512);
    memset(argv2, '\0', sizeof(argv2[10][512]) * 10 * 512);
    memset(first, '\0', 512); memset(fin, '\0', 512);
    memset(mytp, '\0', 512); strcat(mytp, temp);
    if (pipe(fd) < 0) { perror("pipe"); exit(1); }
    if ((pid = fork()) < 0) { perror("fork"); exit(1); } 
    else if (pid == 0)
    {
        if (on == 0) 
        {   dup2(fd[1], 1); 
            tokn = strtok(mytp, " \n");
            strcat(first, "/bin/"); strcat(first, tokn);
            while (tokn)
            {
                argv[j] = tokn;
                tokn = strtok(NULL, " \n");
                j++;
            }
        }
        close(fd[1]); close(fd[0]);
        if (execv(first, argv) == -1) { perror("execv"); exit(1); }
    }
    if ((pid2 = fork()) < 0) { perror("final fork"); exit(1); }
    else if (pid2 == 0)
    {
        dup2(fd[0], 0);
        strcat(fin, "/bin/"); 
        tok = strtok(buff, " \n");
        strcat(fin, tok);
        while (tok)
        {
            argv2[i] = tok;
            tok = strtok(NULL, " \n");
            i++;
        }
        close(fd[0]); close(fd[1]);
        if (execv(fin, argv2) == -1) { perror("final ex"); exit(1); } 
    }
    close(fd[0]); close(fd[1]);
    if (wait(NULL) == -1) { perror("wait"); }
    if (wait(NULL) == -1) { perror("wait"); }
}



void mycd(char *buff)
{
    char b[512];
    char *token = NULL;
    char argv[512];
    int i = 0;
    int on;
    DIR *dir;
    
    memset(b, '\0', 512);
    memset(argv, '\0', 512);
    strcpy(b, buff);
    
    token = strtok(b, " \n");
    while(token) 
    {
        if ((strcmp(token, "|") == 0) || (strcmp(token, "<") == 0) ||
            (strcmp(token, ">") == 0))
        { break; }
        if (on == 1)
        {
            strcat(argv, token);
            i++;
        }
        if (strcmp(token, "cd") == 0) { on = 1; }
        token = strtok(NULL, " \n");
    }
    if (!(dir = opendir(argv))) { perror(argv); }
    else { closedir(dir); }
    chdir(argv);
}




int argcount(char *line)
{
    char *token = NULL;
    int count = 0;
    char buff[512];
    strcpy(buff, line);

    token = strtok(buff, " \n");
    while(token != NULL)
    {
        if (strcmp(token, ">") == 0 || strcmp(token, "<") == 0)
        {
            break;
        }
        count++;
        token = strtok(NULL, " \n");
    }
    return count;
}

int count_ops(int left, int right, int pipes, char *buff)
{
    char line[512];
    char *token;
    int tru = 0;
    char lefty[512];
    char righty[512];


    strcpy(line, buff);
    strcpy(lefty, buff);
    strcpy(righty, buff);
    if ((left == pipes) && (right == 0) && (left != 0) && (pipes != 0) &&
        (strlen(strchr(lefty, '|')) > strlen(strchr(righty, '<'))))
    {
        fprintf(stderr, "%s: Ambiguous Input\n", strtok(buff, " "));
        return -1;
    }
    if ((right == pipes) && (left == 0) && (right != 0) && (pipes != 0) &&
        (strlen(strchr(lefty, '>')) > strlen(strchr(righty, '|'))))
    {
        fprintf(stderr, "%s: Ambiguous Output\n", strtok(buff, " "));
        return -1;
    }
    token = strtok(line, " \n");
    while (token != NULL)
    {
        if (strcmp(token, "|") == 0)
        {
            if (tru == 1)
            {
                fprintf(stderr, "usage:");
                exit(-1);
            }
            else { tru = 1; }
        }
        else { tru = 0; }
        token = strtok(NULL, " \n");
        
    } 
    return left + right + pipes;
}

int count_pipes(char *line)
{
    char *token;
    char buff[512];
    int count = 0;

    strcpy(buff, line);
    token = strtok(buff, " \n");
    while (token != NULL)
    {
        if (strcmp(token, "|") == 0) { count++; } 
        token = strtok(NULL, " \n");
    }
    return count;
}

int count_left(char *line)
{
    char *token;
    char buff[512];
    int count = 0;

    strcpy(buff, line);
    token = strtok(buff, " \n");
    while (token != NULL)
    {
        if (strcmp(token, "<") == 0) { count++; } 
        token = strtok(NULL, " \n");
    }
    return count;
}

int count_right(char *line)
{
    char *token;
    char buff[512];
    int count = 0;

    strcpy(buff, line);
    token = strtok(buff, " \n");
    while (token != NULL)
    {
        if (strcmp(token, ">") == 0) { count++; } 
        token = strtok(NULL, " \n");
    }
    return count;
}



void stage(char *line, int stage)
{
    char cder[512];
    char buff[512];
    char fin[512];
    char *token = NULL;
    pid_t pid;
    pid_t pid1;
    int status;
    char *argv[512];
    int i = 0;
    int into = 0;
    int outof = 0;
    char unto[512];
    int fd = 0;
    int fd2 = 0;
    char last[512];
    char olats[512];

    memset(buff, '\0', 512); memset(fin, '\0', 512);
    memset(argv, '\0', sizeof(argv[10][512]) * 10 * 512);
    memset(olats, '\0', 512); memset(cder, '\0', 512); memset(last, '\0', 512);
    strcpy(buff, line);
    strcpy(cder, line);
    token = strtok(buff, " \n");
    strcat(fin, "/bin/");
    strcat(fin, token);
    while (token)
    { 
        if (into == 1) { memset(unto, '\0', 512); strcat(unto, token); break; } 
        if (outof == 1)
        {
            if ((fd2 = open(token, O_CREAT | O_RDWR, 0644)) < 0)
            { perror("open 1"); exit(1); }
            if ((pid1 = fork()) == -1) { perror("fork"); exit(1); }
            else if (pid1 == 0) 
            {
                dup2(fd2, 0);
                if (count_right(buff) == 0){
                if (execl(fin, token, NULL) == -1) 
                    { perror("out ex"); exit(1); }}
            }
            wait(&status);
            close(fd2);
            outof = 0;
        }
        if (strcmp(token, "cd") == 0) { mycd(cder); }
        if (strcmp(token, ">") == 0) { into = 1;  
            token = strtok(NULL, " \n"); continue; }
        if (strcmp(token, "<") == 0) { outof = 1; 
            token = strtok(NULL, " \n"); continue; }
        argv[i] = token;
        memset(last, '\0', 512);
        strcat(last, token);
        token = strtok(NULL, " \n");
        i++;
    }
    if ((pid = fork()) < 0) { perror("fork"); exit(1); }
    if (pid == 0)
    {
        if (into == 1) {
            if ((fd = open(unto, O_CREAT | O_RDWR | O_TRUNC, 
            0644)) < 0)
            {
                perror("open"); exit(1);
            } 
            dup2(fd, 1);
        }
        if (execv(fin, argv) == -1)
        {
            perror(last);
            exit(1);
        }
        close(fd);
    }
    wait(&status);
}

void three(char *buff)
{
    char line[512];
    int fd1[2], fd2[2]; 
    pid_t pid, pid2, pid3;
    char *token = NULL; 
    char first[512]; char fr[512];
    char second[512]; char sec[512];
    char third[512]; char thr[512];
    char uno[512]; char dos[512]; char tres[512];
    char *argv[512]; char *argv2[512]; char *argv3[512];
    int i = 0;
    char b1[512]; char b2[512]; char b3[512];
    
    memset(line, '\0', 512); memset(third, '\0', 512);
    memset(first, '\0', 512); memset(second, '\0', 512); 
    strcat(line, buff);
    if (pipe(fd1) < 0 || pipe(fd2) < 0) 
    { perror("pipe"); exit(1); }
    token = strtok(line, " |");
    for (i = 0; i < 3; i++)
    {
        if (i == 0) { strcat(first, token); }
        if (i == 1) { strcat(second, token); }
        if (i == 2) { strcat(third, token); }
        token = strtok(NULL, " |");
    }
    if ((pid = fork()) == -1) { perror("fork"); exit(1); }
    else if (pid == 0)
    {
        strcat(uno, first); argv[0] = first;
        memset(fr, '\0', 512); dup2(fd1[1], 1);
        strcat(fr, strtok(first, " ")); 
        strcat(b1, "/bin/"); strcat(b1, fr);
        close(fd1[0]); close(fd1[1]);
        close(fd2[0]); close(fd2[1]);
        if (execv(b1, argv) == -1)
        {
            perror("first"); exit(1);
        }
    }
    if ((pid2 = fork()) == -1) { perror("fork"); exit(1); }
    else if (pid2 == 0)
    {
        memset(sec, '\0', 512); dup2(fd2[1], 1); dup2(fd1[0], 0);
        strcat(dos, second); argv2[0] = dos;
        strcat(sec, strtok(second, " ")); 
        strcat(b2, "/bin/"); strcat(b2, sec);
        close(fd1[0]); close(fd1[1]);
        close(fd2[0]); close(fd2[1]);
        if (execv(b2, argv2) == -1)
        {
            perror("second"); exit(1);
        }
    }
    if ((pid3 = fork()) == -1) { perror("fork"); exit(1); }
    else if (pid3 == 0)
    {
        memset(thr, '\0', 512); dup2(fd2[0], 0);
        strcat(tres, third); argv3[0] = tres;
        strcat(thr, strtok(third, " ")); 
        strcat(b3, "/bin/"); strcat(b3, thr);
        close(fd1[0]); close(fd1[1]);
        close(fd2[0]); close(fd2[1]);
        if (execv(b3, argv3) == -1)
        {
            perror("third"); exit(1);
        }
    }
    close(fd1[0]); close(fd1[1]); close(fd2[0]); close(fd2[1]);
    wait(NULL); wait(NULL); wait(NULL); 
}








