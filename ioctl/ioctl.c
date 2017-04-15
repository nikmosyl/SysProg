#include "chardev.h"
#include "string.h"

#include <fcntl.h>      					//открыть
#include <unistd.h>     					//выйти
#include <sys/ioctl.h>						//работа с ioctl
#include <stdlib.h>
#include <stdio.h>

//Работа с драйвером через ioctl получение сообщения
ioctl_set_msg(int file_desc, char *message)
{
	int ret_val;

	ret_val = ioctl(file_desc, IOCTL_SET_MSG, message);

	if (ret_val < 0)
	{
		printf("Ошибка при вызове ioctl_set_msg: %d\n", ret_val);
		exit(-1);
	}
}

//отправка сообщения процессу
ioctl_get_msg(int file_desc)
{
	int ret_val;
	char message[100];
	ret_val = ioctl(file_desc, IOCTL_GET_MSG, message);

	if (ret_val < 0)
	{
		printf("Ошибка при вызове ioctl_get_msg: %d\n", ret_val);
		exit(-1);
	}

	if(strlen(message)<=0)
	{
		printf("Сообщений ещё не поступало!\n");
	}
	else
	{
		printf("В файле храниться следующее: %s\n", message);
	}
}


void main(int argc, char* argv[])
{

	int selection;
	int file_desc;
	char mesagaIN[100];
	char *msg = "Это сообщение передается через ioctl\n";

	file_desc = open(DEVICE_FILE_NAME, 0);

	if (file_desc < 0)
	{
		printf("Невозможно открыть файл устройства: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}

	printf("Добавить сообщение в файл   - 1\nПоказать сообщение из файла - 2\n");
	scanf("%d", &selection);

	switch(selection)
	{
		case 1:
			printf("Введите сообщение для mydev: ");
			scanf("%s", mesagaIN);
			ioctl_set_msg(file_desc, mesagaIN);
		break;

		case 2:
			if(1==0)
			{
				printf("Сообщений ещё не поступало\n");
			}
			else
			{
				ioctl_get_msg(file_desc);
			}
		break;
	}

	close(file_desc);
}
