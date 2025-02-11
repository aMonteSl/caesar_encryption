#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

enum {
	MAX_LINE = 1024,
	MIN_MINUS = 'a',
	MIN_UPPER = 'A',
	MAX_MINUS = 'z',
	MAX_UPPER = 'Z',
	DIFF_MINUS_TO_UPPER = 'A' - 'a',
	ALPHABET_SIZE = 'Z' - 'A' + 1
};

struct Caesar {
	int key;
	char *message;
	char *cipher;
};
typedef struct Caesar Caesar;

void
freeCaesar(Caesar *caesar)
{
	if (caesar != NULL) {
		if (caesar->message != NULL) {
			free(caesar->message);
		}
		if (caesar->cipher != NULL) {
			free(caesar->cipher);
		}
		free(caesar);
	}
}

void
initCaesar(Caesar *caesar, char *key)
{
	caesar->message = NULL;
	caesar->cipher = NULL;

	caesar->key = atoi(key);
	if (caesar->key < 0) {
		freeCaesar(caesar);
		errx(EXIT_FAILURE, "atoi failed");
	}
	if (caesar->key == 0) {
		freeCaesar(caesar);
		errx(EXIT_FAILURE, "key must be a positive integer");
	}
}

Caesar *
createCaesar(char *key)
{
	Caesar *caesar = malloc(sizeof(Caesar));

	if (caesar == NULL) {
		errx(EXIT_FAILURE, "malloc failed");
	}

	initCaesar(caesar, key);

	return caesar;
}

void
readAllLines(Caesar *caesar)
{
	int size = 0;
	int capacity = MAX_LINE;
	char *buffer = malloc(capacity);
	char *new_buffer;
	int bytesRead;

	if (buffer == NULL) {
		freeCaesar(caesar);
		errx(EXIT_FAILURE, "malloc failed");
	}

	while ((bytesRead = fread(buffer + size, 1, MAX_LINE, stdin)) > 0) {
		size += bytesRead;
		if (size + MAX_LINE > capacity) {
			capacity *= 2;
			new_buffer = realloc(buffer, capacity);
			if (new_buffer == NULL) {
				free(buffer);
				freeCaesar(caesar);
				errx(EXIT_FAILURE, "realloc failed");
			}
			buffer = new_buffer;
		}
	}

	if (size == 0) {
		free(buffer);
		freeCaesar(caesar);
		errx(EXIT_FAILURE, "no message to cipher");
	}

	buffer[size] = '\0';
	caesar->message = buffer;
}

int
isLowerCase(char c)
{
	return c >= MIN_MINUS && c <= MAX_MINUS;
}

void
toUpper(char *message)
{
	int i;
	int len = strlen(message);

	for (i = 0; i < len; i++) {
		if (isLowerCase(message[i])) {
			message[i] += DIFF_MINUS_TO_UPPER;
		}
	}
}

void
getMessage(Caesar *caesar)
{
	readAllLines(caesar);
	if (caesar->message == NULL) {
		freeCaesar(caesar);
		errx(EXIT_FAILURE, "no message to cipher");
	}
	toUpper(caesar->message);
}

int
isUpperCase(char c)
{
	return c >= MIN_UPPER && c <= MAX_UPPER;
}

int
shiftCharacter(char c, int key)
{
	return (c - MIN_UPPER + key) % ALPHABET_SIZE + MIN_UPPER;
}

void
cipherMessage(Caesar *caesar)
{
	int i;
	int len = strlen(caesar->message);

	caesar->cipher = strdup(caesar->message);
	if (caesar->cipher == NULL) {
		freeCaesar(caesar);
		errx(EXIT_FAILURE, "strdup failed");
	}
	for (i = 0; i < len; i++) {
		if (isUpperCase(caesar->cipher[i])) {
			caesar->cipher[i] =
			    shiftCharacter(caesar->cipher[i], caesar->key);
		}
	}
}

int
main(int argc, char *argv[])
{
	Caesar *caesar;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s key\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	caesar = createCaesar(argv[1]);

	getMessage(caesar);

	cipherMessage(caesar);

	printf("%s", caesar->cipher);

	freeCaesar(caesar);
	exit(EXIT_SUCCESS);
}
