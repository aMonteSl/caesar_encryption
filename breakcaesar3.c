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

typedef struct BreakCaesar BreakCaesar;
struct BreakCaesar {
    Candidate *top;
    char* ciphertext;
};

void initBreakCaesar(BreakCaesar *breakCaesar) {
    breakCaesar->top = NULL;
    breakCaesar->ciphertext = NULL;
}


void readAllLines(char **message) {
    char *buffer = NULL;
    size_t size = 0;
    size_t capacity = MAX_LINE;
    
    buffer = malloc(capacity);
    if (!buffer) {
        perror("malloc");
        return;
    }

    size_t bytesRead;
    while ((bytesRead = fread(buffer + size, 1, MAX_LINE, stdin)) > 0) {
        size += bytesRead;

        // Ampliar memoria si es necesario
        if (size + MAX_LINE > capacity) {
            capacity *= 2;
            char *temp = realloc(buffer, capacity);
            if (!temp) {
                perror("realloc");
                free(buffer);
                return;
            }
            buffer = temp;
        }
    }

    // Agregar el terminador nulo para que sea una cadena válida
    buffer[size] = '\0';

    *message = buffer;
}

int isLowerCase(char c) {
    return c >= MIN_MINUS && c <= MAX_MINUS;
}


void toUpper(char *message) {
    size_t len = strlen(message);
    for (size_t i = 0; i < len; i++) {
        if (isLowerCase(message[i])) {
            message[i] += DIFF_MINUS_TO_UPPER;
        }
    }
}


void freeBreakCaesar(BreakCaesar *breakCaesar){
    if (breakCaesar == NULL) {
        return;
    }
    Candidate *current = breakCaesar->top;
    while (current != NULL) {
        Candidate *temp = current;
        current = current->next;
        free(temp);
    }
    if (breakCaesar->ciphertext != NULL) {
        free(breakCaesar->ciphertext);
    }
    free(breakCaesar);
}

void getMessage(BreakCaesar *breakCaesar){
    readAllLines(&breakCaesar->ciphertext);
    if (breakCaesar->ciphertext == NULL) {
        freeBreakCaesar(breakCaesar);
        fprintf(stderr, "no message to break\n");
        exit(EXIT_FAILURE);
    }
    toUpper(breakCaesar->ciphertext);
}














/* Descifra el texto usando la clave dada */
void decryptWithKey(const char *ciphertext, char *plaintext, int key) {
    int len = strlen(ciphertext);
    int i;
    for (i = 0; i < len; i++) {
        char c = ciphertext[i];
        if (c >= MIN_UPPER && c <= MAX_UPPER) {
            plaintext[i] = (c - MIN_UPPER - key + ALPHABET_SIZE) % ALPHABET_SIZE + MIN_UPPER;
        } else {
            plaintext[i] = c;
        }
    }
    plaintext[len] = '\0';
}


void calculateFrequencies(char *text, double *frequencies) {
    int count[ALPHABET_SIZE] = {0};
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
            frequencies[i] = (double) count[i] / total;
        } else {
            frequencies[i] = 0;
        }
    }
}

double calculateEuclideanDistance(double *freq1, double *freq2) {
    int i;
    double diff;
    double x;
    double approx;
    double distance = 0.0;

    for (i = 0; i < ALPHABET_SIZE; i++) {
        diff = freq1[i] - freq2[i];
        distance += diff * diff;
    }

    /* Aproximación de sqrt con Newton-Raphson */
    x = distance;
    approx = distance;
    if (distance > 0) {
        for (i = 0; i < 10; i++) {
            approx = 0.5 * (approx + x / approx);
        }
    } else {
        approx = 0.0;
    }
    return approx;
}


