#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <math.h>

enum {
	MAX_LINE = 1024,
	MIN_MINUS = 'a',
	MIN_UPPER = 'A',
	MAX_MINUS = 'z',
	MAX_UPPER = 'Z',
	DIFF_MINUS_TO_UPPER = 'A' - 'a',
	ALPHABET_SIZE = 'Z' - 'A' + 1
};

double FREQ_ENGLISH[ALPHABET_SIZE] = {
	0.08167,		// A
	0.01492,		// B
	0.02782,		// C
	0.04253,		// D
	0.12702,		// E
	0.02228,		// F
	0.02015,		// G
	0.06094,		// H
	0.06966,		// I
	0.00153,		// J
	0.00772,		// K
	0.04025,		// L
	0.02406,		// M
	0.06749,		// N
	0.07507,		// O
	0.01929,		// P
	0.00095,		// Q
	0.05987,		// R
	0.06327,		// S
	0.09056,		// T
	0.02758,		// U
	0.00978,		// V
	0.02360,		// W
	0.00150,		// X
	0.01974,		// Y
	0.00074			// Z
};

char *COMMON_BIGRAMS[] = {
	"TH", "HE", "IN", "EN", "NT", "RE", "ER", "AN", "TI", "ES",
	"ON", "AT", "SE", "ND", "OR", "AR", "AL", "TE", "CO", "DE",
	"TO", "RA", "ET", "ED", "IT", "SA", "EM", "RO"
};

int NUM_BIGRAMS = sizeof(COMMON_BIGRAMS) / sizeof(COMMON_BIGRAMS[0]);

const char *COMMON_TRIGRAMS[] = {
	"THE", "AND", "THA", "ENT", "ING", "ION", "TIO", "FOR",
	"NDE", "HAS", "NCE", "EDT", "TIS", "OFT", "STH", "MEN"
};

int NUM_TRIGRAMS = sizeof(COMMON_TRIGRAMS) / sizeof(COMMON_TRIGRAMS[0]);

typedef struct Candidate Candidate;
struct Candidate {
	int key;
	double distance;
	int bigrams;
	int trigrams;
	Candidate *next;
};

typedef struct Candidates {
	Candidate *top;
	Candidate *last;
} Candidates;

void
readAllLines(char **dest)
{
	int total_size = 0;
	int len;
	char buffer[MAX_LINE];
	char *message = NULL;
	char *new_message;

	while (fgets(buffer, MAX_LINE, stdin) != NULL) {
		len = strlen(buffer);
		new_message = realloc(message, total_size + len + 1);
		if (new_message == NULL) {
			free(message);
			errx(EXIT_FAILURE, "realloc failed");
		}
		message = new_message;
		strcpy(message + total_size, buffer);
		total_size += len;
	}

	if (message == NULL) {
		errx(EXIT_FAILURE, "no message to cipher");
	}

	*dest = message;
}

int
isLowerCase(char c)
{
	return c >= MIN_MINUS && c <= MAX_MINUS;
}

void
toUpper(char *message)
{
	int len = strlen(message);
	int i;

	for (i = 0; i < len; i++) {
		if (isLowerCase(message[i])) {
			message[i] += DIFF_MINUS_TO_UPPER;
		}
	}
}

void
getMessage(char **message)
{
	readAllLines(message);
	if (*message == NULL) {
		fprintf(stderr, "Error: no message to read\n");
		exit(EXIT_FAILURE);
	}
	toUpper(*message);
}

int
isNullPointer(void *pointer)
{
	return pointer == NULL;
}

Candidates *
newCandidates()
{
	Candidates *candidates = malloc(sizeof(Candidates));

	if (isNullPointer(candidates)) {
		fprintf(stderr, "Error: creation candidates failed\n");
		exit(EXIT_FAILURE);
	}
	candidates->top = NULL;
	candidates->last = NULL;
	return candidates;
}

void
freeCandidates(Candidates *candidates)
{
	if (candidates == NULL)
		return;
	Candidate *current = candidates->top;

	while (current != NULL) {
		Candidate *temp = current;

		current = current->next;
		free(temp);
	}
	free(candidates);
}

