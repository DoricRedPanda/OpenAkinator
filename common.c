#include "common.h"

void redir_io(char *fn, int old)
{
	int fd;
	if (!old) {
		if ((fd = open(fn, O_RDONLY)) < 0) {
			perror("(EE) open() failed: ");
			exit(EXIT_FAILURE);
		}
	} else {
		if ((fd = open(fn, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0) {
			perror("(EE) open() failed: ");
			exit(EXIT_FAILURE);
		}
	}
	if (dup2(fd, old) < 0) {
		perror("dup2: ");
		exit(EXIT_FAILURE);
	}
	ungetc('\n', stdin);
	getchar();
	close(fd);
}

void restore_io(int saved, int cur) {
	if (dup2(saved, cur) < 0) {
		perror("dup2: ");
		exit(EXIT_FAILURE);
	}
	close(saved);
	ungetc('\n', stdin);
	getchar();
}

char getAnswer()
{
	char ch;
	while ((ch = getchar()) != 'y' && ch != 'n' &&
			ch != '?' && ch != EOF);
	getchar();
	return ch;
}

char askConfirmation(char *text)
{
	puts(text);
	printf("(y/n)? ");
	return (getAnswer() == 'y');

}

char* getText()
{
	int allocated = DEFAULT_TEXT_LENGTH * sizeof(char);
	int i = 0;
	char *text = malloc(allocated);
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF) {
		text[i++] = ch;
		if (i + 2 >= allocated) {
			allocated += sizeof(char);
			text = realloc(text, allocated);
		}
	}
	if (!i) {
		free(text);
		return NULL;
	}
	text[i] = '\0';
	return text;
}

void backupFile(char *filename)
{
	char *newfn = malloc(_POSIX_ARG_MAX * sizeof(char));
	snprintf(newfn, _POSIX_ARG_MAX, "%s.old", filename);
	rename(filename, newfn);
	free(newfn);
}
