#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int fd_pa[2];
  int fd_ch[2];
  char buf[8];

  pipe(fd_pa);
  pipe(fd_ch);

  if (fork() == 0) {
    // child
    close(fd_pa[1]);
    close(fd_ch[0]);
    read(fd_pa[0], buf, 1);
    // printf("child read %c", buf[0]);
    printf("%d: received ping\n", getpid());
    write(fd_ch[1], "c", 1);
    exit(0);
  }

  // parent
  close(fd_pa[0]);
  close(fd_ch[1]);
  write(fd_pa[1], "p", 1);
  read(fd_ch[0], buf, 1);
  // printf("parent read %c", buf[0]);
  printf("%d: received pong\n", getpid());

  exit(0);
}

