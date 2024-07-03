#include <iostream>
#include <fstream>
#include <regex>
#include <string>

int main() {
    // Open the input file
    std::ifstream inputFile("Emily-Dickinson-Three-Series.txt");
    if (!inputFile) {
        std::cerr << "Error: Unable to open input file.\n";
        return 1;
    }

    // Create a regular expression to match words with all uppercase letters
    std::regex title_regex("[A-Z][A-Z]+.?");

    // Create an output file to write the modified text
    std::ofstream outputFile("Emily-Dickinson-No-Titles.txt");
    if (!outputFile) {
        std::cerr << "Error: Unable to open output file.\n";
        return 1;
    }

    // Read the file word by word
    std::string word;
    while (inputFile >> word) {
        // Check if the word matches the regex pattern
        if (!std::regex_match(word, title_regex)) {
            // Write the word to the output file if it does not match the pattern
            outputFile << word << " ";
        }
    }

    std::cout << "Text without matching words has been written to output.txt.\n";

    return 0;
}
