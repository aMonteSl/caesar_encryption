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
    "TH", "HE", "IN", "EN", "NT", "RE", "ER", "AN", "TI", "ES",
    "ON", "AT", "SE", "ND", "OR", "AR", "AL", "TE", "CO", "DE",
    "TO", "RA", "ET", "ED", "IT", "SA", "EM", "RO"
};
const int NUM_BIGRAMS = sizeof(COMMON_BIGRAMS) / sizeof(COMMON_BIGRAMS[0]);

const char *COMMON_TRIGRAMS[] = {
    "THE", "AND", "THA", "ENT", "ING", "ION", "TIO", "FOR",
    "NDE", "HAS", "NCE", "EDT", "TIS", "OFT", "STH", "MEN"
};
const int NUM_TRIGRAMS = sizeof(COMMON_TRIGRAMS) / sizeof(COMMON_TRIGRAMS[0]);

typedef struct Candidate Candidate;
struct Candidate {
    int key;
    double distance;
    int bigrams;
    int trigrams;
    Candidate *next;
};

typedef struct Candidates Candidates;
struct Candidates {
    Candidate *top;
};

/* Lee toda la entrada estándar y la almacena en *message */
void readAllLines(char **message) {
    size_t size = 0;
    size_t capacity = MAX_LINE * 4; // bloques más grandes
    char *buffer = malloc(capacity);
    if (buffer == NULL) {
        errx(EXIT_FAILURE, "malloc failed");
    }

    size_t bytesRead;
    while ((bytesRead = fread(buffer + size, 1, capacity - size, stdin)) > 0) {
        size += bytesRead;
        if (size == capacity) {
            capacity *= 2;
            char *new_buffer = realloc(buffer, capacity);
            if (new_buffer == NULL) {
                free(buffer);
                errx(EXIT_FAILURE, "realloc failed");
            }
            buffer = new_buffer;
        }
    }

    if (size == 0) {
        free(buffer);
        errx(EXIT_FAILURE, "no message to process");
    }

    buffer[size] = '\0'; // Null-terminate
    *message = buffer;
}

int isLowerCase(char c) {
    return c >= MIN_MINUS && c <= MAX_MINUS;
}

/* Convierte la cadena a mayúsculas */
void toUpper(char *message) {
    size_t len = strlen(message);
    for (size_t i = 0; i < len; i++) {
        if (isLowerCase(message[i])) {
            message[i] += DIFF_MINUS_TO_UPPER;
        }
    }
}

/* Obtiene el mensaje de entrada y lo convierte a mayúsculas */
void getMessage(char **message) {
    readAllLines(message);
    if (*message == NULL) {
        fprintf(stderr, "Error: No se pudo leer el mensaje\n");
        exit(EXIT_FAILURE);
    }
    toUpper(*message);
}

int isNullPointer(void *pointer) {
    return pointer == NULL;
}

Candidates *newCandidates() {
    Candidates *candidates = malloc(sizeof(Candidates));
    if (isNullPointer(candidates)) {
        fprintf(stderr, "Error: No se pudo crear la lista de candidatos\n");
        exit(EXIT_FAILURE);
    }
    candidates->top = NULL;
    return candidates;
}

