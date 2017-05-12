#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void main()
{
	pid_t pid;
	int signal;

	pid = fork();
	if (pid == -1)
	{
		perror("Ошибка: ");
		exit(-1);
	}

	if (pid > 0)
	{
		printf("PARENT: Это PARENT процесс\n");
		printf("PARENT: PID: %d\n", getpid());
		printf("PARENT: CHILD's PID: %d\n", pid);
		printf("PARENT: Ожидание сигнала от CHILD...\n");
		waitpid(pid, &signal, 2);
		printf("PARENT: Получено от CHILD: %d\n", WEXITSTATUS(signal));
		printf("PARENT: Выход!\n");
		exit(1);
	}

	if (pid == 0)
	{
		printf("--CHILD: Это CHILD процесс\n");
		printf("--CHILD: CHILD PID: %d\n", getpid());
		printf("--CHILD: Мой PARENT's PID: %d\n", getppid());
		printf("--CHILD: Введите мой код возврата: ");
		scanf("%d", &signal);
		printf("--CHILD: Выход!\n");
		exit(signal);
	}
}
