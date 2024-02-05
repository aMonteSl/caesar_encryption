#!/usr/bin/env python3

"""
-------------------------------
File: caesar.py
Author: Adrián Montes Linares
Date: 05/02/24
Goal: The Caesar cipher encryption program takes a key and a text input, then applies the Caesar cipher algorithm to encrypt the text.
      The goal is to shift each letter in the text by the specified key, ensuring that non-alphabetic characters remain unchanged.
      The result is printed to the standard output.
-------------------------------
"""

import sys

# Constants for ASCII values of 'A' and 'Z', and maximum key value
A_ASCII = ord("A")
Z_ASCII = ord("Z")
MAX_KEY = Z_ASCII - A_ASCII + 1

# Position of the key in sys.argv
POS_DEFAULT_KEY_IN_ARGV = 1

# Error message for incorrect program launch
INPUT_ERROR = "Wrong way to launch the program."

# Example of the correct way to launch the program
EXAMPLE_LAUNCH = "Example: echo 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG' | ./caesar 23"


def manageWrongInput():
    # If the input is incorrect, inform the user and exit the program
    print(INPUT_ERROR + '\n' + EXAMPLE_LAUNCH)
    exit()


def getKey():
    # Control wrong inputs and retrieve the key from command line arguments
    try:
        # Default argv[POS_DEFAULT_KEY_IN_ARGV] type is a string, change it to
        # an int
        key = int(sys.argv[POS_DEFAULT_KEY_IN_ARGV])
    except (ValueError, IndexError):
        # Handle ValueError and IndexError
        manageWrongInput()
    return key


def getTextInput():
    # Read text from standard input and convert it to uppercase
    text_input = sys.stdin.read()
    text_input = text_input.upper()
    return text_input


def controlInput():
    # Get key and text input for Caesar cipher
    key = getKey()
    text_input = getTextInput()
    return key, text_input


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


if __name__ == "__main__":
    # Main program execution
    key, text_input = controlInput()
    encrypted_text = applyCaesar(key, text_input)
    print(encrypted_text)
