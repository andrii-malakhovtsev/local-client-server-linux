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

int main()
{
   int fd_server, 
       fd_client = -1;
   ssize_t nread;
   struct simplemessage msg;
   char* pfifoname;

   msg.sm_clientpid = getpid();
   printf("Клиент %ld запущен\n", (long)msg.sm_clientpid);
   pfifoname = makefifoname(msg.sm_clientpid, FIFO_PREFIX);
   if(mkfifo(pfifoname, PERM_FILE) == -1 && errno != EEXIST)
   {
      fprintf(stderr, "Ошибка при создании канала %s: %s\n", pfifoname, strerror(errno));
      return 1;
   }
   if((fd_server = open(SERVER_FIFO_NAME, O_WRONLY)) < 0)
   {
      fprintf(stderr, "Ошибка открытия канала %s: %s\n", SERVER_FIFO_NAME, strerror(errno));
      return 2;
   }

   printf("\n-------------------------Меню---------------------------\n");
	printf("-n *файл*     - N последних строк файла\n");
	printf("-s *файл*     - является ли файл символической ссылкой\n");
	printf("-m *файл*     - мета-данные файла\n\n");
	printf("stopServer    - остановка работы сервера\n");
	printf("--------------------------------------------------------\n\n");

   printf("Входящий запрос к серверу: ");
   gets(msg.sm_data);
   if (msg.sm_data[1] == 'n')
   {
      char str[BUFFERSIZE];
      printf("Введите число N: ");
      gets(str);
      msg.N = atoi(str);
      if(msg.N == NULL)
         printf("\nВы ввели не число ");
   }
   write(fd_server, &msg, sizeof(msg));

   if((fd_client = open(pfifoname, O_RDONLY)) == -1)
   {
      fprintf(stderr, "Ошибка открытия канала %s: %s\n", pfifoname, strerror(errno));
      return 3;
   }

   printf("\n");
   while(nread = read(fd_client, &msg, sizeof(msg)) != 0)
   {
      printf("%s", msg.sm_data);
   }
  
   printf("\n");
   printf("Клиент %ld закончил работу\n", (long)msg.sm_clientpid);
   close(fd_server);
   close(fd_client);
   unlink(pfifoname);
                                              
   return 0;
}