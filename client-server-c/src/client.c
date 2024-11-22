#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

void showmenu() {
    printf("Available Commands:\n");
    printf("  1. Get N lines from file: n:<N>:<filename>\n");
    printf("  2. Check if file is a symbolic link: s:<filename>\n");
    printf("  3. Get file metadata: m:<filename>\n");
    printf("  4. Stop server: stopServer\n");
    printf("\nExample command: n:10:/path/to/file.txt\n");
    printf("Type 'exit' to quit the client.\n");
}

char* makefifoname(pid_t pid, const char* prefix) {
    char *pfifoname;
    asprintf(&pfifoname, "%s.%ld", prefix, (long) pid);
    return pfifoname;
}

int main() {
    char *client_fifo;
    int fd_server, fd_client;
    struct simplemessage msg;

    msg.sm_clientpid = getpid();
    client_fifo = makefifoname(msg.sm_clientpid, FIFO_PREFIX);
    if (mkfifo(client_fifo, PERM_FILE) == -1 && errno != EEXIST) {
        perror("Failed to create client FIFO");
        exit(EXIT_FAILURE);
    }

    fd_server = open(SERVER_FIFO_NAME, O_WRONLY);
    if (fd_server == -1) {
        perror("Failed to open server FIFO");
        unlink(client_fifo);
        exit(EXIT_FAILURE);
    }

    printf("Client started (PID: %d)\n", msg.sm_clientpid);
    showmenu();

    while (1) {
        printf("\nEnter command: ");
        char input[BUFFERSIZE] = {0};

        if (!fgets(input, sizeof(input), stdin)) {
            printf("Error reading input\n");
            break;
        }
        input[strcspn(input, "\n")] = 0; // Remove trailing newline

        if (strcmp(input, "exit") == 0) {
            printf("Client finished working. Exiting...\n");
            break;
        }

        strncpy(msg.sm_data, input, sizeof(msg.sm_data) - 1);

        // Handle special case for 'n:<N>:<filename>' command
        if (msg.sm_data[0] == 'n' && msg.sm_data[1] == ':') {
            char *colon1 = strchr(msg.sm_data, ':');
            char *colon2 = strchr(colon1 + 1, ':');

            if (colon1 && colon2) {
                *colon1 = '\0';
                *colon2 = '\0';

                msg.N = atoi(colon1 + 1); // Extract N
                strncpy(msg.sm_data, colon2 + 1, sizeof(msg.sm_data) - 1); // Extract filename
            } else {
                printf("Invalid format for 'n' command. Use n:<N>:<filename>.\n");
                continue;
            }
        }

        if (write(fd_server, &msg, sizeof(msg)) == -1) {
            perror("Failed to send message to server");
            continue;
        }

        fd_client = open(client_fifo, O_RDONLY);
        if (fd_client == -1) {
            perror("Failed to open client FIFO for reading");
            continue;
        }

        ssize_t nread = read(fd_client, &msg, sizeof(msg));
        if (nread > 0) {
            printf("Server response:\n%s\n", msg.sm_data);
        } else {
            perror("Failed to read server response");
        }

        close(fd_client);
    }

    close(fd_server);
    unlink(client_fifo);
    free(client_fifo);

    return 0;
}