void freeCandidates(Candidates *candidates) {
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

/* Descifra el texto usando la clave dada */
void decryptWithKey(const char *ciphertext, char *plaintext, int key) {
    size_t len = strlen(ciphertext);
    for (size_t i = 0; i < len; i++) {
        if (ciphertext[i] >= MIN_UPPER && ciphertext[i] <= MAX_UPPER) {
            plaintext[i] = (ciphertext[i] - MIN_UPPER - key + ALPHABET_SIZE) % ALPHABET_SIZE + MIN_UPPER;
        } else {
            plaintext[i] = ciphertext[i];
        }
    }
    plaintext[len] = '\0';
}

/* Calcula las frecuencias de cada letra en el texto */
void calculateFrequencies(const char *text, double *frequencies) {
    int count[ALPHABET_SIZE] = {0};
    int total = 0;
    size_t len = strlen(text);

    for (size_t i = 0; i < len; i++) {
        if (text[i] >= MIN_UPPER && text[i] <= MAX_UPPER) {
            count[text[i] - MIN_UPPER]++;
            total++;
        }
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        frequencies[i] = (total > 0) ? ((double) count[i] / total) : 0;
    }
}

/* Calcula la distancia euclidiana entre dos vectores de frecuencias */
double calculateEuclideanDistance(const double *freq1, const double *freq2) {
    double distance = 0.0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        double diff = freq1[i] - freq2[i];
        distance += diff * diff;
    }
    /* Aproximación de sqrt con Newton-Raphson */
    double x = distance;
    double approx = distance;
    if (distance > 0) {
        for (int i = 0; i < 10; i++) {
            approx = 0.5 * (approx + x / approx);
        }
    } else {
        approx = 0.0;
    }
    return approx;
}

