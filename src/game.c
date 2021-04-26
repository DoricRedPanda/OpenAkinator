#include <math.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "game.h"

PersonArr
getPersons(const char *filename)
{
	FILE *file;
	PersonArr prsn;

	file = wrp_fopen(filename, "r");
	fscanf(file, "%lu\n", &(prsn.len));
	prsn.arr = wrp_malloc(prsn.len * sizeof(Person));
	for (size_t i = 0; i < prsn.len; i++) {
		prsn.arr[i].text = getText(file);
		fscanf(file, "%d\n", &(prsn.arr[i].popularity));
	}
	fclose(file);
	return prsn;
}

QuestionArr
getQuestions(const char *filename, size_t pNum)
{
	FILE *file;
	QuestionArr qst;

	file = wrp_fopen(filename, "r");
	fscanf(file, "%lu\n", &(qst.len));
	qst.arr = wrp_malloc(qst.len * sizeof(Question));
	for (size_t i = 0; i < qst.len; i++) {
		qst.arr[i].text = getText(file);
		qst.arr[i].yes = wrp_malloc(pNum * sizeof (int));
		qst.arr[i].no = wrp_malloc(pNum * sizeof (int));
		qst.arr[i].prY = wrp_malloc(pNum * sizeof (double));
		qst.arr[i].prN = wrp_malloc(pNum * sizeof (double));
		for (size_t j = 0; j < pNum; j++)
			fscanf(file, "%d ", &(qst.arr[i].yes[j]));
		for (size_t j = 0; j < pNum; j++)
			fscanf(file, "%d ", &(qst.arr[i].no[j]));
	}
	fclose(file);
	return qst;
}

void
printOdds(double *odds, size_t n)
{
	printf("Odds: ");
	for (size_t i = 0; i < n; i++)
		printf("%lf ", odds[i]);
	putchar('\n');
}

void
normalize(double *odds, size_t n)
{
	double div = 0.0;
	for (size_t i = 0; i < n; i++)
		div += odds[i];
	for (size_t i = 0; i < n; i++)
		odds[i] /= div;
}

void
getPriorOdds(const PersonArr prsn, double *odds)
{
	for (size_t i = 0; i < prsn.len; i++)
		odds[i] = prsn.arr[i].popularity;
	normalize(odds, prsn.len);
#ifdef DEBUG
	printOdds(odds, prsn.len);
#endif
}

void
updateBias(double *odds, Question q, size_t n, char flag)
{
	double *likelihood;
	switch (flag) {
		case 'y':
			likelihood = q.prY;
			break;
		case 'n':
			likelihood = q.prN;
			break;
		default:
			return;

	}
	for (size_t i = 0; i < n; i++)
		odds[i] *= likelihood[i];
	normalize(odds, n);
}

long
findLikeliest(const double *odds, long n)
{
	long id = -1;
	double max = 0.0;

	for (long i = 0; i < n; i++)
		if (odds[i] > max) {
			id = i;
			max = odds[i];
		}
	return id;
}

void
learn(PersonArr prsn, QuestionArr qst, size_t idP, const char *answer)
{
	char ans;
	Question *arr = qst.arr;

	prsn.arr[idP].popularity++;
	for (size_t i = 0; i < qst.len; i++) {
		ans = answer[i];
		arr[i].no[idP] += (ans == 'n') | (ans == '?');
		arr[i].yes[idP] += (ans == 'y') | (ans == '?');
	}
}

void
calcLikelihood(QuestionArr qst, size_t pNum)
{
	Question *arr = qst.arr;

	for (size_t i = 0; i < qst.len; i++) {
		for (size_t j = 0; j < pNum; j++) {
			arr[i].prY[j] = arr[i].yes[j];
			arr[i].prY[j] /= arr[i].yes[j] + arr[i].no[j];
			arr[i].prN[j] = 1 - arr[i].prY[j];
		}
	}
}

double
getEntropy(double *odds, double *likelyhood, size_t n)
{
	double entropy = 0.0, tmp;
	for (size_t i = 0; i < n; i++) {
		tmp = odds[i] * likelyhood[i];
		if (tmp > 0)
			entropy += tmp * log2(tmp);
	}
	return -entropy;
}

double
getAnsProb(double *odds, double *likelyhood, size_t n)
{
	double prob = 0.0;
	for (size_t i = 0; i < n; i++)
		prob += odds[i] * likelyhood[i];
	return prob;
}

long
chooseQuestion(QuestionArr qst, double *odds, size_t pNum, char *ans)
{
	long id = -1;
	double minEntropy = INFINITY, entropy;
	Question *arr = qst.arr;

	for (size_t i = 0; i < qst.len; i++) {
		if (ans[i])
			continue;
		entropy = getEntropy(odds, arr[i].prY, pNum) *
			getAnsProb(odds, arr[i].prY, pNum);
		entropy += getEntropy(odds, arr[i].prN, pNum) *
			getAnsProb(odds, arr[i].prN, pNum);
		if (entropy < minEntropy) {
			minEntropy = entropy;
			id = i;
		}
#ifdef DEBUG
		printf("Entropy for question %lu = %lf\n", i, entropy);
#endif
	}
	return id;
}

char
guessPerson(PersonArr prsn, QuestionArr qst,
            size_t idP, char *ans, double *odds)
{
#ifdef DEBUG
	printf("(p = %lf)", odds[idP]);
#endif
	puts(prsn.arr[idP].text);
	if (askConfirmation("Am I right?")) {
		learn(prsn, qst, idP, ans);
		return 1;
	}
	odds[idP] = 0.0;
	return 0;
}

