#!/usr/bin/env python3
import sys

POS_DEFAULT_KEY_IN_ARGV = 1
INPUT_ERROR = "Wrong way to launch the program."
EXAMPLE_LAUNCH = "Example: echo 'THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG'| ./caesar 23"

def manageWrongInput():
    # If not correct input, inform user and exit the program
    print(INPUT_ERROR + '\n' + EXAMPLE_LAUNCH)
    exit()

def getKey():
    # Control wrongs inputs
    try:
        # Default argv[POS_DEFAULT_KEY_IN_ARGV] type string, change to int
        key = int(sys.argv[POS_DEFAULT_KEY_IN_ARGV])
    except (ValueError, IndexError):
        # Manage ValueError, IndexError
        manageWrongInput()
    return key

def getTextInput():
    # Read text form standar input
    text_input = sys.stdin.read()
    # Pass to mayus all the text
    text_input = text_input.upper()
    return text_input

def controlInput():
    # Get key for caesar
    key = getKey()
    # Get text for caesar
    text_input = getTextInput()
    return key, text_input

def applyCaesar(key, text_input):
    None



if __name__ == "__main__":

    key, text_input = controlInput()
    applyCaesar(key, text_input)