/* Cuenta la cantidad de bigramas comunes en el texto */
int countBigrams(const char *text) {
    int count = 0;
    size_t len = strlen(text);
    for (size_t i = 0; i < (len > 0 ? len - 1 : 0); i++) {
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

/* Cuenta la cantidad de trigramas comunes en el texto */
int countTrigrams(const char *text) {
    int count = 0;
    size_t len = strlen(text);
    for (size_t i = 0; i < (len > 1 ? len - 2 : 0); i++) {
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

/* Crea un nuevo candidato para la clave dada */
Candidate *createCandidate(int key, double distance, int bigrams, int trigrams, Candidates *candidates) {
    Candidate *candidate = malloc(sizeof(Candidate));
    if (candidate == NULL) {
        fprintf(stderr, "Error: No se pudo crear el candidato\n");
        freeCandidates(candidates);
        exit(EXIT_FAILURE);
    }
    candidate->key = key;
    candidate->distance = distance;
    candidate->bigrams = bigrams;
    candidate->trigrams = trigrams;
    candidate->next = NULL;
    return candidate;
}

void addCandidate(Candidate *candidate, Candidates *candidates) {
    if (candidates->top == NULL) {
        candidates->top = candidate;
    } else {
        Candidate *current = candidates->top;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = candidate;
    }
}

/* Prueba todas las claves (1 a ALPHABET_SIZE-1) y guarda cada candidato */
void startBruteForce(const char *ciphertext, Candidates *candidates) {
    size_t len = strlen(ciphertext);
    /* Se asigna un buffer con el tamaño del mensaje completo */
    char *plaintext = malloc(len + 1);
    if (!plaintext)
        errx(EXIT_FAILURE, "malloc failed");

    double frequencies[ALPHABET_SIZE];
    double distance;
    int key, bigrams, trigrams;

    for (key = 1; key <= ALPHABET_SIZE - 1; key++) {
        decryptWithKey(ciphertext, plaintext, key);
        calculateFrequencies(plaintext, frequencies);
        distance = calculateEuclideanDistance(frequencies, FREQ_ENGLISH);
        bigrams = countBigrams(plaintext);
        trigrams = countTrigrams(plaintext);
        Candidate *new_candidate = createCandidate(key, distance, bigrams, trigrams, candidates);
        addCandidate(new_candidate, candidates);
    }
    free(plaintext);
}

/* Realiza una copia del candidato */
Candidate *copyCandidate(Candidate *original) {
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

Candidate *getBestDistance(Candidates *candidates) {
    Candidate *best = candidates->top;
    for (Candidate *current = candidates->top->next; current != NULL; current = current->next) {
        if (current->distance < best->distance) {
            best = current;
        }
    }
    return copyCandidate(best);
}

Candidate *getBestBigrams(Candidates *candidates) {
    Candidate *best = candidates->top;
    for (Candidate *current = candidates->top->next; current != NULL; current = current->next) {
        if (current->bigrams > best->bigrams) {
            best = current;
        }
    }
    return copyCandidate(best);
}

Candidate *getBestTrigrams(Candidates *candidates) {
    Candidate *best = candidates->top;
    for (Candidate *current = candidates->top->next; current != NULL; current = current->next) {
        if (current->trigrams > best->trigrams) {
            best = current;
        }
    }
    return copyCandidate(best);
}

/*
 * Filtra los candidatos para quedarse solo con los mejores en distancia,
 * bigramas y trigramas. Se liberan las copias duplicadas para evitar fugas.
 */
Candidates *filterCandidates(Candidates *candidates) {
    Candidate *bestDistance = getBestDistance(candidates);
    Candidate *bestBigrams = getBestBigrams(candidates);
    Candidate *bestTrigrams = getBestTrigrams(candidates);

    printf("Best distance: %f, correspondiente a la key: %d\n", bestDistance->distance, bestDistance->key);
    printf("Best bigrams: %d, correspondiente a la key: %d\n", bestBigrams->bigrams, bestBigrams->key);
    printf("Best trigrams: %d, correspondiente a la key: %d\n", bestTrigrams->trigrams, bestTrigrams->key);

    Candidates *filteredCandidates = newCandidates();

    /* Añadimos cada candidato si no es duplicado; si es duplicado se libera */
    if (bestDistance) {
        addCandidate(bestDistance, filteredCandidates);
    }
    if (bestBigrams) {
        if (bestDistance == NULL || bestBigrams->key != bestDistance->key) {
            addCandidate(bestBigrams, filteredCandidates);
        } else {
            free(bestBigrams);
        }
    }
    if (bestTrigrams) {
        if ((bestDistance == NULL || bestTrigrams->key != bestDistance->key) &&
            (bestBigrams == NULL || bestTrigrams->key != bestBigrams->key)) {
            addCandidate(bestTrigrams, filteredCandidates);
        } else {
            free(bestTrigrams);
        }
    }

    /* Liberamos la lista original de candidatos */
    freeCandidates(candidates);
    return filteredCandidates;
}

void printBestCandidates(Candidates *candidates) {
    for (Candidate *current = candidates->top; current != NULL; current = current->next) {
        printf("%d: %f, %d, %d\n", current->key, current->distance, current->bigrams, current->trigrams);
    }
}

/* Genera un archivo por cada candidato con el mensaje descifrado */
void generateFiles(Candidates *candidates, const char *ciphertext) {
    size_t len = strlen(ciphertext);
    char *plaintext = malloc(len + 1);
    if (!plaintext)
        errx(EXIT_FAILURE, "malloc failed");

    for (Candidate *current = candidates->top; current != NULL; current = current->next) {
        decryptWithKey(ciphertext, plaintext, current->key);
        char filename[20];
        sprintf(filename, "key-%d.txt", current->key);
        FILE *file = fopen(filename, "w");
        if (file == NULL) {
            fprintf(stderr, "Error: No se pudo crear el archivo %s\n", filename);
            free(plaintext);
            exit(EXIT_FAILURE);
        }
        fprintf(file, "%s", plaintext);
        fclose(file);
    }
    free(plaintext);
}

int main(int argc, char *argv[]) {
    char *ciphertext = NULL;

    /* Obtener el mensaje a descifrar desde la entrada estándar */
    getMessage(&ciphertext);
    printf("Ciphertext: %s\n", ciphertext);

    /* Inicializar candidatos y ejecutar la fuerza bruta */
    Candidates *candidates = newCandidates();
    startBruteForce(ciphertext, candidates);

    /* Filtrar candidatos para quedarse solo con los mejores */
    Candidates *filteredCandidates = filterCandidates(candidates);

    /* Imprimir información y generar archivos de salida */
    printBestCandidates(filteredCandidates);
    generateFiles(filteredCandidates, ciphertext);

    freeCandidates(filteredCandidates);
    free(ciphertext);
    return 0;
}
