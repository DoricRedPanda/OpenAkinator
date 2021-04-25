#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "game.h"

static char *g_personsFN;
static char *g_questionsFN;
static const char msg_new_files[] = "Do you want to create new files: "
                                    DEFAULT_PERSONS_FILENAME
                                    " and "
                                    DEFAULT_QUESTIONS_FILENAME
                                    " in this directory?";
static const char msg_usage[] = "Usage: %s "
                                 DEFAULT_PERSONS_FILENAME
                                " "
                                 DEFAULT_QUESTIONS_FILENAME;

static void
init(int argc, char **argv)
{
	FILE *file;
	if (argc == 3) {
		g_personsFN = argv[1];
		g_questionsFN = argv[2];
	} else if (argc == 1) {
		if (askConfirmation(msg_new_files)) {
			file = wrp_fopen(DEFAULT_PERSONS_FILENAME, "w");
			fputs("0\n", file);
			fclose(file);
			g_personsFN = DEFAULT_PERSONS_FILENAME;
			file = wrp_fopen(DEFAULT_QUESTIONS_FILENAME, "w");
			fputs("0\n", file);
			fclose(file);
			g_questionsFN = DEFAULT_QUESTIONS_FILENAME;
		} else {
			errx(EXIT_FAILURE, msg_usage, argv[0]);
		}
	} else {
		errx(EXIT_FAILURE, msg_usage, argv[0]);
	}
}

int
main(int argc, char **argv)
{
	init(argc, argv);
	routine(g_personsFN, g_questionsFN);
	return 0;
}
