#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <grp.h>
#include <inttypes.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include <tar.h>
#include <arpa/inet.h>

#include "mytar.h"


/*CHECK CHECKSUM() DUMMY*/


int main(int argc, char *argv[])
{
    int count = 0;
    int fd;
    int j = 0;
    int i = 0;
    char buff[1024] = { '\0' };

    if (argc < 3) { fprintf(stderr, "usage"); exit(1); }
    count = argc - 3;
    if (!(is_c(argv[1]) || is_t(argv[1]) || is_x(argv[1])))
    {
        fprintf(stderr, "usage");
        exit(-1);
    }
    if (is_c(argv[1]))
    {
        if ((fd = open(argv[2], O_RDWR | O_APPEND | O_CREAT, 0644)) == -1)
        {
            fprintf(stderr, "Usage: open");
            exit(-1);
        }
        for (j = 0; j < count; j++)
        {
            create((argv + 3)[j], fd);
        }
        if(write(fd, buff, 1024) == -1)
        {
            perror("write here");
            exit(-1);
        }
        if ((close(fd)) == -1)
        {
            fprintf(stderr, "close here");
            exit(-1);
        }
    }
    if (is_t(argv[1]))
    {
        if (is_v(argv[1]))
        {
            if (argc >= 4){ 
                for (i = argc - 1; i > 2; i--)
                {
                    basic_t(argv[2], 1, argv[i], 1); 
                }
            }
            else { basic_t(argv[2], 1, "hi", 0); }
        }
        else
        {
            if (argc >= 4) { 
               for (i = argc - 1; i > 2; i--) 
                {
                    basic_t(argv[2], 0, argv[i], 1); 
                }
            }
            else { basic_t(argv[2], 0, "hi", 0); }
        }
    }    
    if (is_x(argv[1]))
    {
        extract(argv[2], argv + 3, argc - 3);
    }
    return 0;
}

int is_c(char *set)
{
    int i = 0;

    for (i = 0; i < strlen(set); i++)
    {
        if (set[i] == 'c')
        {
            return 1;
        }
    }
    return 0;
}



void basic_t(char *file, int v, char *named, int n)
{
    struct header hp;
    int fd = 0;
    char name[100];
    char uname[9];
    char group[9];
    char time[16];
    char prefix[135];
    long size;
    time_t mtime;
    mode_t hmode;
    char *perm;
    int first = 0;
    int good = 0;

    header(file, &hp);
    if ((fd = open(file, O_RDONLY)) == -1)
    {
        perror("open");
        exit(-1);
    }
    while (read(fd, &hp, 512) > 0)
    {
        memset(prefix, 0, 135);
        size = strtol(hp.size, NULL, 8);
        if (checksum(&hp) == strtol(hp.chksum, NULL, 8))
        {
            good = 1;
            if (n == 1)
            {
                if (strlen(hp.prefix))
                {
                    strcat(prefix, hp.prefix); 
                    strcat(prefix, "/");
                    strcat(prefix, hp.name);
                    if (strcmp(named, prefix) != 0) { continue; }
                }
                else
                {
                    if (strcmp(named, hp.name) != 0 && (!same(named, hp.name))) 
                    { continue; }}
            }
            if (v == 1) {
                perm = (char *)malloc(11);
                strcpy(uname, hp.uname);
                strcpy(group, hp.gname);
                mtime = (time_t)strtol(hp.mtime, NULL, 8);
                hmode = (mode_t)strtol(hp.mode, NULL, 8);
                perm = permissions(perm, hmode, *(hp.typeflag));
                printf("%10s ", perm);
                printf("%s/%s ", uname, group); 
                printf("%8d ", (int)strtol(hp.size, NULL, 8));
                strftime(time, 17, "%Y-%m-%d %H:%M", localtime(&mtime));
                printf("%s ", time);
                fflush(stdout);
                free(perm);
            }
            snprintf(name, 101, "%s", hp.name); 
            if (strlen(hp.prefix)) {
                memset(prefix, 0, 135);
                strcat(prefix, hp.prefix);
                strcat(prefix, "/");
                strcat(prefix, name);
                printf("%s\n", prefix);
            }
            else {
                if (first == 0) { printf("%s\n", hp.name); first++; }
                else { printf("%s\n", name); } 
            }
            fflush(stdout);
        }
        else{
            lseek(fd, (512 * (size/512 + size % 512)), SEEK_CUR); 
        }
    }
    if (close(fd) == -1)
    {
        perror("close");
        exit(-1);
    }
    if (!good) { fprintf(stderr, "not good"); exit(1); }
}

