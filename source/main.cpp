#include <algorithm>
#include <iostream>
#include <fstream>
#include <list>
#include <cstring>
#include <regex>
#include "MurmurHash2.h"

class HashTable {
    private:
        struct WordCount
        {
            std::string word;
            int count;

            WordCount(std::string p_word, int p_count)
            : word(std::move(p_word)), count(p_count)
            {}
        };
        static const int hashGroups{100003}; // TODO: find good value for # of hashGroups
        std::list<WordCount> table[hashGroups]; // int here is number of times a word appears
    
    public:
        bool isEmpty();
        unsigned long hashFunction(std::string& key);
        void insertItem(std::string& key);
        void printLegomena(std::string output_file, int depth); // for Hapax depth = 1, Dis = 2 , etc...

};

bool HashTable::isEmpty()
{
    int sum{};
    for (int i{}; i < hashGroups; ++i)
    {
        sum += table[i].size();
    }
    if (!sum) {
        return true;
    }
    return false;
        

}

unsigned long HashTable::hashFunction(std::string& key)
{
    // uint64_t MurmurHash64A ( const void * key, int len, uint64_t seed )
    uint32_t seed{1};

    uint32_t hash{MurmurHash2(key.c_str(), static_cast<int>(key.size()), seed)};
    return hash % hashGroups;
}

void HashTable::insertItem(std::string& key)
{
    unsigned long hashValue = hashFunction(key);

    auto& cell = table[hashValue];
    auto bItr = begin(cell);
    bool keyExists = false;
    // Check if hashValue already in Table
    for (; bItr != end(cell); bItr++)
    {
        if (bItr -> word == key)
        {
            keyExists = true;
            bItr -> count += 1; // if this works correctly: if word already in table, increment int counter
            break; 
        }
    }
    // If hashValue exists, add new Word to back of list with count 1
    if (!keyExists)
    {
        cell.emplace_back(key, 1);
    }
    return;
}

// Qustion: is there a way of quickly finding entries of appropriate depth without iterating thru list?
// What if: entries get "flagged" when Count is 1, "unflagged" when Count exceed Depth
// What if: we hash by Word & Count???
        // re-hash each Word that exceeds Depth
void HashTable::printLegomena(std::string outputFilename, int depth)
{
    
    // Put HT into a std::vector, and then sort it
    std::vector<std::pair<std::string, int>> sortedList{};
    
    for (int i{}; i < hashGroups; i++)
    {
        if (table[i].size() == 0) continue; 

        auto bItr = table[i].begin();
        for (; bItr != table[i].end(); bItr++)
        {
            if (bItr->count <= depth)
            {
                sortedList.push_back(std::make_pair(bItr->word, bItr->count));
            }
        }
    }
    

    std::sort(sortedList.begin(), sortedList.end(), [](const std::pair<std::string, int> &x,
                                       const std::pair<std::string, int> &y)
    {
        return x.first < y.first; // define how we want to sort
    });

    // open outputFile and print to it

    std::ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open file " << outputFilename << " for writing" << std::endl;
        return;
    }
    // Print hapax legomena ordered by length
    for (const auto& pair : sortedList) {
        outputFile << pair.first << std::endl;
    }
    std::cout << "Hapax Legomena written to " << outputFilename << std::endl;

    return;
}

void word_check(HashTable& HT, std::string& word_by_word);

int main(int argc, char *argv[])
{

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_filename> <output_filename>, and then an option int legomena_depth" << std::endl;
        return 1;
    }
    
    // Open Text
    

    std::ifstream text{argv[1]};

    if (!text)
    {
        // Print an error and exit
        std::cerr << "Uh oh, that file could not be opened for reading!\n";
        return 1;
    }

    // Read Text word-by-word

    // Instantiate HashTable
    HashTable HT{};

    //read thru text
    std::string word_by_word{};
    while (text >> word_by_word)
        word_check(HT, word_by_word);


    //first get depth if it exists from argv, otherwise set equal to 1
    int depth{1};
    if (argv[3])
    {
        sscanf (argv[3],"%d",&depth);
    }
    std::string output_file{argv[2]};


    HT.printLegomena(output_file,depth);
}

void word_check(HashTable& HT, std::string& word_by_word)
{
    // This regex cleans the text beautifully; BUT: significantly slower

    // regex that matches only words, with possible dashes inside word
    std::regex word_regex("[a-zA-Z]+(-?[a-zA-Z]+)*");

    // Would it be faster to just iterate thru the string deleting bad characters?
    // How to catch dashed words doing this?
    // maybe if(string[i] == '-' && isAlpha(string[i+1]))
    // TODO try this ^^^

    // The Following is the Shakespeare Version that includes apostrophes
    // Which is BAD because it includes possessives and verb forms, but other misses "o'erpicturing"
    // If you're going to use make sure to find/replace for smart apostrophes
    // std::regex word_regex("['a-zA-Z]+(-?['a-zA-Z]*)*");

    // use this iterator to step thru every match the search gives
    // ^^ this is necessary because e.g. "end--begin" need to be processed as two words
    for (std::sregex_iterator it{word_by_word.begin(), word_by_word.end(), word_regex}, end{}; it != end; ++it)
    {
        std::smatch match = *it;
        std::string match_str = match.str();
        // put it in lower case
        std::transform(match_str.begin(), match_str.end(), match_str.begin(),
            [](unsigned char c){ return std::tolower(c); });
        HT.insertItem(match_str);
    }
   

    

    // TODO check if hash table size is reasonable

}
