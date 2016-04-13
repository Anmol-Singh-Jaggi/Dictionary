#!/usr/bin/env python3
import json
import random


def main():
    dictionary_file_path = 'dictionary.json'
    with open(dictionary_file_path, "r") as dictionary_file:
        dictionary = json.load(dictionary_file)

    while True:
        # Select a random word-meaning pair
        word_meaning = random.choice(list(dictionary.items()))
        # Select randomly whether to display word or meaning
        choice = random.randint(0, 1)
        print(word_meaning[choice])
        input('??')
        print(word_meaning[1 - choice])
        print('\n' + '-' * 30 + '\n')


if __name__ == '__main__':
    main()
