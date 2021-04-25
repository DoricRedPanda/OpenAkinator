#ifndef GAME_H
#define GAME_H

#define INIT_NUM_OF_CHARACTERS 128
#define INIT_NUM_OF_QUESTIONS 128
#define ACCURACY 0.9
#define INI_MULTIPLIER 8
#define DEFAULT_PERSONS_FILENAME "persons.txt"
#define DEFAULT_QUESTIONS_FILENAME "questions.txt"

struct Person {
	char *text;
	int popularity;
};

struct Question {
	char *text;
	/* number of user answered yes thinking of i-th character*/
	int *yes;
	/* same, except user answered no */
	int *no;
	/* conditional probability or evidence */
	double *prY;
	double *prN;
};

struct PersonArr {
	size_t len;
	struct Person *arr;
};

struct QuestionArr {
	size_t len;
	struct Question *arr;
};

enum GameResult {
	WIN, LOSE
};

typedef struct Person Person;
typedef struct Question Question;
typedef struct PersonArr PersonArr;
typedef struct QuestionArr QuestionArr;

void routine(char *personsFN, char *questionsFN);


#endif
