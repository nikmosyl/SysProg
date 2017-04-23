#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUF_LEN 1024
//
int main()
{
	int sock, listener;
	struct sockaddr_in addr;
	char buf[BUF_LEN];
	int bytes_read;
	int portNumber = 1000;
	int completeBind = 0;

	listener = socket(AF_INET, SOCK_STREAM, 0);

	if(listener < 0)
	{
		perror("Во время создания сокета возникла ошибка");
		exit(-1);
	}
	while(completeBind==0)
	{
		addr.sin_family = AF_INET;
		addr.sin_port = htons(portNumber);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
			portNumber++;
		}
		else
		{
			completeBind=1;
		}
	}
	printf("Сервер доступен. Порт %d\n", portNumber);

	listen(listener, 1);
	printf(">>Сервер запущен!\n");

	while(1)
	{
		sock = accept(listener, NULL, NULL);

		if(sock < 0)
		{
			perror("Соединение с сокетом нарушено");
			exit(-3);
		}
		bytes_read = recv(sock, buf, BUF_LEN, 0);
		if (bytes_read < 0)
		{
			printf(">>Ошибка при получении данных\n");
			send(sock, "Овет сервера: ОШИБКА!\n", BUF_LEN, 0);
		}
		else
		{
			int i;
			printf("Сообщение от клиента: ");

			for (i = 0; i < bytes_read; i++)
			{
				printf("%c", buf[i]);
			}
			printf("\n");
			if(endSession(buf)==1)
			{
				printf("Выключаем свервер...\n");
				shutdown(listener,2);
				printf("shutdown listener\n");
                                close(listener);
                                printf("close listener\n");
			}
			send(sock, "Ответ сервера: УСПЕШНО!\n", BUF_LEN, 0);
		}

		close(sock);
	}

	return 0;
}

int endSession(char *mesaga)
{
	char *zapros;
	zapros = "shutdownServer";
	if(strcmp(mesaga, zapros)==0)
	{
		return 1;
	}
	return 0;
}