void
savePersons(const char *filename, PersonArr prsn)
{
	FILE *file;

	file = wrp_fopen(filename, "w");
	fprintf(file, "%lu\n", prsn.len);
	for (size_t i = 0; i < prsn.len; i++) {
		fputs(prsn.arr[i].text, file);
		fputc('\n', file);
		fprintf(file,"%d\n", prsn.arr[i].popularity);
	}
	fclose(file);
}

void
saveQuestions(const char *filename, QuestionArr qst, size_t pNum)
{
	FILE *file;

	file = wrp_fopen(filename, "w");
	fprintf(file, "%lu\n", qst.len);
	for (size_t i = 0; i < qst.len; i++) {
		fputs(qst.arr[i].text, file);
		fputc('\n', file);
		for (size_t j = 0; j < pNum; j++)
			fprintf(file, "%d ", qst.arr[i].yes[j]);
		fputc('\n', file);
		for (size_t j = 0; j < pNum; j++)
			fprintf(file, "%d ", qst.arr[i].no[j]);
		fputc('\n', file);
	}
	fclose(file);
}

PersonArr
insertPerson(PersonArr prsn, QuestionArr qst, const char *ans) {
	Question *arr;
	size_t newsize;

	prsn.arr = wrp_realloc(prsn.arr, (prsn.len + 1) * sizeof(Person));
	printf("Enter your person's name: ");
	prsn.arr[prsn.len].text = getText(stdin);
	prsn.arr[prsn.len].popularity = 1;
	arr = qst.arr;
	newsize = (prsn.len + 1) * sizeof(int);
	for (size_t i = 0; i < qst.len; i++) {
		arr[i].yes = wrp_realloc(arr[i].yes, newsize);
		arr[i].no = wrp_realloc(arr[i].no, newsize);
		arr[i].yes[prsn.len] = 1 + INI_MULTIPLIER * (ans[i] == 'y');
		arr[i].no[prsn.len] = 1 + INI_MULTIPLIER * (ans[i] == 'n');
	}
	prsn.len++;
	return prsn;
}

QuestionArr
insertQuestion(QuestionArr qst, size_t pNum, size_t idP) {
	int ch;
	Question *arr;

	qst.arr = wrp_realloc(qst.arr, (qst.len + 1) * sizeof(Question));
	arr = qst.arr;
	printf("Your question: ");
	arr[qst.len].text = getText(stdin);
	arr[qst.len].yes = wrp_malloc(pNum * sizeof(int));
	arr[qst.len].no = wrp_malloc(pNum * sizeof(int));
	arr[qst.len].prY = wrp_malloc(pNum * sizeof(double));
	arr[qst.len].prN = wrp_malloc(pNum * sizeof(double));
	printf("Answer for your person: ");
	ch = getAnswer();
	for (size_t i = 0; i < pNum; i++) {
		arr[qst.len].yes[i] = 1;
		arr[qst.len].no[i] = 1;
	}
	arr[qst.len].yes[idP] += INI_MULTIPLIER * (ch == 'y');
	arr[qst.len].no[idP] += INI_MULTIPLIER * (ch == 'n');
	qst.len++;
	return qst;
}

long
game(PersonArr prsn, QuestionArr qst, double *odds, char *ans)
{
	long idQ, idP;
	char isRight;

	memset(ans, 0, qst.len * sizeof(char));
	getPriorOdds(prsn, odds);
	calcLikelihood(qst, prsn.len);
	do {
		idQ = chooseQuestion(qst, odds, prsn.len, ans);
		if (idQ >= 0) {
			puts(qst.arr[idQ].text); /* write question text */
			ans[idQ] = getAnswer();
			updateBias(odds, qst.arr[idQ], prsn.len, ans[idQ]);
		}
#ifdef DEBUG
		printOdds(odds, prsn.len);
#endif
		idP = findLikeliest(odds, prsn.len);
		if (idP < 0)
			return -1;
		if (odds[idP] > ACCURACY || idQ < 0) {
			isRight = guessPerson(prsn, qst, idP, ans, odds);
			if (isRight)
				return idP;
		}
	} while (idQ >= 0 || askConfirmation("Do you want to continue?"));
	return -1;
}


void
freePersons(PersonArr prsn)
{
	for (size_t i = 0; i < prsn.len; i++)
		free(prsn.arr[i].text);
	free(prsn.arr);
}

void
freeQuestions(QuestionArr qst)
{
	for (size_t i = 0; i < qst.len; i++) {
		free(qst.arr[i].text);
		free(qst.arr[i].yes);
		free(qst.arr[i].no);
		free(qst.arr[i].prY);
		free(qst.arr[i].prN);
	}
	free(qst.arr);
}

void
routine(char *personsFN, char *questionsFN)
{
	PersonArr prsn;
	QuestionArr qst;
	double *odds;
	char *ans;
	long idP;

	prsn = getPersons(personsFN);
	qst = getQuestions(questionsFN, prsn.len);
	odds = wrp_malloc(prsn.len * sizeof(double));
	ans = wrp_malloc(qst.len * sizeof(char));
	idP = game(prsn, qst, odds, ans);
	if (idP < 0) {
		puts("You win!");
		if (askConfirmation("Do you want to add your person?")) {
			prsn = insertPerson(prsn, qst, ans);
			if (askConfirmation("Do you want to add new question?"))
				qst = insertQuestion(qst, prsn.len, prsn.len - 1);
		}
	} else if (odds[idP] < ACCURACY) {
		if (askConfirmation("Do you want to add new question?"))
			qst = insertQuestion(qst, prsn.len, idP);
	}
	savePersons(personsFN, prsn);
	saveQuestions(questionsFN, qst, prsn.len);
	free(ans);
	free(odds);
	freeQuestions(qst);
	freePersons(prsn);
}

