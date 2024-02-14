#!/usr/bin/env python3

import sys
import math
import select

class CaesarCipher:
    # Constants for ASCII values of 'A' and 'Z', and maximum key value
    A_ASCII = ord("A")
    Z_ASCII = ord("Z")
    MAX_KEY = Z_ASCII - A_ASCII + 1

    # Position of the key in sys.argv
    POS_DEFAULT_KEY_IN_ARGV = 1

    # Position of the .py name in sys.argv
    POS_DEFAULT_NAME_IN_ARGV = 0

    # Global constant to represent the minimum number of expected arguments
    MIN_ARGUMENTS = 2

    def __init__(self) -> None:
        pass

    @staticmethod
    def isLetterAsciiValue(ascii_value):
        # Rerturn if the ascci_value is between A-Z
        return (CaesarCipher.A_ASCII <= ascii_value <= CaesarCipher.Z_ASCII)

    @staticmethod
    def getValueAscii(letter):
        # Get the Ascii value of a char
        return ord(letter)
    
    @staticmethod
    def getCharAscii(ascii_value):
        # Get char of an Ascii_value
        return chr(ascii_value)
    
    @staticmethod
    def manageWrongInput(err_msg):
        # If the input is incorrect, inform the user and exit the program
        print(err_msg)
        exit()

    @staticmethod
    def getKey():
        # Control wrong inputs and retrieve the key from command line arguments
        try:
            # Default argv[POS_DEFAULT_KEY_IN_ARGV] type is a string, change it to
            # an int
            key = int(sys.argv[CaesarCipher.POS_DEFAULT_KEY_IN_ARGV])
        except (ValueError, IndexError):
            # Handle ValueError and IndexError
            key = None
        return key
    
    @staticmethod
    def removeArgument(index):
        # Delete arguments of argv
        del sys.argv[index]


    @staticmethod
    def removeTheName():
        CaesarCipher.removeArgument(CaesarCipher.POS_DEFAULT_NAME_IN_ARGV)

    @staticmethod
    def removeTheKey():
        CaesarCipher.removeArgument(CaesarCipher.POS_DEFAULT_KEY_IN_ARGV)

    @staticmethod
    def removeTheNameAndKey():
        # Remove first the key and then the name of the .py
        CaesarCipher.removeTheKey()
        CaesarCipher.removeTheName()

    @staticmethod
    def LengthArguments():
        # Return the length of sys.argv
        return len(sys.argv)


    @staticmethod
    def textInArguments():
        # Check if there is white_text provided as command-line arguments.
        return CaesarCipher.LengthArguments() > CaesarCipher.MIN_ARGUMENTS
    
    @staticmethod
    def getTextInArguments():
        # Get white_text input provided as command-line arguments.
        white_text = ''
        for i in range(CaesarCipher.LengthArguments()):
            word = sys.argv[i]
            white_text += word
            white_text += ' '
        return white_text
    
    @staticmethod
    def getTextEstandarInput():
        # Check if there's something to read from standard input
        if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
            # Read white_text from standard input
            text_input = sys.stdin.read()
            return text_input
        else:
            # No input available, return None or raise an exception as desired
            return None
        
    @staticmethod
    def getWhiteText():
        # Get the white_text input for Caesar cipher encryption.
        # This function retrieves the white_text input either from command-line arguments or standard input,
        # depending on whether white_text is provided as arguments. The white_text is
        # converted to uppercase for consistency.
        
        if CaesarCipher.textInArguments():
            CaesarCipher.removeTheNameAndKey()
            text_input = CaesarCipher.getTextInArguments()
        else:
            text_input = CaesarCipher.getTextEstandarInput()

        if text_input:
            text_input = text_input.upper()

        return text_input

    @staticmethod
    def getInput():
        # Get key and white_text input for Caesar cipher
        key = CaesarCipher.getKey()
        text_input = CaesarCipher.getWhiteText()
        return key, text_input
    
    @staticmethod
    def getEncryptedTex():
        if CaesarCipher.textInArguments():
            CaesarCipher.removeTheName()
            text_input = CaesarCipher.getTextInArguments()
        else:
            text_input = CaesarCipher.getTextEstandarInput()

        if text_input:
            text_input = text_input.upper()

        return text_input
    
