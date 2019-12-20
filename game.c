#include "common.h"
#include <math.h>

Character* getCharacters(int n_chr)
{
	Character *character = malloc(n_chr * sizeof (Character));

	for (int i = 0; i < n_chr; i++) {
		character[i].text = getText();
		scanf("%d\n", &(character[i].popularity));
	}
	return character;
}

Question* getQuestions(int n_qst, int n_chr)
{
	Question *question = malloc(n_qst * sizeof (Question));
	
	for (int i = 0; i < n_qst; i++) {
		question[i].text = getText();
		question[i].n_yes = malloc(n_chr * sizeof (int));
		question[i].n_no = malloc(n_chr * sizeof (int));
		question[i].n_total = malloc(n_chr * sizeof (int));
		question[i].l_y = malloc(n_chr * sizeof (double));
		question[i].l_n = malloc(n_chr * sizeof (double));
		for (int j = 0; j < n_chr; j++)
			scanf("%d", &(question[i].n_yes[j]));
		getchar();
		for (int j = 0; j < n_chr; j++)
			scanf("%d", &(question[i].n_no[j]));
		getchar();
		for (int j = 0; j < n_chr; j++)
			scanf("%d", &(question[i].n_total[j]));
		getchar();
	}
	return question;
}

void init(int argc, char **argv, Character **chr, Question **qst, int *n_chr, int *n_qst)
{
	int input;

	if (argc != 3) {
		puts("(EE) Wrong usage");
		exit(EXIT_FAILURE);
	}

	input = dup(0);
	redir_io(argv[1], 0);
	scanf("%d\n", n_chr);
	*chr = getCharacters(*n_chr);
	redir_io(argv[2], 0);
	scanf("%d\n", n_qst);
	*qst = getQuestions(*n_qst, *n_chr);
	restore_io(input, 0);
}

double* getPriorOdds(Character *character, double *odds, int n)
{
	for (int i = 0; i < n; i++)
		odds[i] = character[i].popularity;
	return odds;
}

void updateBias(double *odds, Question q, int n, char flag)
{
	double *likelihood;
	switch (flag) {
		case 'y':
			likelihood = q.l_y;
			break;
		case 'n':
			likelihood = q.l_n;
			break;
		default:
			return;

	}
	for (int i = 0; i < n; i++)
		odds[i] *= likelihood[i];
	normalize(odds, n);
}

void normalize(double *odds, int n_chr)
{
	double div = 0.0;
	for (int i = 0; i < n_chr; i++)
		div += odds[i];
	for (int i = 0; i < n_chr; i++)
		odds[i] /= div;
}

int mostLikelyChr(double *odds, int n_chr)
{
	int id = -1;
	double max = 0.0;
	for (int i = 0; i < n_chr; i++)
		if (odds[i] > max) {
			id = i;
			max = odds[i];
		}
	return id;
}

void learn(Character *chr, Question *qst, int n_qst, int chr_id, char * answer)
{
	chr[chr_id].popularity++;
	for (int i = 0; i < n_qst; i++) {
		qst[i].n_total[chr_id]++;
		if (answer[i] == 'n')
			qst[i].n_no[chr_id]++;
		else if (answer[i] == 'y')
			qst[i].n_yes[chr_id]++;
		else if (answer[i] == '?') {
			qst[i].n_no[chr_id]++;
			qst[i].n_yes[chr_id]++;
			qst[i].n_total[chr_id]++;
		}
	}
}

void calcLikelihood(Question *qst, int n_qst, int n_chr)
{
	for (int i = 0; i < n_qst; i++) {
		for (int j = 0; j < n_chr; j++) {
			qst[i].l_y[j] = ((double) qst[i].n_yes[j]) / qst[i].n_total[j];
			qst[i].l_n[j] = ((double) qst[i].n_no[j]) / qst[i].n_total[j];
		}
	}
}

double getEntropy(double *odds, double *likelyhood, int n)
{
	double entropy = 0.0, tmp;
	for (int i = 0; i < n; i++) {
		tmp = odds[i] * likelyhood[i];
		entropy += tmp * log2(tmp);
	}
	return -entropy;
}

double getAnsProb(double *odds, double *likelyhood, int n)
{
	double prob = 0.0;
	for (int i = 0; i < n; i++)
		prob += odds[i] * likelyhood[i];
	return prob;
}

int
chooseQuestion(Question *qst, double *odds, int n_qst, int n_chr, char *ans)
{
	int id = -1;
	double minEntropy = INFINITY, entropy;
	for (int i = 0; i < n_qst; i++) {
		if (ans[i])
			continue;
		entropy = getEntropy(odds, qst[i].l_y, n_chr) *
			getAnsProb(odds, qst[i].l_y, n_chr);
		entropy += getEntropy(odds, qst[i].l_n, n_chr) *
			getAnsProb(odds, qst[i].l_n, n_chr);
		if (entropy < minEntropy) {
			minEntropy = entropy;
			id = i;
		}
		//printf("%lf\n", entropy);
	}
	return id;
}

void printOdds(double *odds, int n)
{
	for (int i = 0; i < n; i++)
		printf("%lf ", odds[i]);
	puts("");
}

int
guessChr(Character *chr, Question *qst,
         int n_qst, int chr_id, char *ans, double *odds, int n_chr)
{
	puts(chr[chr_id].text);
	if (askConfirmation("Am I right?")) {
		learn(chr, qst, n_qst, chr_id, ans);
		return 1;
	}
	odds[chr_id] = 0.0;
	//printOdds(odds, n_chr);
	return 0;
}

