#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_LEN 1024

int main(int argc, char* argv[])
{
	int sock;
	int portNumber;
        struct sockaddr_in addr;
        char buf[BUF_LEN];

	if (argc < 3)
	{
		printf("Введите сообщение и номер порта!\n");
		exit(-1);
	}

	if (argc != 3)
	{
		printf("Направильное количество аргументов\n");
		exit(-4);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock < 0)
	{
		perror("Сокет");
		exit(-2);
	}

	portNumber = atoi(argv[2]);
	printf("Вызываем порт номер %d\n", portNumber);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNumber);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("Соединение не удалось");
		exit(-3);
	}

	int msg_size = 0;

	while (argv[1][msg_size] != '\0')
	{
		msg_size++;
	}

	send(sock, argv[1], msg_size, 0);
	recv(sock, buf, BUF_LEN, 0);

	printf("%s", buf);
	close(sock);

	return 0;
}

