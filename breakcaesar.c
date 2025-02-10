#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    0.08167, // A
    0.01492, // B
    0.02782, // C
    0.04253, // D
    0.12702, // E
    0.02228, // F
    0.02015, // G
    0.06094, // H
    0.06966, // I
    0.00153, // J
    0.00772, // K
    0.04025, // L
    0.02406, // M
    0.06749, // N
    0.07507, // O
    0.01929, // P
    0.00095, // Q
    0.05987, // R
    0.06327, // S
    0.09056, // T
    0.02758, // U
    0.00978, // V
    0.02360, // W
    0.00150, // X
    0.01974, // Y
    0.00074  // Z
};

const char *COMMON_BIGRAMS[] = {
    "TH", "HE", "IN", "EN", "NT", "RE", "ER", "AN", "TI", "ES", "ON", "AT", "SE", "ND", "OR", "AR", "AL", "TE", "CO", "DE", "TO", "RA", "ET", "ED", "IT", "SA", "EM", "RO"
};
const int NUM_BIGRAMS = sizeof(COMMON_BIGRAMS) / sizeof(COMMON_BIGRAMS[0]);

const char *COMMON_TRIGRAMS[] = {
    "THE", "AND", "THA", "ENT", "ING", "ION", "TIO", "FOR", "NDE", "HAS", "NCE", "EDT", "TIS", "OFT", "STH", "MEN"
};
const int NUM_TRIGRAMS = sizeof(COMMON_TRIGRAMS) / sizeof(COMMON_TRIGRAMS[0]);

struct Candidate
{
    int key;
    double distance;
    int bigrams;
    int trigrams;
};
typedef struct Candidate Candidate;

struct Candidates
{
    Candidate candidate;
    struct Candidates *next;
};
typedef struct Candidates Candidates;

void
readline(char **message)
{
    char line[MAX_LINE];
    if (fgets(line, MAX_LINE, stdin) == NULL){
        fprintf(stderr, "Error: No se pudo leer la linea\n");
        exit(EXIT_FAILURE);
    }
    *message = strdup(line);
    if (*message == NULL) {
        fprintf(stderr, "Error: No se pudo duplicar la linea\n");
        exit(EXIT_FAILURE);
    }
}

void
toUpper(char *message){
    for (int i = 0; i < strlen(message); i++){
        if (message[i] >= MIN_MINUS && message[i] <= MAX_MINUS){
            message[i] += DIFF_MINUS_TO_UPPER;
        }
    }
}

void
getMessage(char **message){
    readline(message);
    if (*message == NULL){
        fprintf(stderr, "Error: No se pudo leer el mensaje\n");
        exit(EXIT_FAILURE);
    }
    toUpper(*message);
}

void
calculateFrequencies(const char *text, double *frequencies) {
    int count[ALPHABET_SIZE] = {0};
    int total = 0;

    for (int i = 0; i < strlen(text); i++) {
        if (text[i] >= MIN_UPPER && text[i] <= MAX_UPPER) {
            count[text[i] - MIN_UPPER]++;
            total++;
        }
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        frequencies[i] = (double)count[i] / total;
    }
}

double calculateEuclideanDistance(const double *freq1, const double *freq2) {
    double distance = 0.0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        double diff = freq1[i] - freq2[i];
        distance += diff * diff;  // Evita pow()
    }

    // Aproximación de sqrt usando el método de Newton-Raphson
    double x = distance;
    double approx = distance;
    if (distance > 0) {
        for (int i = 0; i < 10; i++) {  // Iteraciones para mejorar precisión
            approx = 0.5 * (approx + x / approx);
        }
    } else {
        approx = 0.0;  // sqrt(0) = 0
    }

    return approx;
}

void
decryptWithKey(const char *ciphertext, char *plaintext, int key) {
    for (int i = 0; i < strlen(ciphertext); i++) {
        if (ciphertext[i] >= MIN_UPPER && ciphertext[i] <= MAX_UPPER) {
            plaintext[i] = (ciphertext[i] - MIN_UPPER - key + ALPHABET_SIZE) % ALPHABET_SIZE + MIN_UPPER;
        } else {
            plaintext[i] = ciphertext[i];
        }
    }
    plaintext[strlen(ciphertext)] = '\0';
}

int
countBigrams(const char *text) {
    int count = 0;
    for (int i = 0; i < strlen(text) - 1; i++) {
        char bigram[3] = {text[i], text[i + 1], '\0'};
        for (int j = 0; j < NUM_BIGRAMS; j++) {
            if (strcmp(bigram, COMMON_BIGRAMS[j]) == 0) {
                count++;
                break;
            }
        }
    }
    return count;
}

int
countTrigrams(const char *text) {
    int count = 0;
    for (int i = 0; i < strlen(text) - 2; i++) {
        char trigram[4] = {text[i], text[i + 1], text[i + 2], '\0'};
        for (int j = 0; j < NUM_TRIGRAMS; j++) {
            if (strcmp(trigram, COMMON_TRIGRAMS[j]) == 0) {
                count++;
                break;
            }
        }
    }
    return count;
}

