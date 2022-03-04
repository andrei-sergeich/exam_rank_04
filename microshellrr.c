#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#ifdef TEST_SH
# define TEST		1
#else
# define TEST		0
#endif

typedef struct s_cmd
{
	char **args;
	int	pipe[2];
	int prev_pipe;
	int type[2];
}	t_cmd;

int	ft_strlen(char *s)
{
	int i;

	i = 0;
	if (!s)
		return (i);
	while (s[i])
		i++;
	return (i);
}

int	lst_len(t_cmd *lst, int start, char **argv)
{
	int end;
	int len;

	end = start;
	while (argv[end] && strcmp(argv[end], ";") && strcmp(argv[end], "|"))
		end++;
	len = end - start;
	if (len > 0)
	{
		lst->args = &argv[start];
		lst->type[0] = lst->type[1];
		if (!argv[end])		 //конец строки
			lst->type[1] = 0;
		else if (!strcmp(argv[end], "|"))
			lst->type[1] = 1;
		else
			lst->type[1] = 2; // ";"
		argv[end] = NULL;	// NULL для разделения двумерного массива
		lst->prev_pipe = lst->pipe[0];
	}
	return (end);
}

void fatal()
{
	write(2, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(1);
}

void	exec_cmd(t_cmd *lst, char **envp)
{
	pid_t	pid;
	int		ret;

	if ((lst->type[1] == 1 || lst->type[0] == 1) && (pipe(lst->pipe)))
		fatal();
	if ((pid = fork()) < 0)
		fatal();
	if (pid == 0)
	{
		if ((lst->type[1] == 1) && (dup2(lst->pipe[1], 1)) < 0)
			fatal();
		if ((lst->type[0] == 1) && (dup2(lst->prev_pipe, 0)) < 0)
			fatal();
		if ((ret = execve(lst->args[0], lst->args, envp)) < 0)
		{
			write(2, "error: cannot execute ", 22);
			write(2, lst->args[0], ft_strlen(lst->args[0]));
			write(2, "\n", 1);
		}
		exit(ret);
	}
	else
	{
		waitpid(pid, 0, 0);
		if (lst->type[1] == 1 || lst->type[0] == 1)
		{
			close(lst->pipe[1]);
			if (lst->type[1] != 1)
				close(lst->pipe[0]);
		}
		if (lst->type[0] == 1)
			close(lst->prev_pipe);
	}
}

int	main(int argc, char **argv, char **env)
{
	t_cmd	lst;
	int		i;
	int		start;

	i = 0;
	while (i < argc && argv[++i])
	{
		start = i;
		i = lst_len(&lst, i, argv);
		if (!strcmp(argv[start], "cd"))
		{
			if (i - start != 2)
				write(2, "error: cd: cannot change directory to ", \
				ft_strlen("error: cd: cannot change directory to "));
			write(2, lst.args[1], ft_strlen(lst.args[1]));
			write(2, "\n", 1);
		}
		else if (i > start)
			exec_cmd(&lst, env);
	}
	if (TEST)
		while (1);
	return (0);
}

