#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void
mydo(char* p, int argc, char* argv[]) {
    char* params[MAXARG];
    int i, pid;
    for (i = 0; i < argc; ++i) {
        params[i] = argv[i];
    }
    params[argc] = p;
    params[argc+1] = 0;
    
    if (0 == (pid = fork())) {
        // child
        exec(params[0], params);
    } else {
        // parent
        wait(&pid);
    }
}

int
main(int argc, char* argv[]) {
    char buf[512] = {0};
    char *p;
    int i, sz;

    while(read(0, buf, sizeof(buf))) {
        for (i = strlen(buf)-1; i >= 0; --i) {
            if (buf[i] == '\n')  buf[i] = '\0';
        }

        p = buf;
        while(*p != '\0') {
            mydo(p, argc - 1, argv + 1);
            sz = strlen(p);
            p += (sz + 1);
        }
    }

    exit(0);
}
