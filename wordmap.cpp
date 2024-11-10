#include "wordmap.h"

WordMap::WordMap() {}

WordMap::~WordMap() {}

void WordMap::associateOrg(const std::string &org, const std::string &filepath) {
    orgmap[org].insert(filepath);
}

void WordMap::associateName(const std::string &name, const std::string &filepath) {
    namemap[name].insert(filepath);
}

void WordMap::associateWord(const std::string &word, const std::string &filepath) {
    wordmap[word].insert(filepath);
}

void WordMap::disassociate(const std::string &word, const std::string &filepath) {
    if (orgmap.find(word) != orgmap.end()) {
        orgmap[word].erase(filepath);
        if (orgmap[word].empty()) {
            orgmap.erase(word);
        }
    }
    if (namemap.find(word) != namemap.end()) {
        namemap[word].erase(filepath);
        if (namemap[word].empty()) {
            namemap.erase(word);
        }
    }
    if (wordmap.find(word) != wordmap.end()) {
        wordmap[word].erase(filepath);
        if (wordmap[word].empty()) {
            wordmap.erase(word);
        }
    }
}

std::unordered_set<std::string> WordMap::getFilesByOrg(const std::string &word) const {
    if (orgmap.find(word) != orgmap.end()) {
        return orgmap.at(word);
    }
    return std::unordered_set<std::string>();
}

std::unordered_set<std::string> WordMap::getFilesByName(const std::string &word) const {
    if (namemap.find(word) != namemap.end()) {
        return namemap.at(word);
    }
    return std::unordered_set<std::string>();
}

std::unordered_set<std::string> WordMap::getFilesByWord(const std::string &word) const {
    if (wordmap.find(word) != wordmap.end()) {
        return wordmap.at(word);
    }
    return std::unordered_set<std::string>();
}

void WordMap::save(const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath) const {
    std::ofstream oofs(ofilepath);
    std::ofstream nofs(nfilepath);
    std::ofstream wofs(wfilepath);
    if (!oofs.is_open() || !nofs.is_open() || !wofs.is_open()) {
        std::cerr << "Failed to open file for saving: " << ofilepath << ", " << nfilepath << ", and/or" << wfilepath << std::endl;
        return;
    }
    
    std::cout << "Making save files..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto &pair : orgmap) {
        oofs << pair.first;
        for (const auto &file : pair.second) {
            oofs << " " << file;
        }
        oofs << "\n";
    }
    oofs.close();

    std::cout << "Orgmap saved!" << std::endl;

    for (const auto &pair : namemap) {
        nofs << pair.first;
        for (const auto &file : pair.second) {
            nofs << " " << file;
        }
        nofs << "\n";
    }
    nofs.close();

    std::cout << "Namemap saved!" << std::endl;

    for (const auto &pair : wordmap) {
        wofs << pair.first;
        for (const auto &file : pair.second) {
            wofs << " " << file;
        }
        wofs << "\n";
    }
    wofs.close();

    std::cout << "Wordmap saved!" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Files saved in " << duration.count() << " seconds.\n";
}

bool WordMap::load(const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath) {
    std::ifstream oifs(ofilepath);
    std::ifstream nifs(nfilepath);
    std::ifstream wifs(wfilepath);
    if (!oifs.is_open() || !nifs.is_open() || !wifs.is_open()) {
        std::cerr << "Failed to open file for loading: " << wfilepath << std::endl;
        return false;
    }

    std::cout << "Reading save file..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    orgmap.clear();
    std::string line;
    while (std::getline(oifs, line)) {
        std::istringstream iss(line);
        std::string word, file;
        iss >> word;
        while (iss >> file) {
            orgmap[word].insert(file);
        }
    }
    oifs.close();

    std::cout << "Orgmap loaded!" << std::endl;

    namemap.clear();
    while (std::getline(nifs, line)) {
        std::istringstream iss(line);
        std::string word, file;
        iss >> word;
        while (iss >> file) {
            namemap[word].insert(file);
        }
    }
    nifs.close();

    std::cout << "Namemap loaded!" << std::endl;

    wordmap.clear();
    while (std::getline(wifs, line)) {
        std::istringstream iss(line);
        std::string word, file;
        iss >> word;
        while (iss >> file) {
            wordmap[word].insert(file);
        }
    }
    wifs.close();

    std::cout << "Wordmap loaded!" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Save file read in " << duration.count() << " seconds.\n";

    return true;
}