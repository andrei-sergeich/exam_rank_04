#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

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
		if (strcmp(argv[it], ";") == 0 || strcmp(argv[it], "|") == 0)
			it++;
		else
		{
			len = cmd_len(&argv[it]);
			if (!(cmd = (char **) malloc(sizeof(char *) * (len + 1))))
				exit_fatal();
			cmd[len] = NULL;
			begin = it;
			while (argv[it] && strcmp(argv[it], ";") != 0 && strcmp(argv[it], "|") != 0)
			{
				cmd[it - begin] = argv[it];
				it++;
			}
			if (opened_pipe)
			{
				dup2(pipeFD[0], 0);
				close(pipeFD[0]);
			}
			if (argv[it] && strcmp(argv[it], "|") == 0)
			{
				if (pipe(pipeFD) == -1)
					exit_fatal();
				dup2(pipeFD[1], 1);
				close(pipeFD[1]);
			}
			if (strcmp(cmd[0], "cd") == 0)
				execute_cd(cmd);
			else
			{
				if ((pid = fork()) < 0)
					exit_fatal();
				if (pid == 0)
				{
					if (execve(cmd[0], cmd, envp) == -1)
					{
						write(2, "error: cannot execute ", ft_strlen("error: cannot execute "));
						write(2, cmd[0], ft_strlen(cmd[0]));
						write(2, "\n", 1);
						free(cmd);
						return (0);
					}
				}
				waitpid(pid, &begin, 0);
			}
			if (opened_pipe)
			{
				dup2(savedFD_0, 0);
				opened_pipe = 0;
			}
			if (argv[it] && strcmp(argv[it], "|") == 0)
			{
				dup2(savedFD_1, 1);
				opened_pipe = 1;
			}
			free(cmd);
		}
	}
	return (0);
}