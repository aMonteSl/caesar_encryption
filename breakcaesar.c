#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <err.h>

#define BLOCK_SIZE 65536  /* 64 KB para lectura en bloques */

/* Definiciones y constantes */
enum {
    MIN_MINUS = 'a',
    MIN_UPPER = 'A',
    MAX_MINUS = 'z',
    MAX_UPPER = 'Z',
    DIFF_MINUS_TO_UPPER = 'A' - 'a',
    ALPHABET_SIZE = 'Z' - 'A' + 1  /* 26 */
};

/* Definir número de candidatos como constante de compilación (25 claves) */
enum { NUM_CANDIDATES = ALPHABET_SIZE - 1 };

/* Frecuencias de letras en inglés */
double FREQ_ENGLISH[ALPHABET_SIZE] = {
    0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228,
    0.02015, 0.06094, 0.06966, 0.00153, 0.00772, 0.04025,
    0.02406, 0.06749, 0.07507, 0.01929, 0.00095, 0.05987,
    0.06327, 0.09056, 0.02758, 0.00978, 0.02360, 0.00150,
    0.01974, 0.00074
};

/* Bigramas y trigramas comunes */
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

/* Estructura para almacenar los datos de un candidato */
typedef struct {
    int key;
    double sqDistance; /* Distancia euclidiana al cuadrado */
    int bigrams;
    int trigrams;
} Candidate;

/* Estructura para pasar datos a cada thread */
typedef struct {
    int key;                /* Clave a probar (1..25) */
    const char *ciphertext; /* Texto cifrado completo */
    Candidate *result;      /* Donde se almacenará el resultado */
} ThreadData;

/* Función para leer el archivo de entrada en bloques */
char *readFileStream(FILE *fp) {
    size_t capacity = BLOCK_SIZE;
    size_t size = 0;
    char *buffer = malloc(capacity + 1);
    if (!buffer) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    size_t bytesRead;
    while ((bytesRead = fread(buffer + size, 1, BLOCK_SIZE, fp)) > 0) {
        size += bytesRead;
        if (size + BLOCK_SIZE > capacity) {
            capacity *= 2;
            char *temp = realloc(buffer, capacity + 1);
            if (!temp) {
                perror("realloc");
                free(buffer);
                exit(EXIT_FAILURE);
            }
            buffer = temp;
        }
    }
    buffer[size] = '\0';
    return buffer;
}

/* Convierte a mayúsculas la cadena (modifica en sitio) */
void toUpper(char *message) {
    for (size_t i = 0; message[i] != '\0'; i++) {
        if (message[i] >= MIN_MINUS && message[i] <= MAX_MINUS) {
            message[i] += DIFF_MINUS_TO_UPPER;
        }
    }
}

/* Desencripta usando la clave dada */
void decryptWithKey(const char *ciphertext, char *plaintext, int key) {
    size_t len = strlen(ciphertext);
    for (size_t i = 0; i < len; i++) {
        char c = ciphertext[i];
        if (c >= MIN_UPPER && c <= MAX_UPPER) {
            plaintext[i] = (c - MIN_UPPER - key + ALPHABET_SIZE) % ALPHABET_SIZE + MIN_UPPER;
        } else {
            plaintext[i] = c;
        }
    }
    plaintext[len] = '\0';
}

/* Calcula las frecuencias de cada letra (mayúscula) en el texto */
void calculateFrequencies(const char *text, double *frequencies) {
    int count[ALPHABET_SIZE] = {0};
    int total = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] >= MIN_UPPER && text[i] <= MAX_UPPER) {
            count[text[i] - MIN_UPPER]++;
            total++;
        }
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        frequencies[i] = (total > 0) ? ((double) count[i] / total) : 0.0;
    }
}

/* Calcula la distancia euclidiana al cuadrado entre dos distribuciones */
double calculateSquaredEuclideanDistance(const double *freq1, const double *freq2) {
    double distance = 0.0;
    int i;
    for (i = 0; i < ALPHABET_SIZE; i++) {
        double diff = freq1[i] - freq2[i];
        distance += diff * diff;
    }
    return distance;
}

/* Cuenta bigramas en el texto comparando caracteres directamente */
int countBigrams(const char *text) {
    int count = 0;
    size_t len = strlen(text);
    size_t i;
    for (i = 0; i + 1 < len; i++) {
        int j;
        for (j = 0; j < NUM_BIGRAMS; j++) {
            if (text[i] == COMMON_BIGRAMS[j][0] && text[i+1] == COMMON_BIGRAMS[j][1]) {
                count++;
                break;
            }
        }
    }
    return count;
}

/* Cuenta trigramas en el texto comparando caracteres directamente */
int countTrigrams(const char *text) {
    int count = 0;
    size_t len = strlen(text);
    size_t i;
    for (i = 0; i + 2 < len; i++) {
        int j;
        for (j = 0; j < NUM_TRIGRAMS; j++) {
            if (text[i] == COMMON_TRIGRAMS[j][0] &&
                text[i+1] == COMMON_TRIGRAMS[j][1] &&
                text[i+2] == COMMON_TRIGRAMS[j][2]) {
                count++;
                break;
            }
        }
    }
    return count;
}

