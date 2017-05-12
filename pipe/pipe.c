#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	// read <--- fd[0] |--<--pipe--<--| fd[1] <--- write
	int fd[2];
	pid_t pid;

	size_t size;
	char str[14];

	if(pipe(fd) < 0)
	{
		printf("Невозможно создать канал\n");
		exit(-1);
	}

	pid = fork();

	if (pid == -1)
	{
		perror("Error: ");
		exit(-1);
	}

	if (pid > 0)
	{
		int status;
		close(fd[0]);
		size = write(fd[1], "I'm PIPE!", 14);

		if (size != 14)
		{
			printf("PARENT: Невозможно записать всю строку\n");
			exit(-1);
		}

		printf("PARENT: Передача данных CHILDу произошла успешно!\n");
		waitpid(pid, &status, 2);

		if (WEXITSTATUS(status) == 1)
		{
			close(fd[1]);
			printf("PARENT: Готово!\n");
		}

		else
		{
			close(fd[1]);
			printf("PARENT: Ошибка CHILD процесса\n");
		}
	}
	else
	{
		close(fd[1]);

		size = read(fd[0], str, 14);

		if(size < 0)
		{
			printf("--CHILD: Не удаётся прочитать строку\n"); 
			exit(-1);
		}

		printf("--CHILD: получено от PARENTа по каналу: ");
		printf("%s\n", str);

		close(fd[0]);
		exit(1);
	}
	return 0;
}

