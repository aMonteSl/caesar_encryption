#!/usr/bin/env python3

import sys
import math

# Frecuencia de letras en inglés actualizadas
ENGLISH_LETTER_FREQ = {
    'A': 0.08167, 'B': 0.01492, 'C': 0.02782, 'D': 0.04253, 'E': 0.12702,
    'F': 0.02228, 'G': 0.02015, 'H': 0.06094, 'I': 0.06966, 'J': 0.00153,
    'K': 0.00772, 'L': 0.04025, 'M': 0.02406, 'N': 0.06749, 'O': 0.07507,
    'P': 0.01929, 'Q': 0.00095, 'R': 0.05987, 'S': 0.06327, 'T': 0.09056,
    'U': 0.02758, 'V': 0.00978, 'W': 0.02360, 'X': 0.00150, 'Y': 0.01974,
    'Z': 0.00074
}



# Bigramas más comunes en inglés
ENGLISH_BIGRAM = [
    "TH", "HE", "IN", "EN", "NT", "RE", "ER", "AN", "TI", "ES", 
    "ON", "AT", "SE", "ND", "OR", "AR", "AL", "TE", "CO", "DE", 
    "TO", "RA", "ET", "ED", "IT", "SA", "EM", "RO"
]


# Trigramas más comunes en inglés
ENGLISH_TRIGRAM = [
    "THE", "AND", "THA", "ENT", "ING", "ION", "TIO", "FOR", 
    "NDE", "HAS", "NCE", "EDT", "TIS", "OFT", "STH", "MEN"
]

# Constants for ASCII values of 'A' and 'Z', and maximum key value (26)
A_ASCII = ord("A")
Z_ASCII = ord("Z")
MAX_KEY = Z_ASCII - A_ASCII + 1

# Global constant to represent the minimum number of expected arguments
MIN_ARGUMENTS = 1

# Position of the .py name in sys.argv
POS_DEFAULT_NAME_IN_ARGV = 0

def removeArgument(index):
    # Delete arguments of argv
    del sys.argv[index]

def LengthArguments():
    # Return the length of sys.argv
    return len(sys.argv)

def textInArguments():
    # Check if there is text provided as command-line arguments.
    return LengthArguments() > MIN_ARGUMENTS


def getTextInArguments():
    # Get text input provided as command-line arguments.
    text = ''
    for i in range(LengthArguments()):
        word = sys.argv[i]
        text += word
        text += ' '
    return text

def getTextEstandarInput():
    # Read text from standard input
    text_input = sys.stdin.read()

    return text_input


def removeTheNameAndKey():
    # Remove first the key and then the name of the .py

    removeArgument(POS_DEFAULT_NAME_IN_ARGV)


def getText():
    # Get the text input for Caesar cipher encryption.
    # This function retrieves the text input either from command-line arguments or standard input,
    # depending on whether text is provided as arguments. The text is
    # converted to uppercase for consistency.

    if textInArguments():
        removeTheNameAndKey()
        text_input = getTextInArguments()
    else:
        text_input = getTextEstandarInput()
    text_input = text_input.upper()

    return text_input

#------------------------------------------------------------------------------- Lo de arriba es conseguir un texto de la entrada estandar, queda manejar el caso de no texto de entrada y algun error

def getCharAscii(ascii_value):
    # Get char of an Ascii_value
    return chr(ascii_value)

def getValueAscii(letter):
    # Get the Ascii value of a char
    return ord(letter)

def isLetterAsciiValue(ascii_value):
    # Rerturn if the ascci_value is between A-Z
    return (A_ASCII <= ascii_value <= Z_ASCII)

def addKeyToAsciiValue(ascii_value, key):
    # Shift ASCII value by the key, considering the alphabet boundaries
    return (ascii_value - A_ASCII + key) % MAX_KEY + A_ASCII

def performCaesarCipherOperations(letter, key):
    # Get the ASCII value of the letter to encrypt
    ascii_value = getValueAscii(letter)

    if isLetterAsciiValue(ascii_value):
        # Perform Caesar cipher encryption
        encrypted_ascii_value = addKeyToAsciiValue(ascii_value, key)
    else:
        # Non-alphabetic characters remain unchanged
        encrypted_ascii_value = ascii_value

    # Return the char corresponding to the encrypted_ascii_value
    return getCharAscii(encrypted_ascii_value)

