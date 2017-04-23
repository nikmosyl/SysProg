#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void sighandler_provider(int signum, siginfo_t *info, void *ptr);

void signal_int_handler();
void signal_usr1_handler();
void signal_usr2_handler();
void signal_sighup_handler();
void signal_sigquit_handler();
void signal_sigterm_handler();
void signal_sigkill_handler();

int main()
{
	printf("PID %lu\n", (unsigned long)getpid());

	struct sigaction act;

	act.sa_sigaction = sighandler_provider;
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGINT, &act, NULL);
	sigaction(SIGUSR1, &act, NULL);
	sigaction(SIGUSR2, &act, NULL);
	sigaction(SIGHUP, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGKILL, &act, NULL);

	printf("kill -s SIGUSR[номер] [PID]\n");

	while(1)
	{
	}

	return 0;
}

void sighandler_provider(int signum, siginfo_t *info, void *ptr)
{
	printf("Полученный сигнал %d\n", signum);
	printf("Сигнал пришёл от процесса %lu\n", (unsigned long)info->si_pid);
	if (signum == SIGINT)
	{
		signal_int_handler();
	}
	else if (signum == SIGUSR1)
	{
		signal_usr1_handler();
	}
	else if(signum == SIGUSR2)
	{
		signal_usr2_handler();
	}
	else if(signum == SIGHUP)
        {
                signal_sighup_handler();
        }
	else if(signum == SIGQUIT)
        {
                signal_sigquit_handler();
        }
	else if(signum == SIGTERM)
        {
                signal_sigterm_handler();
        }
	else
	{
                signal_sigkill_handler();
        }
}

void signal_int_handler()
{
	printf("сигнал Ctrl + C\n");
	exit(0);
}

void signal_usr1_handler()
{
	printf("Сигнал, определяемый пользователем. ОТПУСТИТЬ!\n");
	exit(0);
}

void signal_usr2_handler()
{
	printf("Сигнал, определяемый пользователем. НЕ ОТПУСКАТЬ!\n");
}

void signal_sighup_handler()
{
	printf("Сигнал HUP. ОТПУСТИТЬ!\n");
        exit(0);
}

void signal_sigquit_handler()
{
	printf("Сигнал QUIT. ОТПУСТИТЬ!\n");
        exit(0);
}

void signal_sigterm_handler()
{
	printf("Сигнал TERM. ОТПУСТИТЬ!\n");
        exit(0);
}

void signal_sigkill_handler()
{
	printf("Сигнал KILL. ОТПУСТИТЬ!\n");
        exit(0);
}
