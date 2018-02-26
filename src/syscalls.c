/*
 * syscalls.c
 *
 *  Created on: Feb 8, 2018
 *      Author: daniel
 */


/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#include <board.h>

#undef errno
extern int errno;

//from uart-board.c:
extern int e_getchar(void);
extern int e_printchar(char ch);

int __io_putchar( int c )
{
	return e_printchar(c);
}

int __io_getchar(void)
{
	return e_getchar();
}


/*
 * _gettimeofday primitive (Stub function)
 * */
int _gettimeofday (struct timeval* tp, struct timezone* tzp) {
  /* Return fixed data for the timezone.  */
  if (tzp) {
    tzp->tz_minuteswest = 0;
    tzp->tz_dsttime = 0;
  }

  return 0;
}
void initialise_monitor_handles() {
}

int _getpid(void) {
  return 1;
}

int _kill(int pid, int sig) {
  errno = EINVAL;
  return -1;
}

void _exit (int status) {
  _kill(status, -1);
  while (1) {}
}

int _write(int file, char* ptr, int len) {
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++) {
    __io_putchar( *ptr++ );
  }
  return len;
}

int _close(int file) {
  return -1;
}

int _fstat(int file, struct stat* st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file) {
  return 1;
}

int _lseek(int file, int ptr, int dir) {
  return 0;
}

int _read(int file, char* ptr, int len) {
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++) {
    *ptr++ = __io_getchar();
  }

  return len;
}

int _open(char* path, int flags, ...) {
  /* Pretend like we always fail */
  return -1;
}

int _wait(int* status) {
  errno = ECHILD;
  return -1;
}

int _unlink(char* name) {
  errno = ENOENT;
  return -1;
}

int _times(struct tms* buf) {
  return -1;
}

int _stat(char* file, struct stat* st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int _link(char* old, char* new) {
  errno = EMLINK;
  return -1;
}

int _fork(void) {
  errno = EAGAIN;
  return -1;
}

int _execve(char* name, char** argv, char** env) {
  errno = ENOMEM;
  return -1;
}
