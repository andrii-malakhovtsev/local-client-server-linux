#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <syslog.h>
#include <dirent.h>
#include "func.h"
#include "defs.h"

void NLastLinesFromFile(char* name, int fd_client, struct simplemessage msg, int N) {
    FILE* filePointer;
    int bufferLength = 255;
    char buffer[bufferLength];
    ssize_t read_bytes;
    if(lstat(name, &buffer) < 0) {
		strcpy(msg.sm_data, "This file doesn't exist\n");
        syslog(LOG_INFO, "Client %d: %s", msg.sm_clientpid, msg.sm_data);
        write(fd_client, &msg, sizeof(msg));
        return;
	}
    filePointer = fopen(name, "r");
    if(filePointer == -1) {
        syslog(LOG_ERR, "Client %i failed to open file", msg.sm_clientpid);
        strcpy(msg.sm_data, "Failed to open file\n");
        write(fd_client, &msg, sizeof(msg));
        return;
    }
    int linesCount = 0;
    while(fgets(buffer, bufferLength, filePointer)) {
        linesCount++;
    }
    filePointer = fopen(name, "r");
    int counter = 0;
    char* toReturn = malloc(strlen(buffer) * N);
    strcpy(toReturn, "\0");
    int i = 0;
    while(fgets(buffer, bufferLength, filePointer)) {
        if(counter >= (linesCount - N)) {
            strcat(toReturn, buffer);
            i++;
        }
        counter++;
    }
    fclose(filePointer);
    strcpy(msg.sm_data, toReturn);
    write(fd_client, &msg, sizeof(msg));
    return;
}

void IsSymbolicLink(char* name, int fd_client, struct simplemessage msg) {
	struct stat buff;
	if(lstat(name, &buff) < 0) {
		strcpy(msg.sm_data, "This file doesn't exist\n");
        syslog(LOG_INFO, "Client %d: %s", msg.sm_clientpid, msg.sm_data);
        write(fd_client, &msg, sizeof(msg));
        return;
	}
    syslog(LOG_INFO, "Client %i: file exist", msg.sm_clientpid);
	if(S_ISLNK(buff.st_mode) == 1) {
		strcpy(msg.sm_data, "It's symbolic link\n");
    }	
	else {
       strcpy(msg.sm_data, "It's not a symbolic link\n");
	}
    write(fd_client, &msg, sizeof(msg));
    return;
}

void FileMetaData(char* name, int fd_client, struct simplemessage msg) {
    struct stat buff;
    int temp = 0;
	if(stat(name, &buff) == -1) {
        syslog(LOG_ERR, "Client %i: file doesn't exist", msg.sm_clientpid);
        strcpy(msg.sm_data, "This file doesn't exist\n");
        write(fd_client, &msg, sizeof(msg));
        return;
	}
    syslog(LOG_INFO, "Client %i: file exist", msg.sm_clientpid);
    char* toReturn = malloc(3 * (30 + strlen(ctime(&buff.st_ctime))));
    strcpy(toReturn, "Last status change:  ");
    strcat(toReturn, ctime(&buff.st_ctime));
    strcat(toReturn, "Last file access:     ");
    strcat(toReturn, ctime(&buff.st_atime));
    strcat(toReturn, "Last file change:    ");
    strcat(toReturn, ctime(&buff.st_mtime));
    strcpy(msg.sm_data, toReturn);
    write(fd_client, &msg, sizeof(msg));
    return;
}