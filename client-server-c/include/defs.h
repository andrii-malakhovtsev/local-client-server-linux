#if !defined(DEFS_H)
#define DEFS_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

#define PERM_FILE (S_IRUSR | S_IWUSR | S_IROTH)
#define SERVER_FIFO_NAME "/tmp/fifo.request"
#define FIFO_PREFIX "/tmp/fifo.answer"
#define BUFFERSIZE 256
#define PATHSIZE 1035
#define LINEBUFFER 255

struct simplemessage {
    pid_t sm_clientpid;
    char sm_data[BUFFERSIZE];
    int N;
};

void showmenu();
char* makefifoname(pid_t pid, const char* prefix);

#endif