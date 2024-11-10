#include "wordmap.h"

WordMap::WordMap() {}

WordMap::~WordMap() {}

void WordMap::associate(const std::string &word, const std::string &filepath) {
    wordmap[word].insert(filepath);
}

void WordMap::disassociate(const std::string &word, const std::string &filepath) {
    if (wordmap.find(word) != wordmap.end()) {
        wordmap[word].erase(filepath);
        if (wordmap[word].empty()) {
            wordmap.erase(word);
        }
    }
}

std::unordered_set<std::string> WordMap::getFiles(const std::string &word) const {
    if (wordmap.find(word) != wordmap.end()) {
        return wordmap.at(word);
    }
    return std::unordered_set<std::string>();
}

void WordMap::save(const std::string &filepath) const {
    std::ofstream ofs(filepath);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open file for saving: " << filepath << std::endl;
        return;
    }
    
    std::cout << "Making save file..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto &pair : wordmap) {
        ofs << pair.first;
        for (const auto &file : pair.second) {
            ofs << " " << file;
        }
        ofs << "\n";
    }
    ofs.close();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Files saved in " << duration.count() << " seconds.\n";
}

bool WordMap::load(const std::string &filepath) {
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open file for loading: " << filepath << std::endl;
        return false;
    }

    std::cout << "Reading save file..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    wordmap.clear();
    std::string line;
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        std::string word, file;
        iss >> word;
        while (iss >> file) {
            wordmap[word].insert(file);
        }
    }
    ifs.close();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Save file read in " << duration.count() << " seconds.\n";

    return true;
}