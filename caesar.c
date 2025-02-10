#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>


// Pasar al ingles

enum {
    MAX_LINE = 1024,
    MIN_MINUS = 'a',
    MIN_UPPER = 'A',
    MAX_MINUS = 'z',
    MAX_UPPER = 'Z',
    DIFF_MINUS_TO_UPPER = 'A' - 'a'
};

// La estructura del cifrado caesar tendra la clave, el mensaje a cifrar y el mensaje cifrado
struct Caesar
{
    int key;
    char* message;
    char* cipher;
};
typedef struct Caesar Caesar;

void
freeCaesar(Caesar *caesar){
    if (caesar != NULL) {
        if (caesar->message != NULL){
            free(caesar->message);
        }
        if (caesar->cipher != NULL){
            free(caesar->cipher);
        }
        free(caesar);
    }
}

void
initCaesar(Caesar *caesar, char *key){
    caesar->message = NULL;
    caesar->cipher = NULL;

    caesar->key = atoi(key);
    if (caesar->key <= 0){
        freeCaesar(caesar);
        errx(EXIT_FAILURE, "atoi failed, key must be a positive integer");
    }
}

Caesar *
createCaesar(char *key){
    Caesar *caesar = malloc(sizeof(Caesar));
    if (caesar == NULL){
        errx(EXIT_FAILURE, "malloc failed");
    }
    
    initCaesar(caesar, key);

    return caesar;
}



void
readAllLines(Caesar *caesar)
{
    int size = 0;
    char *buffer = NULL;
    char line[MAX_LINE];

    while (fgets(line, MAX_LINE, stdin) != NULL) {
        int line_len = strlen(line);
        char *new_buffer = realloc(buffer, size + line_len + 1);
        if (new_buffer == NULL) {
            free(buffer);
            freeCaesar(caesar);
            errx(EXIT_FAILURE, "realloc failed");
        }
        buffer = new_buffer;
        strcpy(buffer + size, line);
        size += line_len;
    }

    if (buffer == NULL) {
        freeCaesar(caesar);
        errx(EXIT_FAILURE, "no message to cipher");
    }

    caesar->message = buffer;
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
getMessage(Caesar *caesar){
    readAllLines(caesar);
    if (caesar->message == NULL){
        freeCaesar(caesar);
        errx(EXIT_FAILURE, "no message to cipher");
    }
    toUpper(caesar->message);
}

void
cipherMessage(Caesar *caesar){
    caesar->cipher = strdup(caesar->message);
    if (caesar->cipher == NULL){
        freeCaesar(caesar);
        errx(EXIT_FAILURE, "strdup failed");
    }
    for (int i = 0; i < strlen(caesar->cipher); i++){
        if (caesar->cipher[i] >= MIN_UPPER && caesar->cipher[i] <= MAX_UPPER){
            caesar->cipher[i] = (caesar->cipher[i] - MIN_UPPER + caesar->key) % 26 + MIN_UPPER;
        }
    }
}

int main(int argc, char* argv[]){
    Caesar *caesar;

    if (argc != 2){
        fprintf(stderr, "Usage: %s key\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    caesar = createCaesar(argv[1]);

    // Obtener el mensaje a cifrar
    getMessage(caesar);

    // Cifrar el mensaje
    cipherMessage(caesar);

    // Imprimo el mensaje cifrado
    printf("%s\n", caesar->cipher);

    freeCaesar(caesar);
    exit(EXIT_SUCCESS);
}