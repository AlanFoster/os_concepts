#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/errno.h>
#include <dirent.h>

// #define print_string printf

// #define NAME_MAX 14

// // typedef struct {
// //     long ino;
// //     char name[NAME_MAX + 1]; // +1 for the '\0' terminator
// // } Dirent;

// // typedef struct { //minimal dir, no buffering etc
// //     int fd;  // File director for directory
// //     Dirent d;  // the directory entry
// // } DIR;

// // DIR *opendir(char *dirname);
// // Dirent *readdir(DIR *dfd);
// // void closedir(DIR *dfd);

// // char *name;
// // struct stat

// #define MAX_PATH 1024

// void dirwalk(char *dir, void (*fcn)(char *)){
//     char name[MAX_PATH];
//     Dirent *dp;
//     DIR *dfd;

//     if ((dfd = opendir(dir)) == NULL) {
//         fprintf(stderr, "dirwalk: can't open %s", dir);
//         return;
//     }

//     while ((dp = readdir(dfd)) != NULL) {
//         if (strcmp(dp->name, ".") == 0 || strcmp(dp->name, "..") == 0) {
//             continue; // Skip self and parent
//         }

//         // printf("wwwwwwwwwwwwwwwwwhat %s\n", dp->name);

//         if (strlen(dir) + strlen(dp->name)+2 > sizeof(name)) {
//             fprintf(stderr, "dirwalk: name %s/%s too long \n", dir, dp->name);
//             continue;
//         }

//         // printf("childs: %s/%s\n", dir, dp->name);
//         sprintf(name, "%s/%s", dir, dp->name);
//         (*fcn)(name);
//     }

//     closedir(dfd);
// }

// void fsize(char *name) {
//     struct stat statbuf;

//     int result = stat(name, &statbuf);
//     if (result == -1) {
//         fprintf(stderr, "fsize: can't access %s %d %d\n", name, result, errno);
//         return;
//     }

//     // printf("%d\n", statbuf.st_mode & S_IFMT);

//     // printf("%d\n", S_IFDIR);
//     if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
//         dirwalk(name, fsize);
//     }
//     printf("%8ld %s\n", statbuf.st_size, name);
// }


// int main (int argc, char **argv) {
//     print_string("hello world\n");

//     if (argc == 1) {
//         fsize(".");
//     } else {
//         while (--argc > 0) {
//             fsize(*++argv);
//         }
//     }
//     return 0;
// }

void print_file_details(char *path) {
    struct stat statbuff;
    if (stat(path, &statbuff) != 0) {
        fprintf(stderr, "problem reading %s\n", path);
        return;
    }

    if ((statbuff.st_mode & S_IFMT) == S_IFDIR) {
        printf("%8ld directory %s\n", statbuff.st_size, path);
    } else {
        printf("%8ld file %s\n", statbuff.st_size, path);
    }
}

#define MAX_PATH 255
int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage: prog <name>\n");
    }

    char *path = argv[1];
    DIR *pDir;
    pDir = opendir(path);
    if (pDir == NULL) {
        printf("Cannot open directory %s\n", path);
        return ENOENT;
    }

    struct dirent *entry;
    while ((entry = readdir(pDir)) != NULL) {
        char childpath[MAX_PATH];

        if (strlen(path) + entry->d_namlen > sizeof(childpath)) {
            fprintf(stderr, "path too long %s/%s\n", path, entry->d_name);
            continue;
        }

        sprintf(childpath, "%s/%s", path, entry->d_name);
        print_file_details(childpath);
    }

    return 0;
}