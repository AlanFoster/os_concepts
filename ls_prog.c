#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
// #include "unistd.h"
#define PERMS 0755

#define print_string printf

void error(char fmt[], ...) {
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, "error: ");
    fprintf(stderr, *fmt);
    // vfprint(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

int main(int argc, char *argv[]) {
    print_string("hello world\n");

    int f1, f2, n;
    char buf[BUFSIZ];

    if (argc != 3) {
        error("Usage: cp from to");
    }

    if ((f1 = open(argv[1], O_RDONLY, 0)) == -1) {
        error("cp: can't open %s", argv[1]);
    }

    if ((f2 = creat(argv[2], PERMS)) == -1) {
        error("cp: can't create %s, mode %03o", argv[2], PERMS);
    }

    while ((n = read(f1, buf, BUFSIZ)) > 0) {
        if (write(f2, buf, n) != n) {
            error("cp: write error on file %s", argv[2]);
        }
    }

    // write(0, "hello waaaaaorld", 15);

    return 0;
}


// #define NULL 0
// #define EOF (-1)
// #define BUFSIZE 2014
// #define OPEN_MAX 20
// #define print_string printf

// typedef struct _iobuf {
//     int cnt; // chars left
//     char *ptr;
//     char *base;
//     int flag; // file mode access
//     int fd; // File descript0r
// } FILE;
// extern FILE _iob[OPEN_MAX];

// #define stdin (&_iob[0]);
// #define stdout (&_iob[1]);
// #define stderr (&_iob[2]);

// enum _flags {
//     _READ = 01,
//     _WRITE = 02,
//     _UNBUF = 04,
//     _EOF = 010,
//     _ERR = 020
// };

// int _fillbuff(FILE *);
// int _flushbuf(int, FILE *);

// #define feof(p) (((p)->flag &_EOF) != 0)
// #define feof(p) (((p)->flag &_ERR) != 0)
// #define fileno(p) ((p)->fd)

// #define getc(p) (--(p)->cnt >= 0 ? (unsigned char) *(p)->ptr++ : _fillbuff(p)))

