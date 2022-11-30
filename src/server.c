#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include "defs.h"
#include "func.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>


char* makefifoname(pid_t pid, const char* prefix)
{
   char *pfifoname;
   asprintf(&pfifoname, "%s.%ld", prefix, (long) pid);
   return pfifoname;
}

int main(int argc, char *argv[])
{

   FILE *fp;
   char path[1035];
   int count = 0;
   fp = popen("ps -x", "r");
   while (fgets(path, sizeof(path), fp) != NULL) {
         if(strstr(path, "my_server") != NULL)
            {
               count++;
               if(count > 1)
               {
                  printf("Сервер уже есть\n");
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
   syslog(LOG_INFO, "Сервер запущен");

   if (mkfifo(SERVER_FIFO_NAME, PERM_FILE) == -1 && errno != EEXIST)
   {
      syslog(LOG_ERR, "Ошибка при создании канала %s", SERVER_FIFO_NAME);
      return 1;
   }
   if ((fd_server = open(SERVER_FIFO_NAME, O_RDONLY)) < 0)
   {
      syslog(LOG_ERR, "Ошибка открытия канала %s", SERVER_FIFO_NAME);
      return 2;
   }
   if ((fd_dummy = open(SERVER_FIFO_NAME, O_WRONLY)) < 0)
   {
      syslog(LOG_ERR, "Ошибка при чтении канала %s", SERVER_FIFO_NAME);
      return 2;
   }

   while ((nread = read(fd_server, &msg, sizeof(msg))) > 0)
   {
      syslog(LOG_INFO, "Получен запрос от %i\n", msg.sm_clientpid);

      pid_t forkPid = fork();
      if (forkPid == 0)
      {
         char *pfifoname;
         if (strcmp("stopServer", msg.sm_data) == 0)
         {
            pfifoname = makefifoname(msg.sm_clientpid, FIFO_PREFIX);
            fd_client = open(pfifoname, O_WRONLY);

            syslog(LOG_INFO, "Сервер закрыт\n");
            strcpy(msg.sm_data, "Сервер успешно закрыт\n");
            write(fd_client, &msg, sizeof(msg));
            close(fd_client);
            unlink(pfifoname);
            system("killall my_server");
            _exit(0);
            return 0;
         }
         else if (strcmp("", msg.sm_data) != 0)
         {
            char name[BUFFERSIZE];
            memset(name, 0, sizeof(name));

            int i = 0;
            for (int j = 3; msg.sm_data[j] != NULL; j++)
            {
               name[i] = msg.sm_data[j];
               i++;
            }

            pfifoname = makefifoname(msg.sm_clientpid, FIFO_PREFIX);
            fd_client = open(pfifoname, O_WRONLY);

            char c = msg.sm_data[1];
            int msgLength = 0;
            switch(c)
            {
               case 'n':
               {
                  NLastLinesFromFile(name, fd_client, msg, msg.N);
                  msgLength++;
                  break;
               }
               case 's':
               {
                  IsSymbolicLink(name, fd_client, msg);
                  msgLength++;
                  break;
               }
               case 'm':
               {
                  FileMetaData(name, fd_client, msg);
                  msgLength++;
                  break;
               }
            }

            if(msgLength == 0)
            {
               strcpy(msg.sm_data, "Некорректная запись");
               write(fd_client, &msg, sizeof(msg));
            }

            syslog(LOG_INFO, "Запрос клиента %i выполнен", msg.sm_clientpid);
            close(fd_client);
            unlink(pfifoname);
         }
         else if(strcmp("", msg.sm_data) == 0)
         {
            pfifoname = makefifoname(msg.sm_clientpid, FIFO_PREFIX);
            fd_client = open(pfifoname, O_WRONLY);

            syslog(LOG_INFO, "Сервер закрыт\n");
            strcpy(msg.sm_data, "Неверная запись");
            write(fd_client, &msg, sizeof(msg));

            close(fd_client);
            unlink(pfifoname);
            close(fd_server);
            _exit(0);
            return 0;
         }
         _exit(0);
      }
      else if(forkPid < 0)
      {
         syslog(LOG_ERR, "Запрос от %i не обработан", msg.sm_clientpid);
      }
   }
   close(fd_server);
   _exit(0);
   return 0;
}