Candidate *
createCandidate(int key, double distance, int bigrams, int trigrams)
{
	Candidate *candidate = malloc(sizeof(Candidate));

	if (candidate == NULL) {
		errx(EXIT_FAILURE, "malloc failed");
	}
	candidate->key = key;
	candidate->distance = distance;
	candidate->bigrams = bigrams;
	candidate->trigrams = trigrams;
	candidate->next = NULL;
	return candidate;
}

void
addCandidate(Candidate *candidate, Candidates *candidates)
{
	if (candidates->top == NULL) {
		candidates->top = candidate;
	} else {
		candidates->last->next = candidate;
	}
	candidates->last = candidate;
}

void
decryptWithKey(char *ciphertext, char *plaintext, int key, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		if (ciphertext[i] >= MIN_UPPER && ciphertext[i] <= MAX_UPPER) {
			plaintext[i] =
			    (ciphertext[i] - MIN_UPPER - key +
			     ALPHABET_SIZE) % ALPHABET_SIZE + MIN_UPPER;
		} else {
			plaintext[i] = ciphertext[i];
		}
	}
	plaintext[len] = '\0';
}

int
countBigrams(char *text)
{
	int count = 0;
	int len = strlen(text);
	int i;
	int j;
	char bigram[3];

	for (i = 0; i < (len > 0 ? len - 1 : 0); i++) {
		bigram[0] = text[i];
		bigram[1] = text[i + 1];
		bigram[2] = '\0';
		for (j = 0; j < NUM_BIGRAMS; j++) {
			if (strcmp(bigram, COMMON_BIGRAMS[j]) == 0) {
				count++;
				break;
			}
		}
	}
	return count;
}

int
countTrigrams(char *text)
{
	int count = 0;
	int len = strlen(text);
	int i;
	int j;
	char trigram[4];

	for (i = 0; i < (len > 1 ? len - 2 : 0); i++) {
		trigram[0] = text[i];
		trigram[1] = text[i + 1];
		trigram[2] = text[i + 2];
		trigram[3] = '\0';
		for (j = 0; j < NUM_TRIGRAMS; j++) {
			if (strcmp(trigram, COMMON_TRIGRAMS[j]) == 0) {
				count++;
				break;
			}
		}
	}
	return count;
}

void
calculateFrequencies(char *text, double *frequencies)
{
	int count[ALPHABET_SIZE] = { 0 };
	int total = 0;
	int len = strlen(text);
	int i;

	for (i = 0; i < len; i++) {
		if (text[i] >= MIN_UPPER && text[i] <= MAX_UPPER) {
			count[text[i] - MIN_UPPER]++;
			total++;
		}
	}

	for (i = 0; i < ALPHABET_SIZE; i++) {
		if (total > 0) {
			frequencies[i] = (double)count[i] / total;
		} else {
			frequencies[i] = 0;
		}
	}
}

double
calculateEuclideanDistance(double *freq1, double *freq2)
{
	int i;
	double diff;
	double distance = 0.0;

	for (i = 0; i < ALPHABET_SIZE; i++) {
		diff = freq1[i] - freq2[i];
		distance += pow(diff, 2);
	}
	return sqrt(distance);
}

void
startBruteForce(char *ciphertext, Candidates *candidates)
{
	int len = strlen(ciphertext);
	char *plaintext = malloc(len + 1);
	double frequencies[ALPHABET_SIZE];
	double distance;
	int key;
	int bigrams;
	int trigrams;

	if (!plaintext)
		errx(EXIT_FAILURE, "malloc failed");

	for (key = 1; key <= ALPHABET_SIZE - 1; key++) {
		decryptWithKey(ciphertext, plaintext, key, len);
		bigrams = countBigrams(plaintext);
		trigrams = countTrigrams(plaintext);
		calculateFrequencies(plaintext, frequencies);
		distance =
		    calculateEuclideanDistance(frequencies, FREQ_ENGLISH);
		Candidate *new_candidate =
		    createCandidate(key, distance, bigrams, trigrams);
		addCandidate(new_candidate, candidates);
	}
	free(plaintext);
}

Candidate *
copyCandidate(Candidate *original)
{
	Candidate *copy = malloc(sizeof(Candidate));

	if (copy != NULL) {
		copy->key = original->key;
		copy->distance = original->distance;
		copy->bigrams = original->bigrams;
		copy->trigrams = original->trigrams;
		copy->next = NULL;
	}
	return copy;
}

