#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int
isMatch(char *s, char*pat)
{
    int i, j, find;

    if (strlen(s) < strlen(pat)) {
        return 0;
    }
    for (i = 0; i < strlen(s) - strlen(pat) + 1; ++i) {
        find = 1;
        for (j = 0; j < strlen(pat); ++j) {
            if (s[i+j] != pat[j]) {
                find = 0;
                break;
            }
        }
        if (find) {
            return 1;
        }
    }
    return 0;
}

void
find(char *path, char *pat)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
    case T_FILE:
        if (isMatch(path, pat)) {
            printf("%s\n", path);
        }
        break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0 || strcmp(de.name, ".") == 0 ||
                    strcmp(de.name, "..") == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            if (st.type == T_DIR) {
                find(buf, pat);
            } else {
                if (isMatch(buf, pat)) {
                    printf("%s\n", buf);
                }
            }
        }
        break;
    }
    close(fd);
}

int
main(int argc, char *argv[])
{
    int i;

    if(argc < 3){
        find(".", "");  // print all entries
    } else {
        for(i=1; i<argc-1; i++)
        find(argv[i], argv[argc-1]);
    }
    exit(0);
}
