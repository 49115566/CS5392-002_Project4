#ifndef _WORDMAP_H_
#define _WORDMAP_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

class WordMap {
private:
    std::unordered_map<std::string, std::unordered_set<std::string>> wordmap;
public:
    WordMap();
    ~WordMap();
    void associate(const std::string &word, const std::string &filepath);
    void disassociate(const std::string &word, const std::string &filepath);
    std::unordered_set<std::string> getFiles(const std::string &word) const;
    void save(const std::string &filepath) const;
    bool load(const std::string &filepath);
};

#endif // _WORDMAP_H_