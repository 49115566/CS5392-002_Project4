#ifndef _WORDMAP_H_ // Include guard to prevent multiple inclusions
#define _WORDMAP_H_ // Include guard to prevent multiple inclusions

#include <string> // Include string library
#include <unordered_map> // Include unordered_map library
#include <unordered_set> // Include unordered_set library
#include <fstream> // Include fstream library for file operations
#include <sstream> // Include sstream library for string stream operations
#include <iostream> // Include iostream library for input/output operations
#include <chrono> // Include chrono library for time operations

class WordMap { // Define WordMap class
private: // Private members
    std::unordered_map<std::string, int> toid; // Map from string to int
    std::unordered_map<int, std::string> tofile; // Map from int to string

    std::unordered_map<std::string, std::unordered_set<int>> orgmap; // Map from string to set of ints for organizations
    std::unordered_map<std::string, std::unordered_set<int>> namemap; // Map from string to set of ints for names
    std::unordered_map<std::string, std::unordered_set<int>> wordmap; // Map from string to set of ints for words

    std::unordered_map<std::string, std::unordered_map<int, int>> wordFrequency; // Map from word to map of file id to frequency
public: // Public members
    WordMap(); // Constructor
    ~WordMap(); // Destructor
    void associateOrg(const std::string &org, const std::string &filepath); // Associate organization with file path
    void associateName(const std::string &name, const std::string &filepath); // Associate name with file path
    void associateWord(const std::string &word, const std::string &filepath); // Associate word with file path
    void disassociate(const std::string &word, const std::string &filepath); // Disassociate word from file path
    std::unordered_map<std::string, int> getFilesByOrg(const std::string &word) const; // Get files by organization
    std::unordered_map<std::string, int> getFilesByName(const std::string &word) const; // Get files by name
    std::unordered_map<std::string, int> getOtherFilesByWord(const std::string &word) const; // Get other files by word
    std::unordered_map<std::string, int> getFilesByWord(const std::string &word) const; // Get files by word
    void save(const std::string &filenamepath, const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath, const std::string &ffilepath) const; // Save data to files
    bool load(const std::string &filenamepath, const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath, const std::string &ffilepath);
};

#endif // _WORDMAP_H_ // End of include guard