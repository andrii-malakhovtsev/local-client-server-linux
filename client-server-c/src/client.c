#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "defs.h"

char* makefifoname(pid_t pid, const char* prefix)
{
   char *pfifoname;
   asprintf(&pfifoname, "%s.%ld", prefix, (long) pid);
   return pfifoname;
}

void showmenu() {
   printf("\n-------------------------Menu---------------------------\n");
	printf("-n *file*     - N last file lines\n");
	printf("-s *file*     - is file a symbolic link\n");
	printf("-m *file*     - file meta-data\n\n");
	printf("stopServer    - stop server\n");
	printf("--------------------------------------------------------\n\n");
}

int main() { 
   int fd_server, fd_client = -1;
   ssize_t nread;
   struct simplemessage msg;
   char* pfifoname;
   
   msg.sm_clientpid = getpid();
   printf("Client %ld started\n", (long)msg.sm_clientpid);

   pfifoname = makefifoname(msg.sm_clientpid, FIFO_PREFIX);
   if (mkfifo(pfifoname, PERM_FILE) == -1 && errno != EEXIST) {
      fprintf(stderr, "Channel creating error %s: %s\n", pfifoname, strerror(errno));
      return 1;
   }
   if ((fd_server = open(SERVER_FIFO_NAME, O_WRONLY)) < 0) {
      fprintf(stderr, "Channel opening error %s: %s\n", SERVER_FIFO_NAME, strerror(errno));
      return 2;
   }

   showmenu();
   printf("Incoming request to server: ");
   gets(msg.sm_data);
   if (msg.sm_data[1] == 'n') {
      char str[BUFFERSIZE];
      printf("Enter the N-digit: ");
      gets(str);
      msg.N = atoi(str);
      if (msg.N == NULL)
         printf("\nYou didn't enter a digit ");
   }

   write(fd_server, &msg, sizeof(msg));
   if ((fd_client = open(pfifoname, O_RDONLY)) == -1) {
      fprintf(stderr, "Channel opening error %s: %s\n", pfifoname, strerror(errno));
      return 3;
   }
   printf("\n");

   while (nread = read(fd_client, &msg, sizeof(msg)) != 0) {
      printf("%s", msg.sm_data);
   }
   
   printf("\n");
   printf("Client %ld finished working!\n", (long)msg.sm_clientpid);
   close(fd_server);
   close(fd_client);
   unlink(pfifoname);

   return 0;
}