def applyCaesar(key, text_input):
    # Initialize string_encryption
    string_encryption = ''

    for letter in text_input:
        # Encrypt each letter in the text using Caesar cipher
        string_encryption += performCaesarCipherOperations(letter, key)

    return string_encryption

# Lo de arriba codigo copiado de caesar, lo de abajo es el bloque para que funcione decrypt_with_key
#------------------------------------------------------------------------------------------------------------------------------

def frequenciesToVector(text, freq_letters):
    if len(text) == 0:
        return [0] * len(ENGLISH_LETTER_FREQ)  # Devuelve un vector de ceros si el texto es vacío
    return [freq_letters[letra] / len(text) for letra in ENGLISH_LETTER_FREQ.keys()]


def frequenciesLetters(text):
    freq_letters = {letter: 0 for letter in ENGLISH_LETTER_FREQ.keys()}

    for letter in text:
        if letter in freq_letters:
            freq_letters[letter] += 1

    freq_letters = frequenciesToVector(text, freq_letters)
    return freq_letters

def distanceBetweenText(freq_possi, freq_expected):
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(freq_possi, freq_expected)))

# euclideanTextDistance
def euclideanTextDistance(possible_text):
    freq_possi = frequenciesLetters(possible_text)
    freq_expected = list(ENGLISH_LETTER_FREQ.values())
    return distanceBetweenText(freq_possi, freq_expected)



# Lo de arriba distnacia euclidea
#-----------------------------------------------------------------------------------------------------------------------------


def countBigrams(possible_text):
    bigram_count = 0
    for i in range(len(possible_text) - 1):
        bigram = possible_text[i:i+2]
        if bigram in ENGLISH_BIGRAM:
            bigram_count +=1
    return bigram_count
        

def countTrigrams(possible_text):
    count = 0
    for i in range(len(possible_text) - 1):
        bigram = possible_text[i:i+3]  # Obtener el bigrama y convertirlo a mayúsculas
        if bigram in ENGLISH_TRIGRAM:
            count += 1
    return count

# Lo de arriba es lo de los bigramas  trigramas
#-----------------------------------------------------------------------------------------------------------------------------
def caesar_breaker(ciphertext):
    top_keys = {}
    best_distance = 1
    best_key_distance = 0
    best_key_bigram = 0
    best_bigram_count = 0
    best_key_trigram = 0
    best_trigram_count = 0
    for k in range(1, MAX_KEY):
        # Para poder usar la misma funcion de addKeyToAsciiValue podemos pasar la k a negativa aqui, o en un futuro si hay una clase comun, y luego dos hijas (descriptar, encriptar) un atributo que las distinga y cambie a negativa la key en la funcion addKE...
        k = -1 *k
        possible_text = applyCaesar(k, ciphertext)
        print(f"CON LA CLAVE {abs(k)}")
        print(f"TEXTO CIFRADO: {ciphertext}")
        print(f"TEXTO DESCIFRADO: {possible_text}")
        
        # Lo de arriba en una funcion propia
        distance = euclideanTextDistance(possible_text)
        if distance < best_distance:
            best_distance = distance
            best_key_distance = k
        # Lo de arriba es todo lo de la distancia euclidea
        bigram_count = countBigrams(possible_text)
        print(f"Cuenta bigrama: {bigram_count}")
        if bigram_count > best_bigram_count:
            best_bigram_count = bigram_count
            best_key_bigram = k
        # Lo de arriba trigrama
        trigram_count = countTrigrams(possible_text)
        print(f"Cuenta trigramas: {trigram_count}")
        if trigram_count > best_trigram_count:
            best_trigram_count = trigram_count
            best_key_trigram = k
        
        print("--------------------------------------------")
        

        # ME queda guardar bien a los tres mejors, una forma una lista [] con diccionario dentro [{}, {}, {}] y que sean tres constantes globales las que tienen acceso a las tres posiciones (ejem: para la posicion 1 guardamos el mejor en distancia euclidea....)

    print(f"THE BEST KEY IN DISTANCE{best_key_distance}, the best key in bgrams{best_key_bigram}, and trigram {best_key_trigram}")
    



if __name__ == "__main__":
    ciphertext = getText()
    top = caesar_breaker(ciphertext)