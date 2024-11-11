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
    std::unordered_map<std::string, int> toid;
    std::unordered_map<int, std::string> tofile;

    std::unordered_map<std::string, std::unordered_set<int>> orgmap;
    std::unordered_map<std::string, std::unordered_set<int>> namemap;
    std::unordered_map<std::string, std::unordered_set<int>> wordmap;
public:
    WordMap();
    ~WordMap();
    void associateOrg(const std::string &org, const std::string &filepath);
    void associateName(const std::string &name, const std::string &filepath);
    void associateWord(const std::string &word, const std::string &filepath);
    void disassociate(const std::string &word, const std::string &filepath);
    std::unordered_set<std::string> getFilesByOrg(const std::string &word) const;
    std::unordered_set<std::string> getFilesByName(const std::string &word) const;
    std::unordered_set<std::string> getOtherFilesByWord(const std::string &word) const;
    std::unordered_set<std::string> getFilesByWord(const std::string &word) const;
    void save(const std::string &filenamepath, const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath) const;
    bool load(const std::string &filenamepath, const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath);
};

#endif // _WORDMAP_H_