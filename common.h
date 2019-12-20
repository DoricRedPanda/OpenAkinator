#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#define INIT_NUM_OF_CHARACTERS 128
#define INIT_NUM_OF_QUESTIONS 128
#define DEFAULT_TEXT_LENGTH 1
#define _ACCURACY 0.9

struct Character {
	char *text;
	int popularity;
};

struct Question {
	char *text;
	int *n_yes; /* n_yes[i] - number of user answered yes to i-th question*/
	int *n_no;  /* same, except user answers no */
	int *n_total;
	double *l_y;
	double *l_n;
};

typedef struct Character Character;
typedef struct Question Question;

void restore_io(int saved, int cur);
char getAnswer();
char* getText();
char askConfirmation(char *text);
void backupFile(char *filename);
void redir_io(char *fn, int old);
void normalize(double *odds, int n);
