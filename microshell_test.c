#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#ifdef TEST_SH
# define TEST		1
#else
# define TEST		0
#endif

typedef struct s_list
{
	char	**argv;
	int		pipe[2];
	int		type[2];
	int		prev_pipe;
}	t_list;

int	ft_strlen(char *str)
{
	int len;

	len = 0;
	if (!str)
		return (len);
	while (str[len])
		len++;
	return (len);
}

void	fatal_msg(void)
{
	write(STDERR_FILENO, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(EXIT_FAILURE);
}

int	cmd_len(t_list *cmd, int begin, char *argv[])
{
	int	end;
	int	len;

	end = begin;
	while (argv[end] && strcmp("|", argv[end]) && strcmp(";", argv[end]))
		end++;
	len = end - begin;
	if (len > 0)
	{
		cmd->argv = &argv[begin];
		cmd->type[0] = cmd->type[1];
		if (!argv[end])
			cmd->type[1] = 0;
		if (strcmp("|", argv[end]) == 0)
			cmd->type[1] = 1;
		else
			cmd->type[1] = 2;
		argv[end] = NULL;
		cmd->prev_pipe = cmd->pipe[0];
	}
	return (len);
}

void	execute_cmd(t_list *cmd, char *envp[])
{

}

int	main(int argc, char *argv[], char *envp[])
{
	t_list	cmd;
	int		it;
	int		begin;

	it = 0;
	while (it < argc && argv[++it])
	{
		begin = it;
		it = cmd_len(&cmd, it, argv);
		if (!strcmp("cd", argv[begin]))
		{
			if (it - begin != 2)
				write(STDERR_FILENO, "error: cd: cannot change directory to ", \
				ft_strlen("error: cd: cannot change directory to "));
			write(STDERR_FILENO, cmd.argv[1], ft_strlen(cmd.argv[1]));
			write(STDERR_FILENO, "\n", 1);
		}
		else if (it > begin)
			execute_cmd(&cmd, envp);
	}
	if (TEST)
		while (1);
	return (0);
}