# Todo calculo general de caesar
#-----------------------------------------------------------------------------------
    @staticmethod
    def addKeyToAsciiValue(ascii_value, key):
        # Shift ASCII value by the key, considering the alphabet boundaries
        return (ascii_value - CaesarCipher.A_ASCII + key) % CaesarCipher.MAX_KEY + CaesarCipher.A_ASCII

    @staticmethod
    def performCaesarCipherOperations(letter, key):
        # Get the ASCII value of the letter to encrypt
        ascii_value = CaesarCipher.getValueAscii(letter)

        if CaesarCipher.isLetterAsciiValue(ascii_value):
            # Perform Caesar cipher encryption
            encrypted_ascii_value = CaesarCipher.addKeyToAsciiValue(ascii_value, key)
        else:
            # Non-alphabetic characters remain unchanged
            encrypted_ascii_value = ascii_value

        # Return the char corresponding to the encrypted_ascii_value
        return CaesarCipher.getCharAscii(encrypted_ascii_value)
    
    @staticmethod
    def applyMovementKeyToString(white_text, key):
        # Initialize string_encryption
        string_encryption = ''

        for letter in white_text:
            # Encrypt each letter in the white_text using Caesar cipher
            string_encryption += CaesarCipher.performCaesarCipherOperations(letter, key)

        return string_encryption

    


class CaesarDesEncryption(CaesarCipher):
    # Error message for incorrect program launch
    INPUT_ERROR = "Wrong way to launch the program.\n"
    # Example of the correct way to launch the program
    EXAMPLE_STD_INPUT_LAUNCH = "Example1: echo 'QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD' | ./breakcaesar\n"
    # Another example of how to launch the program
    EXAMPLE_ARGV_LAUNCH = "Example2: ./breakcaesar 23 QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD"

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

    # Numero de indicadores
    NUM_INDICATORS = 3

    TOP_DISTANCE_INDEX = 0
    TOP_BIGRAM_INDEX = 1
    TOP_TRIGRAM_INDEX = 2

    def __init__(self):
        super().__init__()
        self.encryp_text = None
        self.top_keys = [{}, {}, {}]

    def correctInput(self):
        return self.encryp_text is not None
    
    def controlInput(self):
        if not self.correctInput():
            err_msg = self.INPUT_ERROR + self.EXAMPLE_STD_INPUT_LAUNCH + self.EXAMPLE_ARGV_LAUNCH
            CaesarCipher.manageWrongInput(err_msg)
            exit()



    def getTextToDesencryp(self):
        self.encryp_text = CaesarCipher.getEncryptedTex()
        CaesarDesEncryption.controlInput(self)

