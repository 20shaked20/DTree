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
#include <stdbool.h>
#include <dirent.h>
#include <limits.h>

bool lastWasDir = false; // a simple check if the last file was direcotry and now we're in a normal file//
int dirFiles = 0;
/**
 * This method gets the mode of a file and prints the permission of it.
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

int countFilesInDirectory(const char *dirname)
{
    int count = 0;
    DIR *dir = opendir(dirname);
    if (dir != NULL)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);
            
            struct stat fileStat;
            if (stat(path, &fileStat) == 0 && S_ISREG(fileStat.st_mode))
            {
                count++;
            }
        }
        closedir(dir);
    }
    return count;
}


static int dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{   

    if (type == FTW_NS)
    {
        printf("?");
    }
    else
    {
        // for(int i = 0; i< ftwb->level; ++i)
        // {
        //     // if(i == ftwb->level)
        //         printf("%d",i);
        // }
        printf("%*s", 4 * ftwb->level - ftwb->level, ""); //indent//
        
        // for(int i = 0; i< ftwb->level; ++i)
        // {
        //     // if(i == ftwb->level)
        //         printf("%*s", 1/4 * ftwb->level - ftwb->level -1 , ""); //indent few backwards to fit good//
        //         // printf("│");
        // }
        // printf("%*s", ftwb->level - ftwb->level, ""); //indent//

        switch (sbuf->st_mode & S_IFMT)
        {
        case S_IFREG: // reg file.
        {   
            --dirFiles; // remove file from current directory // 

            if((lastWasDir && dirFiles == 0) || dirFiles == 0){
                printf("└──["); 
            } 
            else{
                 printf("├──["); 
            }

            printPermissions(sbuf->st_mode); // prints [xcvxc--xcvxv--xcvxcv]
            struct passwd *owner = getpwuid(sbuf->st_uid); // gets the owner
            printf(" %s %7ld] ", owner ? owner->pw_name : "unknown", (long)sbuf->st_size); // prints owner and ends of the needed data
            printf("%s\n", &pathname[ftwb->base]); // prints file name
            lastWasDir = false;
            break;
        }
        case S_IFDIR: // directory only//
        {  

            if (ftwb->level == 0){ // for the first file
                printf(".\n");
                printf("└──["); 
            }
            else{
                printf("├──[");
            }
             
            printPermissions(sbuf->st_mode); // prints [xcvxc--xcvxv--xcvxcv]
            struct passwd *owner = getpwuid(sbuf->st_uid); // gets the owner
            printf(" %s %7ld] ", owner ? owner->pw_name : "unknown", (long)sbuf->st_size); // prints owner and ends of the needed data
            printf("%s/\n", &pathname[ftwb->base]); // prints file name
            lastWasDir = true;

            /*we do this to get the amount of files in directory for smooth printing*/
            const char *dir = pathname;
            dirFiles = countFilesInDirectory(pathname);

            break;
        }
        default:
            break;
        }
    }
    return 0;
}



int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s directory-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int flags = FTW_PHYS;
    int maxDepth = 10;
    if (nftw(argv[1], dirTree, maxDepth, flags) == -1)
    {
        perror("nftw");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