void saveCharacters(char *fn, Character *chr, int n)
{
	int saved = dup(1);
	backupFile(fn);
	redir_io(fn, 1);
	printf("%d\n", n);
	for (int i = 0; i < n; i++) {
		puts(chr[i].text);
		printf("%d\n", chr[i].popularity);
	}
	restore_io(saved, 1);
}

void saveQuestions(char *fn, Question *qst, int n_qst, int n_chr)
{
	int saved = dup(1);
	backupFile(fn);
	redir_io(fn, 1);
	printf("%d\n", n_qst);
	for (int i = 0; i < n_qst; i++) {
		puts(qst[i].text);
		for (int j = 0; j < n_chr; j++)
			printf(" %d", qst[i].n_yes[j]);
		puts("");
		for (int j = 0; j < n_chr; j++)
			printf(" %d", qst[i].n_no[j]);
		puts("");
		for (int j = 0; j < n_chr; j++)
			printf(" %d", qst[i].n_total[j]);
		puts("");
	}
	restore_io(saved, 1);
}

Character*
insertChr(Character *chr, Question *qst, int n_chr, int n_qst, char *ans) {
	chr = realloc(chr, (n_chr + 1) * sizeof(Character));
	chr[n_chr].text = getText();
	chr[n_chr].popularity = 1;
	for (int i = 0; i < n_qst; i++) {
		qst[i].n_yes = realloc(qst[i].n_yes, (n_chr + 1) * sizeof(int));
		qst[i].n_no = realloc(qst[i].n_no, (n_chr + 1) * sizeof(int));
		qst[i].n_total = realloc(qst[i].n_total, (n_chr + 1) * sizeof(int));
		qst[i].n_yes[n_chr] = 1 + (ans[i] == 'y');
		qst[i].n_no[n_chr] = 1 + (ans[i] == 'n');
		qst[i].n_total[n_chr] = 2 + (ans[i] != 0);
	}
	return chr;
}

Question* insertQuestion(Question *qst, int n_qst, int n_chr, int chr_id) {
	char ch;
	qst = realloc(qst, (n_qst + 1) * sizeof(Question));
	printf("Your question: ");
	qst[n_qst].text = getText();
	qst[n_qst].n_yes = malloc(n_chr * sizeof(int));
	qst[n_qst].n_no = malloc(n_chr * sizeof(int));
	qst[n_qst].n_total = malloc(n_chr * sizeof(int));
	qst[n_qst].l_y = malloc(n_chr * sizeof(double));
	qst[n_qst].l_n = malloc(n_chr * sizeof(double));
	printf("Answer for your character: ");
	ch = getAnswer();
	for (int i = 0; i < n_chr; i++) {
		qst[n_qst].n_yes[i] = 1;
		qst[n_qst].n_no[i] = 1;
		qst[n_qst].n_total[i] = 2;
	}
	qst[n_qst].n_yes[chr_id] += (ch == 'y') + (ch == '?');
	qst[n_qst].n_no[chr_id] += (ch == 'n') + (ch == '?');
	qst[n_qst].n_total[chr_id] += 1 + (ch == '?');
	return qst;
}

int main(int argc, char **argv)
{
	Character *chr;
	Question *qst;
	int n_chr, n_qst, chr_id, qst_id;
	double *odds, prob;
	char *ans, flag;
	init(argc, argv, &chr, &qst, &n_chr, &n_qst);
	odds = malloc(n_chr * sizeof (double));
	ans = malloc(n_qst * sizeof (char));
	do {
		memset(ans, 0, n_qst * sizeof(char));
		odds = getPriorOdds(chr, odds, n_chr);
		normalize(odds, n_chr);
		calcLikelihood(qst, n_qst, n_chr);
		qst_id = 0; chr_id = 0;
		while (chr_id >= 0 && !(
		  (odds[chr_id] > _ACCURACY || qst_id < 0) &&
		  guessChr(chr, qst, n_qst, chr_id, ans, odds))) {
			qst_id = chooseQuestion(qst, odds, n_qst, n_chr, ans);
			if (qst_id < 0) {
				chr_id = mostLikelyChr(odds, n_chr);
				if (odds[chr_id] < 0.1 && !askConfirmation("Continue game?")) {
					chr_id = -1;
				}
				continue;
			}
			puts(qst[qst_id].text);
			ans[qst_id] = getAnswer();
			updateBias(odds, qst[qst_id], n_chr, ans[qst_id]);
			printOdds(odds, n_chr);
			chr_id = mostLikelyChr(odds, n_chr);
		}
		if (chr_id < 0) {
			if (askConfirmation("You win. Do you want to add your character?")) {
				chr = insertChr(chr, qst, n_chr++, n_qst, ans);
				odds = realloc(odds, n_chr * sizeof(double));
			}
		} else if (odds[chr_id] < _ACCURACY) {
			if (askConfirmation("Do you want to add new question?")) {
				qst = insertQuestion(qst, n_qst++, n_chr, chr_id);
				ans = realloc(ans, n_qst * sizeof(char));
			}
		}
		saveCharacters(argv[1], chr, n_chr);
		saveQuestions(argv[2], qst, n_qst, n_chr);
	} while (askConfirmation("One more time?"));
	return 0;
}
