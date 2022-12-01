#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "defs.h"
#include "func.h"

char* makefifoname(pid_t pid, const char* prefix) {
   char *pfifoname;
   asprintf(&pfifoname, "%s.%ld", prefix, (long) pid);
   return pfifoname;
}

int main(int argc, char *argv[]) {
   FILE *fp;
   char path[1035];
   int count = 0;
   fp = popen("ps -x", "r");
   while (fgets(path, sizeof(path), fp) != NULL) {
         if(strstr(path, "server") != NULL){
               count++;
               if(count > 1) {
                  printf("Server already exists\n");
                  return 0;
               }
            }
   }
   pclose(fp);
   int fd_server,
       fd_dummy,
       fd_client = -1;
   ssize_t nread;
   struct simplemessage msg;
   daemon(1, 1);
   openlog("my_server", LOG_PID, LOG_USER);
   syslog(LOG_INFO, "Server opened");
   if (mkfifo(SERVER_FIFO_NAME, PERM_FILE) == -1 && errno != EEXIST) {
      syslog(LOG_ERR, "Channel creating error %s", SERVER_FIFO_NAME);
      return 1;
   }
   if ((fd_server = open(SERVER_FIFO_NAME, O_RDONLY)) < 0) {
      syslog(LOG_ERR, "Channel open error %s", SERVER_FIFO_NAME);
      return 2;
   }
   if ((fd_dummy = open(SERVER_FIFO_NAME, O_WRONLY)) < 0) {
      syslog(LOG_ERR, "Channel read error %s", SERVER_FIFO_NAME);
      return 2;
   }
   while ((nread = read(fd_server, &msg, sizeof(msg))) > 0)
   {
      syslog(LOG_INFO, "Got request from %i\n", msg.sm_clientpid);
      pid_t forkPid = fork();
      if (forkPid == 0) {
         char *pfifoname;
         if (strcmp("stopServer", msg.sm_data) == 0) {
            pfifoname = makefifoname(msg.sm_clientpid, FIFO_PREFIX);
            fd_client = open(pfifoname, O_WRONLY);

            syslog(LOG_INFO, "Server closed\n");
            strcpy(msg.sm_data, "Server is successfully closed\n");
            write(fd_client, &msg, sizeof(msg));
            close(fd_client);
            unlink(pfifoname);
            system("killall my_server");
            _exit(0);
            return 0;
         }
         else if (strcmp("", msg.sm_data) != 0) {
            char name[BUFFERSIZE];
            memset(name, 0, sizeof(name));
            int i = 0;
            for (int j = 3; msg.sm_data[j] != NULL; j++) {
               name[i] = msg.sm_data[j];
               i++;
            }
            pfifoname = makefifoname(msg.sm_clientpid, FIFO_PREFIX);
            fd_client = open(pfifoname, O_WRONLY);
            char c = msg.sm_data[1];
            int messageLength = 0;
            switch(c) {
               case 'n': {
                  writeNlastlinesfromfile(name, fd_client, msg, msg.N);
                  messageLength++;
                  break;
               }
               case 's': {
                  writeisfilesymboliclink(name, fd_client, msg);
                  messageLength++;
                  break;
               }
               case 'm': {
                  writeifilemetadata(name, fd_client, msg);
                  messageLength++;
                  break;
               }
            }

            if(messageLength == 0) {
               strcpy(msg.sm_data, "Incorrect entry");
               write(fd_client, &msg, sizeof(msg));
            }
            syslog(LOG_INFO, "Client request %i has been completed", msg.sm_clientpid);
            close(fd_client);
            unlink(pfifoname);
         }
         else if (strcmp("", msg.sm_data) == 0) {
            pfifoname = makefifoname(msg.sm_clientpid, FIFO_PREFIX);
            fd_client = open(pfifoname, O_WRONLY);
            syslog(LOG_INFO, "Server closed\n");
            strcpy(msg.sm_data, "Invalid entry");
            write(fd_client, &msg, sizeof(msg));
            close(fd_client);
            unlink(pfifoname);
            close(fd_server);
            _exit(0);
            return 0;
         }
         _exit(0);
      }
      else if(forkPid < 0) {
         syslog(LOG_ERR, "Request from %i hasn't been completed", msg.sm_clientpid);
      }
   }
   close(fd_server);
   _exit(0);
   return 0;
}