#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include "defs.h"

char* makefifoname(pid_t pid, const char* prefix) {
    char *pfifoname;
    asprintf(&pfifoname, "%s.%ld", prefix, (long) pid);
    return pfifoname;
}

void showmenu() {
    printf("Commands:\n");
    printf("1. Get N lines from file: n:<N>:<filename>\n");
    printf("2. Check symbolic link: s:<filename>\n");
    printf("3. Get file metadata: m:<filename>\n");
    printf("4. Stop server: stopServer\n");
}

int main(int argc, char* argv[]) {
    char *pfifoname;
    int fd_server, fd_client;
    struct simplemessage msg;

    pfifoname = makefifoname(getpid(), FIFO_PREFIX);
    mkfifo(pfifoname, PERM_FILE);

    fd_server = open(SERVER_FIFO_NAME, O_WRONLY);
    if (fd_server == -1) {
        perror("Failed to open server FIFO");
        exit(EXIT_FAILURE);
    }

    printf("Enter command: ");
    if (fgets(msg.sm_data, BUFFERSIZE, stdin)) {
        msg.sm_data[strcspn(msg.sm_data, "\n")] = 0;
        msg.sm_clientpid = getpid();
        msg.N = 10; // Default for testing

        write(fd_server, &msg, sizeof(msg));
        fd_client = open(pfifoname, O_RDONLY);
        read(fd_client, &msg, sizeof(msg));
        printf("Server response: %s\n", msg.sm_data);
        close(fd_client);
    }

    unlink(pfifoname);
    close(fd_server);
    return 0;
}
