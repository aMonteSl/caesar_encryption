#!/usr/bin/env python3

# Author: Adrián Montes Linares

# Importing the CaesarCipher class from the caesar_cipher_classes module
from caesar_cipher_classes import CaesarCipher


class CaesarEncryption(CaesarCipher):
    # Error message for incorrect program launch
    INPUT_ERROR = "Wrong way to launch the program.\n"
    # Example of the correct way to launch the program using standard input
    EXAMPLE_STD_INPUT_LAUNCH = "Example1: echo 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG' | ./caesar 23\n"
    # Another example of how to launch the program using command-line arguments
    EXAMPLE_ARGV_LAUNCH = "Example2: ./caesar 23 THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG"

    def __init__(self):
        super().__init__()
        self.key = None
        self.white_text = None
        self.cipher_text = None

    def correctInput(self):
        """
        Check if both key and text to encrypt are provided.
        Returns:
            bool: True if both key and text are provided, False otherwise.
        """
        return self.key is not None and self.white_text is not None

    def controlInput(self):
        """
        Check if input is correct, otherwise display error message and exit.
        """
        if not self.correctInput():
            err_msg = self.INPUT_ERROR + self.EXAMPLE_STD_INPUT_LAUNCH + self.EXAMPLE_ARGV_LAUNCH
            CaesarCipher.manageWrongInput(err_msg)
            exit()

    def getTextToEncrypt(self):
        """
        Get key and text to encrypt.
        """
        self.key, self.white_text = CaesarCipher.getInput()
        self.controlInput()

    def encrypt(self):
        """
        Encrypt the text using Caesar cipher algorithm.
        """
        self.cipher_text = CaesarCipher.applyMovementKeyToString(
            self.white_text, self.key)

    def showCipherText(self):
        """
        Display the encrypted text.
        """
        print(self.cipher_text, end='\n')

    def doEncryption(self):
        """
        Perform the encryption process.
        """
        CaesarEncryption.getTextToEncrypt(self)
        CaesarEncryption.encrypt(self)
        CaesarEncryption.showCipherText(self)


if __name__ == "__main__":
    # Create an instance of CaesarEncryption class and perform encryption
    cipher = CaesarEncryption()
    cipher.doEncryption()