int
isBestDistance(Candidate *current, Candidate *best)
{
	return best == NULL || current->distance < best->distance;
}

int
isBestBigrams(Candidate *current, Candidate *best)
{
	return best == NULL || current->bigrams > best->bigrams;
}

int
isBestTrigrams(Candidate *current, Candidate *best)
{
	return best == NULL || current->trigrams > best->trigrams;
}

int
areCandidatesDifferent(Candidate *candidate1, Candidate *candidate2)
{
	return candidate1 == NULL || candidate2 == NULL
	    || candidate1->key != candidate2->key;
}

void
addUniqueCandidates(Candidate *bestDistance, Candidate *bestBigrams,
		    Candidate *bestTrigrams, Candidates *filteredCandidates)
{
	if (bestDistance) {
		addCandidate(bestDistance, filteredCandidates);
	}
	if (bestBigrams) {
		if (areCandidatesDifferent(bestDistance, bestBigrams)) {
			addCandidate(bestBigrams, filteredCandidates);
		} else {
			free(bestBigrams);
		}
	}
	if (bestTrigrams) {
		if (areCandidatesDifferent(bestDistance, bestTrigrams)
		    && areCandidatesDifferent(bestBigrams, bestTrigrams)) {
			addCandidate(bestTrigrams, filteredCandidates);
		} else {
			free(bestTrigrams);
		}
	}
}

Candidates *
filterCandidates(Candidates *candidates)
{
	Candidate *bestDistance = NULL;
	Candidate *bestBigrams = NULL;
	Candidate *bestTrigrams = NULL;

	for (Candidate * current = candidates->top; current != NULL;
	     current = current->next) {
		if (isBestDistance(current, bestDistance)) {
			if (bestDistance != NULL) {
				free(bestDistance);
			}
			bestDistance = copyCandidate(current);
		}
		if (isBestBigrams(current, bestBigrams)) {
			if (bestBigrams != NULL) {
				free(bestBigrams);
			}
			bestBigrams = copyCandidate(current);
		}
		if (isBestTrigrams(current, bestTrigrams)) {
			if (bestTrigrams != NULL) {
				free(bestTrigrams);
			}
			bestTrigrams = copyCandidate(current);
		}
	}

	Candidates *filteredCandidates = newCandidates();

	addUniqueCandidates(bestDistance, bestBigrams, bestTrigrams,
			    filteredCandidates);

	freeCandidates(candidates);
	return filteredCandidates;
}

void
printBestCandidates(Candidates *candidates)
{
	for (Candidate * current = candidates->top; current != NULL;
	     current = current->next) {
		printf("%d: %f, %d, %d\n", current->key, current->distance,
		       current->bigrams, current->trigrams);
	}
}

void
writeFile(const char *filename, const char *plaintext)
{
	FILE *file = fopen(filename, "w");

	if (file == NULL) {
		fprintf(stderr, "Error: No se pudo crear el archivo %s\n",
			filename);
		exit(EXIT_FAILURE);
	}
	fprintf(file, "%s", plaintext);
	fclose(file);
}

void
generateFilename(char *filename, int key)
{
	sprintf(filename, "key-%d.txt", key);
}

void
processCandidate(Candidate *candidate, char *ciphertext, char *plaintext,
		 int len)
{
	decryptWithKey(ciphertext, plaintext, candidate->key, len);
	char filename[20];

	generateFilename(filename, candidate->key);
	writeFile(filename, plaintext);
}

void
generateFiles(Candidates *candidates, char *ciphertext)
{
	int len = strlen(ciphertext);
	char *plaintext = malloc(len + 1);

	if (!plaintext)
		errx(EXIT_FAILURE, "malloc failed");

	for (Candidate * current = candidates->top; current != NULL;
	     current = current->next) {
		processCandidate(current, ciphertext, plaintext, len);
	}
	free(plaintext);
}

int
main(int argc, char *argv[])
{
	char *ciphertext = NULL;

	getMessage(&ciphertext);

	Candidates *candidates = newCandidates();

	startBruteForce(ciphertext, candidates);

	Candidates *filteredCandidates = filterCandidates(candidates);

	printBestCandidates(filteredCandidates);
	generateFiles(filteredCandidates, ciphertext);

	freeCandidates(filteredCandidates);
	free(ciphertext);
	exit(EXIT_SUCCESS);
}
