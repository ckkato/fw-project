#define BLK_SIZE 512

struct header

{
   char name     [100];
   char mode     [8];
   char uid      [8];
   char gid      [8];
   char size     [12];
   char mtime    [12];
   char chksum   [8];
   char typeflag [1];
   char linkname [100];
   char magic    [6];
   char version  [2];
   char uname    [32];
   char gname    [32];
   char devmajor [8];
   char devminor [8];
   char prefix   [155];
};


int is_c(char *);                                                               

int is_t(char *);                                                               

int is_x(char *);                                                               

int is_v(char *);                                                               

int is_S(char *);                                                               

void create(char *, int);

char *permissions(char *, mode_t, char);

char *getTime(char *, char *);

void basic_t(char *, int, char*, int);

void verbose_t(char *);

void makeFile(char *, int);

struct header *header(char *, struct header *);

int insert_special_int(char *, size_t, int32_t);

long checksum(struct header *);

void makeDir(char *, int);

uint32_t extract_special_int(char *, int);

int same(char *, char *);

int open_file(char *filename, int mode, int perm);

int read_file(int fd, void *buf, size_t size);

int write_file(int fd, void *buf, size_t size);

int eof(struct header *hdr);

char *getfname(struct header *hdr, char *buf, size_t size);

int contains(char **files, int numfiles, const char *filename);

void extract(char *tarfile, char **files, int numfiles);

void print_err(void);
