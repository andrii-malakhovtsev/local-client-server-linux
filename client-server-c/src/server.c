#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <dirent.h>
#include "defs.h"
#include "func.h"

// Utility to create FIFO names dynamically
char* makefifoname(pid_t pid, const char* prefix) {
    char *pfifoname;
    asprintf(&pfifoname, "%s.%ld", prefix, (long) pid);
    return pfifoname;
}

int main(int argc, char *argv[]) {
    FILE *fp;
    char path[PATHSIZE];
    int instance_count = 0;

    // Check for existing server instances
    fp = popen("ps -x", "r");
    while (fgets(path, sizeof(path), fp) != NULL) {
        if (strstr(path, "server") != NULL) {
            instance_count++;
            if (instance_count > 1) {
                printf("Server already exists\n");
                return 0;
            }
        }
    }
    pclose(fp);

    int fd_server, fd_dummy, fd_client = -1;
    ssize_t nread;
    struct simplemessage msg;

    // Create daemon process
    daemon(1, 1);
    openlog("server", LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Server opened");

    // Create server FIFO
    if (mkfifo(SERVER_FIFO_NAME, PERM_FILE) == -1 && errno != EEXIST) {
        syslog(LOG_ERR, "Failed to create server FIFO: %s", SERVER_FIFO_NAME);
        return 1;
    }

    // Open FIFO for reading and dummy writing
    if ((fd_server = open(SERVER_FIFO_NAME, O_RDONLY)) < 0) {
        syslog(LOG_ERR, "Failed to open server FIFO: %s", SERVER_FIFO_NAME);
        return 2;
    }
    if ((fd_dummy = open(SERVER_FIFO_NAME, O_WRONLY)) < 0) {
        syslog(LOG_ERR, "Failed to open dummy write end: %s", SERVER_FIFO_NAME);
        return 2;
    }

    while ((nread = read(fd_server, &msg, sizeof(msg))) > 0) {
        syslog(LOG_INFO, "Received request from %d", msg.sm_clientpid);
        pid_t fork_pid = fork();

        if (fork_pid == 0) { // Child process
            char *client_fifo;
            client_fifo = makefifoname(msg.sm_clientpid, FIFO_PREFIX);
            fd_client = open(client_fifo, O_WRONLY);

            if (strcmp("stopServer", msg.sm_data) == 0) {
                syslog(LOG_INFO, "Server stopped by client %d", msg.sm_clientpid);
                strcpy(msg.sm_data, "Server successfully closed\n");
                write(fd_client, &msg, sizeof(msg));
                close(fd_client);
                unlink(client_fifo);
                free(client_fifo);
                system("killall server");
                _exit(0);
            } else if (strcmp("", msg.sm_data) != 0) {
                char name[BUFFERSIZE] = {0};
                strncpy(name, msg.sm_data + 3, sizeof(name) - 1);

                switch (msg.sm_data[1]) {
                    case 'n':
                        writeNlastlinesfromfile(name, fd_client, msg, msg.N);
                        break;
                    case 's':
                        writeisfilesymboliclink(name, fd_client, msg);
                        break;
                    case 'm':
                        writefilemetadata(name, fd_client, msg);
                        break;
                    default:
                        strcpy(msg.sm_data, "Invalid command");
                        write(fd_client, &msg, sizeof(msg));
                        break;
                }

                syslog(LOG_INFO, "Completed client %d request", msg.sm_clientpid);
                close(fd_client);
                unlink(client_fifo);
                free(client_fifo);
            } else {
                strcpy(msg.sm_data, "Invalid entry");
                write(fd_client, &msg, sizeof(msg));
                close(fd_client);
                unlink(client_fifo);
                free(client_fifo);
            }
            _exit(0);
        } else if (fork_pid < 0) {
            syslog(LOG_ERR, "Failed to fork for client %d", msg.sm_clientpid);
        }
    }

    close(fd_server);
    close(fd_dummy);
    unlink(SERVER_FIFO_NAME);
    syslog(LOG_INFO, "Server shutting down");
    closelog();
    return 0;
}
