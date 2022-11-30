#if !defined(FUNC_H)
#define FUNC_H

#include <fcntl.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include "func.h"
#include "defs.h"

#define BUFFERSIZE 256

void NLastLinesFromFile(char* name,  int fd_client, struct simplemessage msg, int N);
void IsSymbolicLink(char* name, int fd_client, struct simplemessage msg);
void FileMetaData(char* name, int fd_client, struct simplemessage msg);

#endif