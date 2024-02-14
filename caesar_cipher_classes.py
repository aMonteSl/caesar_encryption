#!/usr/bin/env python3

import sys
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
    def removeTheNameAndKey():
        # Remove first the key and then the name of the .py
        CaesarCipher.removeArgument(CaesarCipher.POS_DEFAULT_KEY_IN_ARGV)
        CaesarCipher.removeArgument(CaesarCipher.POS_DEFAULT_NAME_IN_ARGV)

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
    def getText():
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
        text_input = CaesarCipher.getText()
        return key, text_input
    
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

    

"""
class CaesarEncryption(CaesarCipher):
    # Error message for incorrect program launch
    INPUT_ERROR = "Wrong way to launch the program.\n"
    # Example of the correct way to launch the program
    EXAMPLE_STD_INPUT_LAUNCH = "Example1: echo 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG' | ./caesar 23\n"
    # Another example of how to lauch the program
    EXAMPLE_ARGV_LAUNCH = "Example2: ./caesar 23 THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG"

    def __init__(self):
        self.key = None
        self.white_text = None
        self.cipher_text = None

    def correctInput(self):
        return self.key and self.white_text

    def controlInput(self):
        if not CaesarEncryption.correctInput(self):
            err_msg = CaesarEncryption.INPUT_ERROR + CaesarEncryption.EXAMPLE_STD_INPUT_LAUNCH + CaesarEncryption.EXAMPLE_ARGV_LAUNCH
            CaesarCipher.manageWrongInput(err_msg)

        
    def getTextToEncryp(self):
        self.key, self.white_text = CaesarCipher.getInput()
        CaesarEncryption.controlInput(self)

    
    def encrypt(self):
        self.cipher_text = CaesarCipher.applyMovementKeyToString(self.white_text, self.key)

    def showCipherText(self):
        print(f"{self.cipher_text}", end='')

    def doEncryption(self):
        CaesarEncryption.getTextToEncryp(self)
        CaesarEncryption.encrypt(self)
        CaesarEncryption.showCipherText(self)
"""     


if __name__ == "__main__":
    #cifrador = CaesarEncryption()
    #cifrador.doEncryption()
    None