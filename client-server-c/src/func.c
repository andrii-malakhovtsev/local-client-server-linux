#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <dirent.h>
#include "func.h"
#include "defs.h"

void writeNlastlinesfromfile(char* name, int fd_client, struct simplemessage msg, int N) {
    FILE* filePointer;
    char buffer[LINEBUFFER];
    struct stat file_stat;

    if (lstat(name, &file_stat) < 0) {
        strcpy(msg.sm_data, "File does not exist\n");
        syslog(LOG_INFO, "Client %d: %s", msg.sm_clientpid, msg.sm_data);
        write(fd_client, &msg, sizeof(msg));
        return;
    }

    if (!(filePointer = fopen(name, "r"))) {
        syslog(LOG_ERR, "Client %d: failed to open file %s", msg.sm_clientpid, name);
        strcpy(msg.sm_data, "Failed to open file\n");
        write(fd_client, &msg, sizeof(msg));
        return;
    }

    int line_count = 0;
    while (fgets(buffer, sizeof(buffer), filePointer)) line_count++;
    rewind(filePointer);

    char result[N * LINEBUFFER];
    memset(result, 0, sizeof(result));
    int current_line = 0;

    while (fgets(buffer, sizeof(buffer), filePointer)) {
        if (current_line++ >= (line_count - N)) {
            strncat(result, buffer, sizeof(result) - strlen(result) - 1);
        }
    }

    fclose(filePointer);
    strcpy(msg.sm_data, result);
    write(fd_client, &msg, sizeof(msg));
}

void writeisfilesymboliclink(char* name, int fd_client, struct simplemessage msg) {
    struct stat file_stat;
    if (lstat(name, &file_stat) < 0) {
        strcpy(msg.sm_data, "File does not exist\n");
        syslog(LOG_INFO, "Client %d: %s", msg.sm_clientpid, msg.sm_data);
        write(fd_client, &msg, sizeof(msg));
        return;
    }

    if (S_ISLNK(file_stat.st_mode)) {
        strcpy(msg.sm_data, "File is a symbolic link\n");
    } else {
        strcpy(msg.sm_data, "File is not a symbolic link\n");
    }
    write(fd_client, &msg, sizeof(msg));
}

void writefilemetadata(char* name, int fd_client, struct simplemessage msg) {
    struct stat file_stat;

    if (stat(name, &file_stat) < 0) {
        syslog(LOG_ERR, "Client %d: file does not exist", msg.sm_clientpid);
        strcpy(msg.sm_data, "File does not exist\n");
        write(fd_client, &msg, sizeof(msg));
        return;
    }

    char metadata[3 * BUFFERSIZE];
    snprintf(metadata, sizeof(metadata),
             "Last status change: %sLast file access: %sLast file modification: %s",
             ctime(&file_stat.st_ctime),
             ctime(&file_stat.st_atime),
             ctime(&file_stat.st_mtime));

    strcpy(msg.sm_data, metadata);
    write(fd_client, &msg, sizeof(msg));
}
