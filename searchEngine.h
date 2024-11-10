#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "wordmap.h"
#include <filesystem>
#include <algorithm>

class SearchEngine {
public:
    // Constructor
    SearchEngine(const std::string& savePath, const std::string& folderPath);

    // Destructor
    ~SearchEngine();

    // Function to save data
    void save(const std::string& savePath) const;

    // Function to search for a word
    std::unordered_set<std::string> search(const std::string& searchTerms) const;

private:
    WordMap wordMap;

    // Helper function to process data
    void buildFromScratch(const std::string& folderPath);

    // Helper function to process search terms
    std::unordered_set<std::string> parse(const std::string& searchTerms) const;

    // Helper function to process JSON data
    std::unordered_set<std::string> getRelevantWords(const std::string& filePath) const;
};

#endif // SEARCHENGINE_H