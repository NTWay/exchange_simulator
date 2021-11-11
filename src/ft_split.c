#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void ft_putstr(char *s)
{
	while (*s)
		write(1, s++, 1);
}

int ft_is_space(char *str, char sep)
{
	int count;

	count = 0;
	while ((*str == '\t' || *str == ' ' || *str == sep) && *str)
	{
		count++;
		str++;
	}
	return (count);
}

int word_count(char *str, char sep)
{
	int count = 0;

	while (*str && *str != '\t' && *str != ' ' && *str != sep)
	{
		str++;
		count++;
	}
	return count;
}

int count_of_words(char *s, char sep)
{
	int count;

	count = 0;
	s += ft_is_space(s, sep);
	while (*s)
	{
		if (word_count(s, sep))
		{
			s += word_count(s, sep);
			count++;
		}
		else
			s += ft_is_space(s, sep);
	}
	return (count);
}

char *ft_cpy_word(char *dest, char *s, int n)
{
	int i;

	i = 0;
	while (*s && i < n)
	{
		dest[i] = s[i];
		++i;
	}
	dest[i] = '\0';
	return (dest);
}

char **ft_split(char *s, char sep)
{
	char **res;
	char *word;
	int i;

	i = 0;
	res = malloc(sizeof(word) * count_of_words(s, sep) + 1);
	s += ft_is_space(s, sep);
	while(*s)
	{
		if (word_count(s, sep))
		{
			word = malloc(sizeof(char) * word_count(s, sep) + 1);
			word = ft_cpy_word(word, s, word_count(s, sep));
			res[i] = word; 
			i++;
			s += word_count(s, sep);
		}
		else
			s += ft_is_space(s, sep);
	}
	res[i] = NULL;
	return (res);
}
/*
int main(int argc, char **argv)
{
	char **str;
	char sep = ',';

	if (argc == 2)
		str = ft_split(argv[1], sep);
	
	int i = 0;
	while (str[i])
	{
		ft_putstr(str[i++]);
		write(1, "\n", 1);
	}
	return 0;
}
*/