int is_t(char *set)
{
    int i = 0;

    for (i = 0; i < strlen(set); i++)
    {
        if (set[i] == 't')
        {
            return 1;
        }
    }
    return 0;
}

int is_x(char *set)
{
    int i = 0;

    for (i = 0; i < strlen(set); i++)
    {
        if (set[i] == 'x')
        {
            return 1;
        }
    }
    return 0;
}
int is_v(char *set)
{
    int i = 0;

    for (i = 0; i < strlen(set); i++)
    {
        if (set[i] == 'v')
        {
            return 1;
        }
    }
    return 0;
}

int is_S(char *set)
{
    int i = 0;

    for (i = 0; i < strlen(set); i++)
    {
        if (set[i] == 'S')
        {
            return 1;
        }
    }
    return 0;
}

char *permissions(char *perm, mode_t type, char tf)
{
    int i = 0;

    for (i = 0; i < 10; i++)
    {
        perm[i] = '-';
    }
    if (tf == '0') { perm[0] = '-'; }
    else if (tf == '2') { perm[0] = 'l'; }
    else { perm[0] = 'd'; } 
    if ((type & S_IRUSR) == S_IRUSR)                                        
    {                                                                       
        perm[1] = 'r';                                                      
    }                                                                       
    if ((type & S_IWUSR) == S_IWUSR)                                        
    {                                                                       
        perm[2] = 'w';                                                      
    }                                                                       
    if ((type & S_IXUSR) == S_IXUSR)                                        
    {                                                                       
        perm[3] = 'x';                                                      
    }                                                                       
    if ((type & S_IRGRP) == S_IRGRP)                                        
    {                                                                       
        perm[4] = 'r';                                                      
    }                                                                       
    if ((type & S_IWGRP) == S_IWGRP)                                        
    {                                                                       
        perm[5] = 'w';                                                      
    }                                                                       
    if ((type & S_IXGRP) == S_IXGRP)                                        
    {                                                                       
        perm[6] = 'x';                                                      
    }                                                                       
    if ((type & S_IROTH) == S_IROTH)                                        
    {                                                                       
        perm[7] = 'r';                                                      
    }                                                                       
    if ((type & S_IWOTH) == S_IWOTH)                                        
    {                                                                       
        perm[8] = 'w';                                                      
    }                                                                       
    if ((type & S_IXOTH) == S_IXOTH)                                        
    {                                                                       
        perm[9] = 'x';                                                      
    }                           
    perm[10] = '\0';
    return perm;
}

char *getTime(char *filePath, char *time)
{
    struct stat sb;

    stat(filePath, &sb);
    strftime(time, 17, "%Y-%m-%d %H:%M", localtime(&sb.st_mtime));
    return time;
}



void create(char *file, int fd)
{
    struct stat sb;

    if (lstat(file, &sb) == -1)
    {
        return;
    }
    if (S_ISREG(sb.st_mode)) { makeFile(file, fd); }
    else if (S_ISDIR(sb.st_mode)) { makeDir(file, fd); }
}

void makeDir(char *file, int fd)
{
    struct stat sb;
    struct dirent *finfo;
    DIR *dir;
    char name[255];
    
    strcpy(name, file);
    strcat(name, "/");
    
    makeFile(name, fd);
    if (!(dir = opendir(file)))
    {
        perror("opendir");
        exit(-1);
    }
    while ((finfo = readdir(dir)))
    {
        if (strcmp(finfo -> d_name, ".") != 0 && 
            strcmp(finfo -> d_name, "..") != 0)
        {
            strcpy(name, file); 
            strcat(name, "/");
            strcat(name, finfo -> d_name);
            if (lstat(name, &sb) == -1)
            {
                perror("lstat");
                exit(-1);
            }
            if (strlen(name) > 255)
            {
                continue;
            }

            if (S_ISREG(sb.st_mode))
            {
                makeFile(name, fd);
            }
            else if (S_ISDIR(sb.st_mode))
            {
                makeDir(name, fd);
            }
        }
    }
    closedir(dir);
}

