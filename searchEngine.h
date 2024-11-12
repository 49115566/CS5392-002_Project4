#ifndef SEARCHENGINE_H // Include guard to prevent multiple inclusions of this header file
#define SEARCHENGINE_H // Define the include guard

#include "wordmap.h" // Include the WordMap header file
#include <vector> // Include the vector library
#include <filesystem> // Include the filesystem library
#include <algorithm> // Include the algorithm library

class SearchEngine { // Define the SearchEngine class
public: // Public access specifier
    // Constructor
    SearchEngine(const std::string& folderPath, const std::string &filenamepath, const std::string& osavePath, const std::string& nsavePath, const std::string& wsavePath, const std::string& fsavePath);

    // Destructor
    ~SearchEngine(); // Destructor declaration

    // Function to search for a word
    std::vector<std::string> search(const std::string& searchTerms) const; // Function to search for a word

private: // Private access specifier
    WordMap wordMap; // Instance of WordMap

    // Helper function to process data
    void buildFromScratch(const std::string& folderPath); // Function to build data from scratch

    // Helper function to process search terms
    std::unordered_set<std::string> parse(const std::string& searchTerms) const; // Function to parse search terms

    // Helper function to process JSON data
    std::vector<std::unordered_set<std::string>> getRelevantData(const std::string& filePath) const; // Function to get relevant data from a file
};

#endif // SEARCHENGINE_H // End of include guard