int countBigrams(char *text) {
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

int countTrigrams(char *text) {
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

// /* Crea un nuevo candidato para la clave dada */
// Candidate *createCandidate(int key, double distance, int bigrams, int trigrams, Candidates *candidates) {
//     Candidate *candidate = malloc(sizeof(Candidate));
//     if (candidate == NULL) {
//         fprintf(stderr, "Error: No se pudo crear el candidato\n");
//         freeCandidates(candidates);
//         exit(EXIT_FAILURE);
//     }
//     candidate->key = key;
//     candidate->distance = distance;
//     candidate->bigrams = bigrams;
//     candidate->trigrams = trigrams;
//     candidate->next = NULL;
//     return candidate;
// }

// void addCandidate(Candidate *candidate, Candidates *candidates) {
//     if (candidates->top == NULL) {
//         candidates->top = candidate;
//     } else {
//         Candidate *current = candidates->top;
//         while (current->next != NULL) {
//             current = current->next;
//         }
//         current->next = candidate;
//     }
// }

Candidate *createCandidate(int key, double distance, int bigrams, int trigrams, BreakCaesar *breakCaesar) {
    Candidate *candidate = malloc(sizeof(Candidate));
    if (candidate == NULL) {
        fprintf(stderr, "Error: No se pudo crear el candidato\n");
        freeBreakCaesar(breakCaesar);
        exit(EXIT_FAILURE);
    }
    candidate->key = key;
    candidate->distance = distance;
    candidate->bigrams = bigrams;
    candidate->trigrams = trigrams;
    candidate->next = NULL;
    return candidate;
}

void addCandidate(Candidate *candidate, BreakCaesar *breakCaesar) {
    if (breakCaesar->top == NULL) {
        breakCaesar->top = candidate;
    } else {
        Candidate *current = breakCaesar->top;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = candidate;
    }
}




/* Prueba todas las claves (1 a ALPHABET_SIZE-1) y guarda cada candidato */
void startBruteForce(BreakCaesar *breakCaesar) {
    int len = strlen(breakCaesar->ciphertext);
    char *plaintext = malloc(len + 1);
    double frequencies[ALPHABET_SIZE];
    double distance;
    int key, bigrams, trigrams;

    if (!plaintext){
        errx(EXIT_FAILURE, "malloc failed");
    }

    
    for (key = 1; key <= ALPHABET_SIZE - 1; key++) {
        printf("Empezando con la clave %d\n", key);
        decryptWithKey(breakCaesar->ciphertext, plaintext, key);
        calculateFrequencies(plaintext, frequencies);
        distance = calculateEuclideanDistance(frequencies, FREQ_ENGLISH);
        printf("Calculando bigramas y trigramas con la clave %d\n", key);
        bigrams = countBigrams(plaintext);
        trigrams = countTrigrams(plaintext);
        printf("Mitad con la clave %d\n", key);
        Candidate *new_candidate = createCandidate(key, distance, bigrams, trigrams, breakCaesar);
        addCandidate(new_candidate, breakCaesar);
        printf("Terminando con la clave %d\n", key);
    }
    free(plaintext);
}

Candidate *copyCandidate(Candidate *candidate) {
    if (candidate == NULL) {
        return NULL;
    }
    Candidate *copy = malloc(sizeof(Candidate));
    if (copy == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    copy->key = candidate->key;
    copy->distance = candidate->distance;
    copy->bigrams = candidate->bigrams;
    copy->trigrams = candidate->trigrams;
    copy->next = NULL;
    return copy;
}

void
getBestCandidates(Candidate **bestDistance, Candidate **bestBigrams, Candidate **bestTrigrams, BreakCaesar *breakCaesar) {
    Candidate *current = breakCaesar->top;
    while (current != NULL) {
        if (*bestDistance == NULL || current->distance < (*bestDistance)->distance) {
            if (*bestDistance != NULL) {
                free(*bestDistance);
            }
            *bestDistance = copyCandidate(current);
        }
        if (*bestBigrams == NULL || current->bigrams > (*bestBigrams)->bigrams) {
            if (*bestBigrams != NULL) {
                free(*bestBigrams);
            }
            *bestBigrams = copyCandidate(current);
        }
        if (*bestTrigrams == NULL || current->trigrams > (*bestTrigrams)->trigrams) {
            if (*bestTrigrams != NULL) {
                free(*bestTrigrams);
            }
            *bestTrigrams = copyCandidate(current);
        }
        current = current->next;
    }
}

void addUniqueCandidates(Candidate *bestDistance, Candidate *bestBigrams, Candidate *bestTrigrams, BreakCaesar *filteredCandidates) {
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
}

void
filterCandidates(BreakCaesar **breakCaesar) {
    // Crear tres punteros para los mejores candidatos
    Candidate *bestDistance = NULL;
    Candidate *bestBigrams = NULL;
    Candidate *bestTrigrams = NULL;

    // Obtener los mejores candidatos
    getBestCandidates(&bestDistance, &bestBigrams, &bestTrigrams, *breakCaesar);

    // Imprimir los mejores candidatos
    // printf("Best distance: %f, correspondiente a la key: %d\n", bestDistance->distance, bestDistance->key);
    // printf("Best bigrams: %d, correspondiente a la key: %d\n", bestBigrams->bigrams, bestBigrams->key);
    // printf("Best trigrams: %d, correspondiente a la key: %d\n", bestTrigrams->trigrams, bestTrigrams->key);

    BreakCaesar *filteredCandidates = malloc(sizeof(BreakCaesar));
    if (!filteredCandidates) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    initBreakCaesar(filteredCandidates);
    filteredCandidates->ciphertext = strdup((*breakCaesar)->ciphertext);

    // Añadir candidatos únicos
    addUniqueCandidates(bestDistance, bestBigrams, bestTrigrams, filteredCandidates);

    // Liberar la memoria de los candidatos originales
    freeBreakCaesar(*breakCaesar);
    *breakCaesar = filteredCandidates;
}


void generateFiles(BreakCaesar *breakCaesar) {
    if (breakCaesar->ciphertext == NULL || breakCaesar->top == NULL) {
        fprintf(stderr, "Error: No hay datos para generar archivos\n");
        return;
    }

    size_t len = strlen(breakCaesar->ciphertext);
    char *plaintext = malloc(len + 1);
    if (!plaintext) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (Candidate *current = breakCaesar->top; current != NULL; current = current->next) {
        decryptWithKey(breakCaesar->ciphertext, plaintext, current->key);
        char filename[20];
        sprintf(filename, "key-%d.txt", current->key);
        FILE *file = fopen(filename, "w");
        if (file == NULL) {
            fprintf(stderr, "Error: No se pudo crear el archivo %s\n", filename);
            free(plaintext);
            freeBreakCaesar(breakCaesar);
            exit(EXIT_FAILURE);
        }
        fprintf(file, "%s", plaintext);
        fclose(file);
    }
    free(plaintext);
}

int main(int argc, char* argv[]){
    BreakCaesar *breakCaesar = malloc(sizeof(BreakCaesar));
    if (!breakCaesar) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    initBreakCaesar(breakCaesar);

    getMessage(breakCaesar);

    // printf("Ciphertext: %s\n", breakCaesar->ciphertext);

    /* Inicializar candidatos y ejecutar la fuerza bruta */
    startBruteForce(breakCaesar);

    filterCandidates(&breakCaesar);

    // Impresión de los mejores candidatos
    Candidate *current = breakCaesar->top;
    while (current != NULL) {
        printf("%d: %f, %d, %d\n", current->key, current->distance, current->bigrams, current->trigrams);
        current = current->next;
    }

    // Llamar a función para escribir en archivo con key
    generateFiles(breakCaesar);

    freeBreakCaesar(breakCaesar);

    exit(EXIT_SUCCESS);
}