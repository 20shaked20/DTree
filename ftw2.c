/* nftw_dir_tree.c
   Demonstrate the use of nftw(3). Walk though the directory tree specified
   on the command line (or the current working directory if no directory
   is specified on the command line), displaying an indented hierarchy
   of files in the tree. For each file, display:
      * a letter indicating the file type (using the same letters as "ls -l")
      * a string indicating the file type, as supplied by nftw()
      * the file's i-node number.
*/
#define _XOPEN_SOURCE 600 /* Get nftw() */
#include <ftw.h>
#include <sys/types.h> /* Type definitions used by many programs */
#include <stdio.h>     /* Standard I/O functions */
#include <stdlib.h>    /* Prototypes of commonly used library functions,
                             plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>    /* Prototypes for many system calls */
#include <errno.h>     /* Declares errno and defines error constants */
#include <string.h>    /* Commonly used string-handling functions */
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>


/**
 * this method prints the relevant permissions of each file.
*/
void printPermissions(mode_t mode)
{
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

static int /* Callback function called by ftw() */
dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    if (type == FTW_NS)
    { /* Could not stat() file */
        printf("?");
    }
    else
    {
        switch (sbuf->st_mode & S_IFMT)
        { /* Print file type */
        case S_IFREG:
        {
            printf("%*s[", 4 * ftwb->level, ""); 
            printPermissions(sbuf->st_mode);
            struct passwd *owner = getpwuid(sbuf->st_uid);
            printf(" %s ", owner ? owner->pw_name : "unknown");
            printf("%7ld] ", (long)sbuf->st_size);
            printf("%s\n", &pathname[ftwb->base]);
            break;
        }
        case S_IFDIR:
        {
            printf("%*s[", 4 * ftwb->level, "");
            printPermissions(sbuf->st_mode);
            struct passwd *owner = getpwuid(sbuf->st_uid);
            printf(" %s ", owner ? owner->pw_name : "unknown");
            printf("%7ld] ", (long)sbuf->st_size);
            printf("%s/\n", &pathname[ftwb->base]);
            break;
        }
        default:
            break;
        }
    }
    return 0; /* Tell nftw() to continue */ /* Tell nftw() to continue */
}

int main(int argc, char *argv[])
{
    int flags = 0;
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s directory-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (nftw(argv[1], dirTree, 10, flags) == -1)
    {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
