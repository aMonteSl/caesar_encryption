#!/usr/bin/env python3

import sys
import math
import select

class CaesarCipher:
    # Position of the key in sys.argv
    POS_DEFAULT_KEY_IN_ARGV = 1

    # Position of the .py name in sys.argv
    POS_DEFAULT_NAME_IN_ARGV = 0

    # Global constant to represent the minimum number of expected arguments
    MIN_ARGUMENTS = 2

    def __init__(self) -> None:
        pass
    
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
        # Check if there is text provided as command-line arguments.
        return CaesarCipher.LengthArguments() > CaesarCipher.MIN_ARGUMENTS
    
    @staticmethod
    def getTextInArguments():
        # Get text input provided as command-line arguments.
        text = ''
        for i in range(CaesarCipher.LengthArguments()):
            word = sys.argv[i]
            text += word
            text += ' '
        return text
    
    @staticmethod
    def getTextEstandarInput():
        # Check if there's something to read from standard input
        if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
            # Read text from standard input
            text_input = sys.stdin.read()
            return text_input
        else:
            # No input available, return None or raise an exception as desired
            return None
        
    @staticmethod
    def getText():
        # Get the text input for Caesar cipher encryption.
        # This function retrieves the text input either from command-line arguments or standard input,
        # depending on whether text is provided as arguments. The text is
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
        # Get key and text input for Caesar cipher
        key = CaesarCipher.getKey()
        text_input = CaesarCipher.getText()
        return key, text_input
    
# Input
#------------------------------------------------------------------
# Todo lo de cifrar o descifrar, la parte comun de sumar o restar a una string (apllyCaesar, esto cambiarlo por otro nmbre como aplicarMovimientoKeyString)

class CaesarEncryption(CaesarCipher):
    # Error message for incorrect program launch
    INPUT_ERROR = "Wrong way to launch the program.\n"
    # Example of the correct way to launch the program
    EXAMPLE_STD_INPUT_LAUNCH = "Example1: echo 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG' | ./caesar 23\n"
    # Another example of how to lauch the program
    EXAMPLE_ARGV_LAUNCH = "Example2: ./caesar 23 THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG"

    def __init__(self):
        self.key = None
        self.text = None

    def controlInput(self, key, text):
        if key and text:
            self.key = key
            self.text = text
        else:
            err_msg = CaesarEncryption.INPUT_ERROR + CaesarEncryption.EXAMPLE_STD_INPUT_LAUNCH + CaesarEncryption.EXAMPLE_ARGV_LAUNCH
            CaesarCipher.manageWrongInput(err_msg)
    

        
    def getTextToEncryp(self):
        key, text = CaesarCipher.getInput()
        CaesarEncryption.controlInput(self, key, text)

    



if __name__ == "__main__":
    cifrador = CaesarEncryption()
    cifrador.getTextToEncryp()
    print(cifrador.key)
    print(cifrador.text)