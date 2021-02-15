#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_PRIME 35

void fork_if_needed(int fd, int fd2) {
  int fd_ch[2];
  int fd_ch2[2];  // for parent waiting child to close
  int n = 0;
  int primes[MAX_PRIME];

  // read until another side of pipe is closed by parent
  while (read(fd, &primes[n], sizeof(int))) {
    ++n;
  }

  if (n == 0) {
    exit(0);
  }

  pipe(fd_ch);
  pipe(fd_ch2);

  if (fork()) {
    // child
    close(fd);  // fd, fd2 is only used by parent process
    close(fd2);
    close(fd_ch[1]);
    close(fd_ch2[0]);
    fork_if_needed(fd_ch[0], fd_ch2[1]);
    exit(0);  // child process should end here
  } else {
    close(fd_ch[0]);
    close(fd_ch2[1]);
    // parent
    printf("prime %d\n", primes[0]);
    int i;
    for (i = 1; i < n; ++i) {
      if (primes[i] % primes[0] != 0) {
        write(fd_ch[1], &primes[i], sizeof(int));
      }
    }
    close(fd_ch[1]);
    // In fact child will not send back anything,
    // this is for blockingly waiting.
    int tmp;
    read(fd_ch2[0], &tmp, sizeof(tmp));
  }
}

int
main(int argc, char *argv[])
{
  int fd[2];
  int fd2[2];  // for parent waiting child to close
  int candidates[MAX_PRIME], sz_candidates = 0;

  int i;
  for (i = 2; i < MAX_PRIME; ++i) {
    candidates[sz_candidates++] = i;
  }

  pipe(fd);
  pipe(fd2);

  if (fork()) {
    // child
    close(fd[1]);
    close(fd2[0]);
    fork_if_needed(fd[0], fd2[1]);
  } else {
    close(fd[0]);
    close(fd2[1]);
    // parent
    for (i = 0; i < sz_candidates; ++i) {
      write(fd[1], &candidates[i], sizeof(int));
    }
    // In fact child will not send back anything,
    // this is for blockingly waiting.
    close(fd[1]);
    int tmp;
    read(fd2[0], &tmp, sizeof(int));
  }

  exit(0);
}