void
startBruteForce(const char *ciphertext, Candidates **candidates) {
    char plaintext[MAX_LINE];
    double frequencies[ALPHABET_SIZE];

    for (int key = 1; key <= ALPHABET_SIZE - 1 ; key++) {
        decryptWithKey(ciphertext, plaintext, key);
        calculateFrequencies(plaintext, frequencies);
        double distance = calculateEuclideanDistance(frequencies, FREQ_ENGLISH);
        int bigrams = countBigrams(plaintext);
        int trigrams = countTrigrams(plaintext);

        // Crear un nuevo candidato
        Candidates *new_candidate = malloc(sizeof(Candidates));
        new_candidate->candidate.key = key;
        new_candidate->candidate.distance = distance;
        new_candidate->candidate.bigrams = bigrams;
        new_candidate->candidate.trigrams = trigrams;
        new_candidate->next = *candidates;
        *candidates = new_candidate;

        // // Guardar el texto descifrado en un archivo
        // char filename[20];
        // sprintf(filename, "key-%d.txt", key);
        // FILE *file = fopen(filename, "w");
        // if (file == NULL) {
        //     fprintf(stderr, "Error: No se pudo crear el archivo %s\n", filename);
        //     exit(EXIT_FAILURE);
        // }
        // fprintf(file, "%s", plaintext);
        // fclose(file);
    }
}

void
printCandidates(Candidates *candidates) {
    Candidates *bestDistance = candidates;
    Candidates *bestBigrams = candidates;
    Candidates *bestTrigrams = candidates;

    for (Candidates *current = candidates; current != NULL; current = current->next) {
        if (current->candidate.distance < bestDistance->candidate.distance) {
            bestDistance = current;
        }
        if (current->candidate.bigrams > bestBigrams->candidate.bigrams) {
            bestBigrams = current;
        }
        if (current->candidate.trigrams > bestTrigrams->candidate.trigrams) {
            bestTrigrams = current;
        }
    }

    if (bestDistance->candidate.key == bestBigrams->candidate.key && bestBigrams->candidate.key == bestTrigrams->candidate.key) {
        printf("%d: %f, %d, %d\n", bestDistance->candidate.key, bestDistance->candidate.distance, bestDistance->candidate.bigrams, bestDistance->candidate.trigrams);
    } else if (bestDistance->candidate.key == bestBigrams->candidate.key) {
        printf("%d: %f, %d, %d\n", bestDistance->candidate.key, bestDistance->candidate.distance, bestDistance->candidate.bigrams, bestDistance->candidate.trigrams);
        printf("%d: %f, %d, %d\n", bestTrigrams->candidate.key, bestTrigrams->candidate.distance, bestTrigrams->candidate.bigrams, bestTrigrams->candidate.trigrams);
    } else if (bestDistance->candidate.key == bestTrigrams->candidate.key) {
        printf("%d: %f, %d, %d\n", bestDistance->candidate.key, bestDistance->candidate.distance, bestDistance->candidate.bigrams, bestDistance->candidate.trigrams);
        printf("%d: %f, %d, %d\n", bestBigrams->candidate.key, bestBigrams->candidate.distance, bestBigrams->candidate.bigrams, bestBigrams->candidate.trigrams);
    } else if (bestBigrams->candidate.key == bestTrigrams->candidate.key) {
        printf("%d: %f, %d, %d\n", bestBigrams->candidate.key, bestBigrams->candidate.distance, bestBigrams->candidate.bigrams, bestBigrams->candidate.trigrams);
        printf("%d: %f, %d, %d\n", bestDistance->candidate.key, bestDistance->candidate.distance, bestDistance->candidate.bigrams, bestDistance->candidate.trigrams);
    } else {
        printf("%d: %f, %d, %d\n", bestDistance->candidate.key, bestDistance->candidate.distance, bestDistance->candidate.bigrams, bestDistance->candidate.trigrams);
        printf("%d: %f, %d, %d\n", bestBigrams->candidate.key, bestBigrams->candidate.distance, bestBigrams->candidate.bigrams, bestBigrams->candidate.trigrams);
        printf("%d: %f, %d, %d\n", bestTrigrams->candidate.key, bestTrigrams->candidate.distance, bestTrigrams->candidate.bigrams, bestTrigrams->candidate.trigrams);
    }
}

void
freeCandidates(Candidates *candidates) {
    while (candidates != NULL) {
        Candidates *temp = candidates;
        candidates = candidates->next;
        free(temp);
    }
}

int main(int argc, char* argv[]){
    char *ciphertext = NULL;

    // Obtener de la entrada estandar el mensaje a descifrar
    getMessage(&ciphertext);

    Candidates *candidates = malloc(sizeof(Candidates));

    printf("Ciphertext: %s\n", ciphertext);

    // Iniciar la fuerza bruta para probar las 25 claves 1-25
    startBruteForce(ciphertext, &candidates);

    // Imprimir los candidatos a solución
    printCandidates(candidates);

    // Liberar la memoria de los candidatos
    freeCandidates(candidates);

    free(ciphertext);
    return 0;
}