/* Función que ejecuta cada thread para probar una clave */
void *candidateThread(void *arg) {
    ThreadData *data = (ThreadData *) arg;
    int key = data->key;
    const char *ciphertext = data->ciphertext;
    size_t len = strlen(ciphertext);
    
    /* Reservar buffer para el texto desencriptado */
    char *plaintext = malloc(len + 1);
    if (!plaintext) {
        perror("malloc");
        pthread_exit(NULL);
    }
    
    /* Desencriptar */
    decryptWithKey(ciphertext, plaintext, key);
    
    /* Calcular frecuencias */
    double frequencies[ALPHABET_SIZE] = {0.0};
    calculateFrequencies(plaintext, frequencies);
    double sqDist = calculateSquaredEuclideanDistance(frequencies, FREQ_ENGLISH);
    
    /* Contar bigramas y trigramas */
    int bigrams = countBigrams(plaintext);
    int trigrams = countTrigrams(plaintext);
    
    /* Guardar resultado en la estructura Candidate */
    data->result->key = key;
    data->result->sqDistance = sqDist;
    data->result->bigrams = bigrams;
    data->result->trigrams = trigrams;
    
    free(plaintext);
    pthread_exit(NULL);
}

/* Genera los archivos de salida para cada candidato seleccionado */
void generateFiles(const char *ciphertext, Candidate *candidates, int numCandidates) {
    size_t len = strlen(ciphertext);
    char *plaintext = malloc(len + 1);
    if (!plaintext) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    int i;
    for (i = 0; i < numCandidates; i++) {
        int key = candidates[i].key;
        decryptWithKey(ciphertext, plaintext, key);
        char filename[30];
        sprintf(filename, "key-%d.txt", key);
        FILE *file = fopen(filename, "w");
        if (!file) {
            fprintf(stderr, "Error: no se pudo crear el archivo %s\n", filename);
            free(plaintext);
            exit(EXIT_FAILURE);
        }
        fprintf(file, "%s", plaintext);
        fclose(file);
    }
    free(plaintext);
}

/* Filtra los candidatos para obtener los mejores según cada criterio.
   Devuelve el número de candidatos únicos (1 a 3) en el arreglo outCandidates. */
int filterCandidates(Candidate *allCandidates, int total, Candidate *outCandidates) {
    Candidate bestDistance = allCandidates[0];
    Candidate bestBigrams = allCandidates[0];
    Candidate bestTrigrams = allCandidates[0];
    
    int i;
    for (i = 1; i < total; i++) {
        if (allCandidates[i].sqDistance < bestDistance.sqDistance) {
            bestDistance = allCandidates[i];
        }
        if (allCandidates[i].bigrams > bestBigrams.bigrams) {
            bestBigrams = allCandidates[i];
        }
        if (allCandidates[i].trigrams > bestTrigrams.trigrams) {
            bestTrigrams = allCandidates[i];
        }
    }
    
    /* Agregar candidatos únicos */
    int count = 0;
    outCandidates[count++] = bestDistance;
    if (bestBigrams.key != bestDistance.key) {
        outCandidates[count++] = bestBigrams;
    }
    if (bestTrigrams.key != bestDistance.key && bestTrigrams.key != bestBigrams.key) {
        outCandidates[count++] = bestTrigrams;
    }
    return count;
}

int main(void) {
    /* Leer entrada desde stdin en bloques */
    char *ciphertext = readFileStream(stdin);
    if (!ciphertext) {
        errx(EXIT_FAILURE, "No se pudo leer el mensaje cifrado.");
    }
    
    /* Convertir a mayúsculas */
    toUpper(ciphertext);
    
    /* Declarar arreglos de tamaño constante */
    Candidate candidates[NUM_CANDIDATES];
    pthread_t threads[NUM_CANDIDATES];
    ThreadData threadData[NUM_CANDIDATES];
    
    int i;
    /* Crear threads para cada clave */
    for (i = 0; i < NUM_CANDIDATES; i++) {
        threadData[i].key = i + 1; /* claves 1..25 */
        threadData[i].ciphertext = ciphertext;
        threadData[i].result = &candidates[i];
        if (pthread_create(&threads[i], NULL, candidateThread, &threadData[i]) != 0) {
            fprintf(stderr, "Error al crear thread para key %d\n", i + 1);
            free(ciphertext);
            exit(EXIT_FAILURE);
        }
    }
    
    /* Esperar a que terminen todos los threads */
    for (i = 0; i < NUM_CANDIDATES; i++) {
        pthread_join(threads[i], NULL);
    }
    
    /* Imprimir información de todos los candidatos (opcional) */
    printf("Resultados de la fuerza bruta:\n");
    for (i = 0; i < NUM_CANDIDATES; i++) {
        printf("Key %2d: Distancia^2 = %f, Bigrams = %d, Trigrams = %d\n",
               candidates[i].key, candidates[i].sqDistance,
               candidates[i].bigrams, candidates[i].trigrams);
    }
    
    /* Filtrar los mejores candidatos según distancia, bigrams y trigrams */
    Candidate filtered[3];
    int numFiltered = filterCandidates(candidates, NUM_CANDIDATES, filtered);
    
    printf("\nCandidatos filtrados:\n");
    for (i = 0; i < numFiltered; i++) {
        printf("Key %2d: Distancia^2 = %f, Bigrams = %d, Trigrams = %d\n",
               filtered[i].key, filtered[i].sqDistance,
               filtered[i].bigrams, filtered[i].trigrams);
    }
    
    /* Generar archivos de salida para los candidatos filtrados */
    generateFiles(ciphertext, filtered, numFiltered);
    
    free(ciphertext);
    return EXIT_SUCCESS;
}