void makeFile(char *file, int fd)
{
    int i = 0;
    struct header hp;    
    int fd2 = 0;
    long size;
    char buff[512];

    header(file, &hp);
    if (write(fd, &hp, 512) == -1 ) {perror("write"); exit(-1);}
    size = strtol(hp.size, NULL, 8);
    if (*(hp.typeflag) == '0')
    {
        if (((fd2 = open(file, O_RDONLY)) == -1))
        {
            return;
        }
        if ((size % 512) != 0) 
        {
            for (i = 0; i < ((size / 512)) + 1; i++)
            {
                memset(buff, 0, 512);
                if (read(fd2, buff,512) == -1)
                {
                    perror("read");
                    exit(-1);
                }
                if(write(fd, buff, 512) == -1)
                {
                    perror("write");
                    exit(-1);
                }
            }
        }
        else
        {
            for (i = 0; i < ((size / 512)) + (size % 512) + 1; i++)
            {
                memset(buff, 0, 512);
                if (read(fd2, buff,512) == -1)
                {
                    perror("read");
                    exit(-1);
                }
                if(write(fd, buff, 512) == -1)
                {
                    perror("write");
                    exit(-1);
                }
            }
        }
        if ((close(fd2) == -1))
        {
            perror("close");
            exit(-1);
        }
    }
}

struct header *header(char *file, struct header *hdr)
{
    struct stat sb;
    mode_t type;
    struct passwd *pwd;
    struct group *grp;
    char *prefix; 
    char *namebuff;
    int extra = 0;
    int i = 0;
    prefix = malloc(255);
    namebuff = malloc(100);

    memset(hdr, 0, 512);
    if (lstat(file, &sb) == -1)
    {
        perror("lstat here");
        exit(-1);
    }    
    if (strlen(file) > 100)
    {
        extra = strlen(file) - 100;
        for (i = 0; i < extra; i++)
        {
            prefix[i] = file[i];
            namebuff[i] = file[i + extra]; 
        }
        strcpy(hdr -> name, namebuff);
        strcpy(hdr -> prefix, prefix);
        free(namebuff); free(prefix);
    }
    else { strcpy(hdr -> name, file); }
    type = sb.st_mode;
    pwd = getpwuid(sb.st_uid);
    grp = getgrgid(sb.st_gid);
    strcpy(hdr -> uname, pwd -> pw_name);
    strcpy(hdr -> gname, grp -> gr_name);
    strcpy(hdr -> magic, "ustar");
    (hdr -> version)[0] = '0'; 
    (hdr -> version)[1] = '0'; 
    sprintf(hdr -> size, "%11.11o", 0);
    if (S_ISREG(type)) 
    { 
        sprintf(hdr -> size, "%11.11o", (int)sb.st_size);
        *(hdr -> typeflag) = '0'; 
    }
    else if (S_ISLNK(type)) 
    { 
        readlink(file, hdr -> linkname, 100);
        *(hdr -> typeflag) = '2'; 
    }
    else if (S_ISDIR(type)) { *(hdr -> typeflag) = '5'; }
    sprintf(hdr->mode, "%7.7o", (int)sb.st_mode);
    sprintf(hdr -> mtime, "%11.11o", (int)sb.st_mtime);
    insert_special_int(hdr -> uid, 8, pwd -> pw_uid);
    sprintf(hdr -> gid, "%7.7o", (int)sb.st_gid);
    sprintf(hdr -> chksum, "%7.7o", (int)checksum(hdr));
    return hdr;
}



int insert_special_int(char *where, size_t size, int32_t val)
{
    int err = 0;
    
    if (val < 0 || (size < sizeof(val)))
    {
        err++;
    }
    else
    {
        memset(where, 0, size);
        *(int32_t *)(where + size - sizeof(val)) = htonl(val);
        *where |= 0x80;
    }
    return err;
}

long checksum(struct header *hdr)
{
    unsigned char *hp;
    int i = 0;
    long sum = 0;

    hp = (unsigned char *)hdr;
    for (i = 0; i < 512; i++)
    {
        if (i < 148 || i > 155)
        {
            sum += hp[i];
        }
        else
        {
            sum += (unsigned char)(' ');
        }
    }     
    return sum;

}

uint32_t extract_special_int(char *where, int len)
{
    int32_t val = -1;
    if ((len >= sizeof(val)) &&(where[0] & 0x80))
    {
        val = *(int32_t *)(where + len - sizeof(val));
        val = ntohl(val);
    }
    return val;
}

