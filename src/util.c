#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#include "util.h"

int
getAnswer()
{
	int ch, end;
	do {
		printf("(y/n)> ");
		ch = getchar();
		if (ch == 'y' || ch == 'n' || ch == '?') {
			end = getchar();
			if (end == '\n' || end == EOF)
				break;
		} else {
			while (ch != '\n' && ch != EOF)
				ch = getchar();
		}
	} while (ch != EOF);
	return ch;
}

char
askConfirmation(const char *text)
{
	puts(text);
	return getAnswer() == 'y';
}

char*
getText(FILE *file)
{
	int allocated = DEFAULT_TEXT_LENGTH * sizeof(char);
	int i = 0;
	char *text;
	int ch;

	text = wrp_malloc(allocated);
	for (;;) {
		ch = fgetc(file);
		if (ch == EOF)
			break;
		if (ch == '\n') {
			if (!i)
				continue;
			else
				break;
		}
		text[i] = ch;
		i++;
		if (i >= allocated) {
			allocated += sizeof(char);
			text = realloc(text, allocated);
		}
	}
	if (!i) {
		free(text);
		errx(EXIT_FAILURE, "BAD INPUT");
	}
	text[i] = '\0';
	return text;
}

void
backupFile(const char *filename)
{
	char *backupName;
	backupName = wrp_malloc(_POSIX_ARG_MAX * sizeof(char));
	snprintf(backupName, _POSIX_ARG_MAX, "%s.old", filename);
	rename(filename, backupName);
	free(backupName);
}

void*
wrp_malloc(size_t size)
{
	void *ptr;
	ptr = malloc(size);
	if (!ptr)
		err(EXIT_FAILURE, "failed to allocate memory");
	return ptr;
}

void*
wrp_realloc(void *ptr, size_t size)
{
	void *newptr;
	newptr = realloc(ptr, size);
	if (!ptr) {
		free(ptr);
		err(EXIT_FAILURE, "failed to allocate memory");
	}
	return newptr;
}

FILE*
wrp_fopen(const char *fn, const char *option)
{
	FILE *file;

	file = fopen(fn, option);
	if (!file)
		err(EXIT_FAILURE, "Failed to open file %s", fn);
	return file;
}