#--------------------------------------------------------------------------------------------------------
    def getPossibleText(self, key):
        key = -1 *key
        return CaesarCipher.applyMovementKeyToString(self.encryp_text, key)
    
    def frequenciesToVector(text, freq_letters):
        if len(text) == 0:
            return [0] * len(CaesarDesEncryption.ENGLISH_LETTER_FREQ)  # Devuelve un vector de ceros si el texto es vacío
        return [freq_letters[letra] / len(text) for letra in CaesarDesEncryption.ENGLISH_LETTER_FREQ.keys()]


    def frequenciesLetters(text):
        freq_letters = {letter: 0 for letter in CaesarDesEncryption.ENGLISH_LETTER_FREQ.keys()}

        for letter in text:
            if letter in freq_letters:
                freq_letters[letter] += 1

        freq_letters = CaesarDesEncryption.frequenciesToVector(text, freq_letters)
        return freq_letters

    def distanceBetweenText(freq_possi, freq_expected):
        return math.sqrt(sum((x - y) ** 2 for x, y in zip(freq_possi, freq_expected)))
    
    # euclideanTextDistance
    def euclideanTextDistance(possible_text):
        freq_possi = CaesarDesEncryption.frequenciesLetters(possible_text)
        freq_expected = list(CaesarDesEncryption.ENGLISH_LETTER_FREQ.values())
        return CaesarDesEncryption.distanceBetweenText(freq_possi, freq_expected)
    
    def countBigrams(possible_text):
        bigram_count = 0
        for i in range(len(possible_text) - 1):
            bigram = possible_text[i:i+2]
            if bigram in CaesarDesEncryption.ENGLISH_BIGRAM:
                bigram_count +=1
        return bigram_count
            

    def countTrigrams(possible_text):
        count = 0
        for i in range(len(possible_text) - 1):
            bigram = possible_text[i:i+3]  # Obtener el bigrama y convertirlo a mayúsculas
            if bigram in CaesarDesEncryption.ENGLISH_TRIGRAM:
                count += 1
        return count
    
    @staticmethod
    def evaluateThreeIndicators(possible_text):
        distance = CaesarDesEncryption.euclideanTextDistance(possible_text)
        bigram_count = CaesarDesEncryption.countBigrams(possible_text)
        trigram_count = CaesarDesEncryption.countTrigrams(possible_text)

        return distance, bigram_count, trigram_count
    
    def isBetterDistance(new_distance, current_distance, position):
        return new_distance < current_distance and position == CaesarDesEncryption.TOP_DISTANCE_INDEX

    def isBetterBigramCount(new_bigram_count, current_bigram_count, position):
        return new_bigram_count > current_bigram_count and position == CaesarDesEncryption.TOP_BIGRAM_INDEX

    def isBetterTrigramCount(new_trigram_count, current_trigram_count, position):
        return new_trigram_count > current_trigram_count and position == CaesarDesEncryption.TOP_TRIGRAM_INDEX

    def updateTopKeys(top_keys, key, distance, bigram_count, trigram_count):
        for i in range(CaesarDesEncryption.NUM_INDICATORS):
            if not top_keys[i] or \
            CaesarDesEncryption.isBetterDistance(distance, top_keys[i]['distance'], i) or \
            CaesarDesEncryption.isBetterBigramCount(bigram_count, top_keys[i]['bigram_count'], i) or \
            CaesarDesEncryption.isBetterTrigramCount(trigram_count, top_keys[i]['trigram_count'], i):
                top_keys[i] = {'key': key, 'distance': distance, 'bigram_count': bigram_count, 'trigram_count': trigram_count}
        return top_keys

    def getTheBestKeys(self):
        for key in range(1, CaesarCipher.MAX_KEY):
            possible_text = CaesarDesEncryption.getPossibleText(self, key)
            distance, bigram_count, trigram_count = CaesarDesEncryption.evaluateThreeIndicators(possible_text)
            self.top_keys = CaesarDesEncryption.updateTopKeys(self.top_keys, key, distance, bigram_count, trigram_count)

    def saveBestKeys(top_keys, encryp_text):
        for key_info in top_keys:
            key = key_info['key']
            if key:
                possible_text = CaesarDesEncryption.getPossibleText(key, encryp_text)
                with open(f'key-{key}.txt', 'w') as f:
                    f.write(possible_text)

    def printBestKeys(top_keys):
        printed_keys = set()  # Para mantener un registro de las claves ya impresas
        for key_info in top_keys:
            key = key_info['key']
            if key not in printed_keys:
                distance = key_info['distance']
                bigram_count = key_info['bigram_count']
                trigram_count = key_info['trigram_count']
                print(f"{key}: {distance:.2f}, {bigram_count}, {trigram_count}")
                printed_keys.add(key)

    def desencryp(self):
        CaesarDesEncryption.getTheBestKeys(self)
        CaesarDesEncryption.saveBestKeys(self.top_keys, self.encryp_text)
        CaesarCipher.printBestKeys(self.top_keys)

    def doDesEncryption(self):
        CaesarDesEncryption.getTextToDesencryp(self)
        CaesarDesEncryption.desencryp(self)




if __name__ == "__main__":
    #cifrador = CaesarEncryption()
    #cifrador.doEncryption()
    descifrador = CaesarDesEncryption()
    descifrador.doDesEncryption()