int same(char *named, char *hpname)
{
    int i = 0;
    int j = 0;
    
    for (i = 0; i < strlen(hpname); i++)
    {
        if (hpname[i] == named[0])
        {
            for (j = 0; j < strlen(named); j++)
            {
                if (hpname[i + j] != named[j])
                {
                    return 0;
                }
            }
            break;
        }
    }
    return 1;
}


int open_file(char *filename, int mode, int perm)
{
    int fd;
    if ((fd = open(filename, mode, perm)) < 0)
    {
        fprintf(stderr, "ERROR WITH OPEN");
        exit(1);
    }
    return fd;
}

int read_file(int fd, void *buf, size_t size)
{
    int i;
    if ((i = read(fd, buf, size)) < 0)
    {
        fprintf(stderr, "ERROR WITH READ");
        exit(1);
    }
    return i;
}

int write_file(int fd, void *buf, size_t size)
{
    int i;
    if ((i = write(fd, buf, size)) != size)
    {
        fprintf(stderr, "ERROR WITH WRITE");
        exit(1);
    }
    return i;
}

int eof(struct header *hdr)
{
    return (!hdr -> name[0] && !hdr -> prefix[0]);
}

char *getfname(struct header *hdr, char *buf, size_t size)
{
    memset(buf, 0, size);
    if (strlen(hdr -> prefix))
    {
        strncat(buf, hdr -> prefix,  154);
        strcat(buf, "/");
    }

    strncat(buf, hdr -> name, 100);
    return buf;
}
    
int contains(char **files, int numfiles, const char *filename)
{
    int i;
    char name[255];
    char file[255];
    char *cptr;

    for (i =0; i < numfiles; i++)
    {
        strcpy(file, files[i]);
        strcpy(name, filename);
        if (file[strlen(file) - 1] == '/')
        {
            file[strlen(file) - 1] = '\0';
        }
        if (!strcmp(file, name))
        {
            return 1;
        }
        if ((cptr = rindex(name, '/')))
        {
            *cptr = '\0';
        }
        while (cptr)
        {
            if (!strcmp(file, name)) { return 1; }
            if ((cptr = rindex(name, '/'))) *cptr = '\0';
        }
    }
    return 0;
}

void makePath(char *filename)
{
    char path[255];
    char dir[255];
    char *token;

    strcpy(path, filename);
    memset(dir, 0, 255);
    
    token = strtok(path, "/");

    while (token != NULL)   
    {
        strcat(dir, token);
        strcat(dir, "/");
        mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO);
        token = strtok(NULL, "/");
    }

    rmdir(dir);
}

void extract(char *tarfile, char **files, int numfiles)
{
    struct header hdr;
    int tar_fd, out_file, block, size, i;
    char buffer[512];

    tar_fd = open_file(tarfile, O_RDONLY, 0);
    
    while(read_file(tar_fd, &hdr, 512) && !eof(&hdr))
    {
        char fname[255];
        
        size =strtol(hdr.size, NULL, 8);
        block = size / 512 + ((size % 512) ? 1 : 0);

        getfname(&hdr, fname, sizeof(fname));
        if (!numfiles || contains(files, numfiles, fname))
        {
            switch(*(hdr.typeflag))
            {
                case '5':
                    mkdir(fname, S_IRWXU | S_IRWXG | S_IRWXO);
                    break;
                
                case '2':
                    symlink(hdr.linkname, fname);
                    break;
            
                default:
                    makePath(fname);

                    out_file = open_file(fname, O_WRONLY | O_CREAT,
                                    S_IRUSR | S_IWUSR |
                                    S_IRGRP | S_IWGRP | 
                                    S_IROTH | S_IWOTH);
                    for (i = 0; i <block; i++)
                    {
                        read_file(tar_fd, buffer, 512);
                        if (size < 512)
                        {
                            write_file(out_file, buffer, size);
                        }
                        else
                        {
                            write_file(out_file, buffer, 512);
                            size -= 512;
                        }
                    }
                    close(out_file);
                    break;
            }
        }
        else
        {
            lseek(tar_fd, 512 * (size / 512 + ((size % 512) ? 1: 0)), 
                SEEK_CUR);
        }
    }
    close(tar_fd);
}

void print_err(void)
{
    fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [...] ]\n");
    exit(1);
}
