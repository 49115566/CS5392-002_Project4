#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "wordmap.h"
#include <vector>
#include <filesystem>
#include <algorithm>

class SearchEngine {
public:
    // Constructor
    SearchEngine(const std::string& folderPath, const std::string& filenamepath, const std::string& osavePath, const std::string& nsavePath, const std::string& wsavePath);

    // Destructor
    ~SearchEngine();

    // Function to search for a word
    std::unordered_set<std::string> search(const std::string& searchTerms) const;

private:
    WordMap wordMap;

    // Helper function to process data
    void buildFromScratch(const std::string& folderPath);

    // Helper function to process search terms
    std::unordered_set<std::string> parse(const std::string& searchTerms) const;

    // Helper function to process JSON data
    std::vector<std::unordered_set<std::string>> getRelevantData(const std::string& filePath) const;
};

#endif // SEARCHENGINE_H