#!/usr/bin/env python3

from caesar_cipher_classes import CaesarCipher

class CaesarEncryption(CaesarCipher):
    # Error message for incorrect program launch
    INPUT_ERROR = "Wrong way to launch the program.\n"
    # Example of the correct way to launch the program
    EXAMPLE_STD_INPUT_LAUNCH = "Example1: echo 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG' | ./caesar 23\n"
    # Another example of how to launch the program
    EXAMPLE_ARGV_LAUNCH = "Example2: ./caesar 23 THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG"

    def __init__(self):
        super().__init__()
        self.key = None
        self.white_text = None
        self.cipher_text = None

    def correctInput(self):
        return self.key is not None and self.white_text is not None

    def controlInput(self):
        if not self.correctInput():
            err_msg = self.INPUT_ERROR + self.EXAMPLE_STD_INPUT_LAUNCH + self.EXAMPLE_ARGV_LAUNCH
            CaesarCipher.manageWrongInput(err_msg)
            exit()

    def getTextToEncrypt(self):
        self.key, self.white_text = CaesarCipher.getInput()
        self.controlInput()

    def encrypt(self):
        self.cipher_text = CaesarCipher.applyMovementKeyToString(self.white_text, self.key)

    def showCipherText(self):
        print(self.cipher_text, end='')

    def doEncryption(self):
        CaesarEncryption.getTextToEncrypt(self)
        CaesarEncryption.encrypt(self)
        CaesarEncryption.showCipherText(self)

if __name__ == "__main__":
    cipher = CaesarEncryption()
    cipher.doEncryption()
