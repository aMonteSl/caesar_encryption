#!/usr/bin/env python3

# Author: Adrián Montes Linares

# Importing the CaesarCipher class from the caesar_cipher_classes module
from caesar_cipher_classes import CaesarCipher
import math


class CaesarDesEncryption(CaesarCipher):
    # Error message for incorrect program launch
    INPUT_ERROR = "Wrong way to launch the program.\n"
    # Example of the correct way to launch the program using standard input
    EXAMPLE_STD_INPUT_LAUNCH = "Example1: echo 'QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD' | ./breakcaesar\n"
    # Another example of how to launch the program using command-line arguments
    EXAMPLE_ARGV_LAUNCH = "Example2: ./breakcaesar QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD"

    # English letter frequencies
    ENGLISH_LETTER_FREQ = {
        'A': 0.08167,
        'B': 0.01492,
        'C': 0.02782,
        'D': 0.04253,
        'E': 0.12702,
        'F': 0.02228,
        'G': 0.02015,
        'H': 0.06094,
        'I': 0.06966,
        'J': 0.00153,
        'K': 0.00772,
        'L': 0.04025,
        'M': 0.02406,
        'N': 0.06749,
        'O': 0.07507,
        'P': 0.01929,
        'Q': 0.00095,
        'R': 0.05987,
        'S': 0.06327,
        'T': 0.09056,
        'U': 0.02758,
        'V': 0.00978,
        'W': 0.02360,
        'X': 0.00150,
        'Y': 0.01974,
        'Z': 0.00074
    }

    # Common English bigrams
    ENGLISH_BIGRAM = [
        "TH", "HE", "IN", "EN", "NT", "RE", "ER", "AN", "TI", "ES", "ON", "AT",
        "SE", "ND", "OR", "AR", "AL", "TE", "CO", "DE", "TO", "RA", "ET", "ED",
        "IT", "SA", "EM", "RO"
    ]

    # Common English trigrams
    ENGLISH_TRIGRAM = [
        "THE", "AND", "THA", "ENT", "ING", "ION", "TIO", "FOR", "NDE", "HAS",
        "NCE", "EDT", "TIS", "OFT", "STH", "MEN"
    ]

    # Number of top indicators
    NUM_INDICATORS = 3

    TOP_DISTANCE_INDEX = 0
    TOP_BIGRAM_INDEX = 1
    TOP_TRIGRAM_INDEX = 2

    def __init__(self):
        super().__init__()
        self.encryp_text = None
        self.top_keys = [{}, {}, {}]

    def correctInput(self):
        """
        Check if encrypted text is provided.
        Returns:
            bool: True if encrypted text is provided, False otherwise.
        """

        return self.encryp_text is not None

    def controlInput(self):
        """
        Check if input is correct, otherwise display error message and exit.
        """
        if not self.correctInput():
            err_msg = self.INPUT_ERROR + self.EXAMPLE_STD_INPUT_LAUNCH + self.EXAMPLE_ARGV_LAUNCH
            CaesarCipher.manageWrongInput(err_msg)
            exit()

    def getTextToDesencryp(self):
        """
        Get encrypted text.
        """
        self.encryp_text = CaesarCipher.getEncryptedTex()
        CaesarDesEncryption.controlInput(self)

    def getPossibleText(self, key):
        """
        Get the possible decrypted text by applying the specified key.
        Args:
            key (int): The key used for decryption.
        Returns:
            str: The decrypted text.
        """
        key = -1 * key
        return CaesarCipher.applyMovementKeyToString(self.encryp_text, key)

    def frequenciesToVector(text, freq_letters):
        """
        Convert the frequency of letters into a vector.
        Args:
            text (str): The text for which frequencies are calculated.
            freq_letters (dict): Dictionary containing frequencies of letters.
        Returns:
            list: List representing the vector of frequencies.
        """
        if len(text) == 0:
            # Return a vector of zeros if the text is empty
            return [0] * len(CaesarDesEncryption.ENGLISH_LETTER_FREQ)
        return [
            freq_letters[letra] / len(text)
            for letra in CaesarDesEncryption.ENGLISH_LETTER_FREQ.keys()
        ]

    def frequenciesLetters(text):
        """
        Calculate the frequency of letters in the text.
        Args:
            text (str): The text for which frequencies are calculated.
        Returns:
            list: List representing the frequencies of letters.
        """
        freq_letters = {
            letter: 0
            for letter in CaesarDesEncryption.ENGLISH_LETTER_FREQ.keys()
        }

        for letter in text:
            if letter in freq_letters:
                freq_letters[letter] += 1

        freq_letters = CaesarDesEncryption.frequenciesToVector(
            text, freq_letters)
        return freq_letters

    def distanceBetweenText(freq_possi, freq_expected):
        """
        Calculate the distance between two frequency vectors.
        Args:
            freq_possi (list): Frequency vector of the possible text.
            freq_expected (list): Frequency vector of the expected text.
        Returns:
            float: The Euclidean distance between the two frequency vectors.
        """
        return math.sqrt(
            sum((x - y)**2 for x, y in zip(freq_possi, freq_expected)))

    # euclideanTextDistance
    def euclideanTextDistance(possible_text):
        """
        Calculate the Euclidean distance between the frequency vector of possible text and the expected text.
        Args:
            possible_text (str): The possible decrypted text.
        Returns:
            float: The Euclidean distance between the frequency vectors.
        """
        freq_possi = CaesarDesEncryption.frequenciesLetters(possible_text)
        freq_expected = list(CaesarDesEncryption.ENGLISH_LETTER_FREQ.values())
        return CaesarDesEncryption.distanceBetweenText(freq_possi,
                                                       freq_expected)

    def countBigrams(possible_text):
        """
        Count the occurrences of common English bigrams in the text.
        Args:
            possible_text (str): The text in which bigrams are counted.
        Returns:
            int: The count of common English bigrams in the text.
        """
        bigram_count = 0
        for i in range(len(possible_text) - 1):
            bigram = possible_text[i:i + 2]
            if bigram in CaesarDesEncryption.ENGLISH_BIGRAM:
                bigram_count += 1
        return bigram_count

    def countTrigrams(possible_text):
        """
        Count the occurrences of common English trigrams in the text.
        Args:
            possible_text (str): The text in which trigrams are counted.
        Returns:
            int: The count of common English trigrams in the text.
        """
        count = 0
        for i in range(len(possible_text) - 1):
            # Get the bigram and convert it to uppercase
            bigram = possible_text[i:i + 3]
            if bigram in CaesarDesEncryption.ENGLISH_TRIGRAM:
                count += 1
        return count

    @staticmethod
    def evaluateThreeIndicators(possible_text):
        """
        Evaluate three indicators: Euclidean distance, bigram count, and trigram count.
        Args:
            possible_text (str): The possible decrypted text.
        Returns:
            tuple: A tuple containing the values of three indicators.
        """
        distance = CaesarDesEncryption.euclideanTextDistance(possible_text)
        bigram_count = CaesarDesEncryption.countBigrams(possible_text)
        trigram_count = CaesarDesEncryption.countTrigrams(possible_text)

        return distance, bigram_count, trigram_count

    def isBetterDistance(new_distance, current_distance, position):
        """
        Check if a new distance is better than the current one.
        Args:
            new_distance (float): The new distance to evaluate.
            current_distance (float): The current best distance.
            position (int): The index position of the indicator.
        Returns:
            bool: True if the new distance is better, False otherwise.
        """
        return new_distance < current_distance and position == CaesarDesEncryption.TOP_DISTANCE_INDEX

    def isBetterBigramCount(new_bigram_count, current_bigram_count, position):
        """
        Check if a new bigram count is better than the current one.
        Args:
            new_bigram_count (int): The new bigram count to evaluate.
            current_bigram_count (int): The current best bigram count.
            position (int): The index position of the indicator.
        Returns:
            bool: True if the new bigram count is better, False otherwise.
        """
        return new_bigram_count > current_bigram_count and position == CaesarDesEncryption.TOP_BIGRAM_INDEX

    def isBetterTrigramCount(new_trigram_count, current_trigram_count,
                             position):
        """
        Check if a new trigram count is better than the current one.
        Args:
            new_trigram_count (int): The new trigram count to evaluate.
            current_trigram_count (int): The current best trigram count.
            position (int): The index position of the indicator.
        Returns:
            bool: True if the new trigram count is better, False otherwise.
        """
        return new_trigram_count > current_trigram_count and position == CaesarDesEncryption.TOP_TRIGRAM_INDEX

    def updateTopKeys(top_keys, key, distance, bigram_count, trigram_count):
        """
        Update the top keys based on the evaluation of three indicators.
        Args:
            top_keys (list): List containing the top keys.
            key (int): The current key being evaluated.
            distance (float): The Euclidean distance of the possible decrypted text.
            bigram_count (int): The count of common English bigrams in the possible decrypted text.
            trigram_count (int): The count of common English trigrams in the possible decrypted text.
        Returns:
            list: Updated list of top keys.
        """
        for i in range(CaesarDesEncryption.NUM_INDICATORS):
            if not top_keys[i] or CaesarDesEncryption.isBetterDistance(
                    distance, top_keys[i]['distance'],
                    i) or CaesarDesEncryption.isBetterBigramCount(
                        bigram_count, top_keys[i]['bigram_count'],
                        i) or CaesarDesEncryption.isBetterTrigramCount(
                            trigram_count, top_keys[i]['trigram_count'], i):
                top_keys[i] = {
                    'key': key,
                    'distance': distance,
                    'bigram_count': bigram_count,
                    'trigram_count': trigram_count
                }
        return top_keys

    def getTheBestKeys(self):
        """
        Find the best keys by evaluating three indicators: Euclidean distance, bigram count, and trigram count.
        """
        best_keys = [{}, {}, {}]
        for key in range(1, CaesarCipher.MAX_KEY):
            possible_text = CaesarDesEncryption.getPossibleText(self, key)
            distance, bigram_count, trigram_count = CaesarDesEncryption.evaluateThreeIndicators(
                possible_text)
            best_keys = CaesarDesEncryption.updateTopKeys(
                best_keys, key, distance, bigram_count, trigram_count)
        self.top_keys = best_keys

    def saveBestKeys(self):
        """
        Save the decrypted texts corresponding to the best keys in separate files.
        """
        for key_info in self.top_keys:
            key = key_info['key']
            if key:
                possible_text = CaesarDesEncryption.getPossibleText(self, key)
                with open(f'key-{key}.txt', 'w') as f:
                    f.write(possible_text)

    def printBestKeys(top_keys):
        """
        Print the best keys and their corresponding indicators.
        Args:
            top_keys (list): List containing the top keys and their indicators.
        """
        printed_keys = set()  # To keep track of already printed keys
        for key_info in top_keys:
            key = key_info['key']
            if key not in printed_keys:
                distance = key_info['distance']
                bigram_count = key_info['bigram_count']
                trigram_count = key_info['trigram_count']
                print(f"{key}: {distance:.8f}, {bigram_count}, {trigram_count}")
                printed_keys.add(key)

    def desencryp(self):
        """
        Decrypt the encrypted text and print the best keys.
        """
        CaesarDesEncryption.getTheBestKeys(self)
        CaesarDesEncryption.saveBestKeys(self)
        CaesarDesEncryption.printBestKeys(self.top_keys)

    def doDesEncryption(self):
        """
        Perform the decryption process.
        """
        CaesarDesEncryption.getTextToDesencryp(self)
        CaesarDesEncryption.desencryp(self)


if __name__ == "__main__":
    desencryption = CaesarDesEncryption()
    desencryption.doDesEncryption()
