#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>															// for Linux
#include <sys/types.h>															// for Linux

#ifdef TEST_SH
# define TEST		1
#else
# define TEST		0
#endif

int	ft_strlen(char *str)
{
	int	len;

	len = 0;
	if (!str)
		return (len);
	while (str[len])
		len++;
	return (len);
}

void	exit_fatal(void)
{
	write(2, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(EXIT_FAILURE);
}

int	cmd_len(char **arg)
{
	int	len;

	len = 0;
	while (arg[len] && strcmp(arg[len], ";") != 0 && strcmp(arg[len], "|") != 0)
		len++;
	return (len);
}

int	execute_cd(char **path)
{
	int	go_to;

	if (path[1] && path[2] == NULL)
	{
		go_to = chdir(path[1]);
		if (go_to == -1)
		{
			write(2, "error: cd: cannot change directory to ", ft_strlen("error: cd: cannot change directory to "));
			write(2, path[1], ft_strlen(path[1]));
			write(2, "\n", 1);
		}
	}
	else
		write(2, "error: cd: bad arguments\n", ft_strlen("error: cd: bad arguments\n"));
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	int		it = 1;
	int		begin;
	char	**cmd;
	int		len;
	int		opened_pipe = 0;
	int		pipeFD[2];
	int		savedFD_0 = dup(0);
	int		savedFD_1 = dup(1);
	pid_t	pid;

	while (it < argc)
	{
		if (strcmp(argv[it], ";") == 0 || strcmp(argv[it], "|") == 0)			// пропускаем, если ; или |
			it++;
		else
		{
			len = cmd_len(&argv[it]);										// считаем сколько элементов записать в подстроку с аргументами
			if (!(cmd = (char **) malloc(sizeof(char *) * (len + 1))))		// память под указатели на аргументы
				exit_fatal();
			cmd[len] = NULL;
			begin = it;
			while (argv[it] && strcmp(argv[it], ";") != 0 && strcmp(argv[it], "|") != 0)
			{
				cmd[it - begin] = argv[it];										// переписываем указатели на аргументы
				it++;
			}
			if (opened_pipe)													// если был открыт пайп
			{
				dup2(pipeFD[0], 0);
				close(pipeFD[0]);
			}
			if (argv[it] && strcmp(argv[it], "|") == 0)							// если нужно открыть пайп
			{
				if (pipe(pipeFD) == -1)
					exit_fatal();
				dup2(pipeFD[1], 1);
				close(pipeFD[1]);
			}
			if (strcmp(cmd[0], "cd") == 0)										// если встретили команду cd
				execute_cd(cmd);
			else
			{
				if ((pid = fork()) < 0)											// если ошибка
					exit_fatal();
				if (pid == 0)													// дочерний процесс
				{
					if (execve(cmd[0], cmd, envp) == -1)				// исполняем команду
					{
						write(2, "error: cannot execute ", ft_strlen("error: cannot execute "));
						write(2, cmd[0], ft_strlen(cmd[0]));
						write(2, "\n", 1);
						free(cmd);												// избавляемся от утечек
						return (0);
					}
				}
				waitpid(pid, &begin, 0);										// родительский процесс
			}
			if (opened_pipe)													// закрываем часть пайпа, если он был открыт в прошлой команде
			{
				dup2(savedFD_0, 0);
				opened_pipe = 0;
			}
			if (argv[it] && strcmp(argv[it], "|") == 0)							// закрываем часть пайпа, если открывали сейчас
			{
				dup2(savedFD_1, 1);
				opened_pipe = 1;
			}
			free(cmd);															// избавляемся от утечек
		}
	}
	if (TEST)
		while (1);
	return (0);
}