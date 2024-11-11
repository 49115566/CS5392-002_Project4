#include "wordmap.h"

WordMap::WordMap() {}

WordMap::~WordMap() {}

void WordMap::associateOrg(const std::string &org, const std::string &filepath) {
    if(toid.find(filepath) == toid.end()) {
        toid[filepath] = toid.size();
        tofile[toid[filepath]] = filepath;
    }

    orgmap[org].insert(toid[filepath]);
}

void WordMap::associateName(const std::string &name, const std::string &filepath) {
    if(toid.find(filepath) == toid.end()) {
        toid[filepath] = toid.size();
        tofile[toid[filepath]] = filepath;
    }

    namemap[name].insert(toid[filepath]);
}

void WordMap::associateWord(const std::string &word, const std::string &filepath) {
    if(toid.find(filepath) == toid.end()) {
        toid[filepath] = toid.size();
        tofile[toid[filepath]] = filepath;
    }

    wordmap[word].insert(toid[filepath]);
}

void WordMap::disassociate(const std::string &word, const std::string &filepath) {
    if (orgmap.find(word) != orgmap.end()) {
        orgmap[word].erase(toid[filepath]);
        if (orgmap[word].empty()) {
            orgmap.erase(word);
        }
    }
    if (namemap.find(word) != namemap.end()) {
        namemap[word].erase(toid[filepath]);
        if (namemap[word].empty()) {
            namemap.erase(word);
        }
    }
    if (wordmap.find(word) != wordmap.end()) {
        wordmap[word].erase(toid[filepath]);
        if (wordmap[word].empty()) {
            wordmap.erase(word);
        }
    }
}

std::unordered_set<std::string> WordMap::getFilesByOrg(const std::string &word) const {
    std::unordered_set<std::string> files;
    if (orgmap.find(word) != orgmap.end()) {
        for (const auto &id : orgmap.at(word)) {
            files.insert(tofile.at(id));
        }
    }
    return files;
}
std::unordered_set<std::string> WordMap::getFilesByName(const std::string &name) const {
    std::unordered_set<std::string> files;
    if (namemap.find(name) != namemap.end()) {
        for (const auto &id : namemap.at(name)) {
            files.insert(tofile.at(id));
        }
    }
    return files;
}

std::unordered_set<std::string> WordMap::getOtherFilesByWord(const std::string &word) const {
    std::unordered_set<std::string> files;
    std::unordered_set<int> excluded_ids;
    
    if (wordmap.find(word) != wordmap.end()) {
        excluded_ids = wordmap.at(word);
    }
    
    for (const auto &pair : tofile) {
        if (excluded_ids.find(pair.first) == excluded_ids.end()) {
            files.insert(pair.second);
        }
    }
    
    return files;
}

std::unordered_set<std::string> WordMap::getFilesByWord(const std::string &word) const {
    std::unordered_set<std::string> files;
    if (wordmap.find(word) != wordmap.end()) {
        for (const auto &id : wordmap.at(word)) {
            files.insert(tofile.at(id));
        }
    }
    return files;
}

void WordMap::save(const std::string &filenamepath, const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath) const {
    std::ofstream fnofs(filenamepath);
    std::ofstream oofs(ofilepath);
    std::ofstream nofs(nfilepath);
    std::ofstream wofs(wfilepath);
    if (!fnofs.is_open() || !oofs.is_open() || !nofs.is_open() || !wofs.is_open()) {
        std::cerr << "Failed to open file for saving: " << filenamepath << ", " << ofilepath << ", " << nfilepath << ", and/or" << wfilepath << std::endl;
        return;
    }
    
    std::cout << "Making save files..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto &pair : tofile) {
        fnofs << pair.first << " " << pair.second << "\n";
    }
    
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

bool WordMap::load(const std::string &filenamepath, const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath) {
    std::ifstream fnifs(filenamepath);
    std::ifstream oifs(ofilepath);
    std::ifstream nifs(nfilepath);
    std::ifstream wifs(wfilepath);
    if (!fnifs.is_open() || !oifs.is_open() || !nifs.is_open() || !wifs.is_open()) {
        std::cerr << "Failed to open file for loading: " << filenamepath << ", " << ofilepath << ", " << nfilepath << ", and/or " << wfilepath << std::endl;
        return false;
    }

    std::cout << "Reading save file..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    toid.clear();
    tofile.clear();
    std::string line;
    while (std::getline(fnifs, line)) {
        std::istringstream iss(line);
        std::string id, file;
        iss >> id >> file;
        toid[file] = std::stoi(id);
        tofile[std::stoi(id)] = file;
    }
    fnifs.close();

    std::cout << "Filemap loaded!" << std::endl;

    orgmap.clear();
    while (std::getline(oifs, line)) {
        std::istringstream iss(line);
        std::string word;
        int id;
        iss >> word;
        while (iss >> id) {
            orgmap[word].insert(id);
        }
    }
    oifs.close();

    std::cout << "Orgmap loaded!" << std::endl;

    namemap.clear();
    while (std::getline(nifs, line)) {
        std::istringstream iss(line);
        std::string word;
        int id;
        iss >> word;
        while (iss >> id) {
            namemap[word].insert(id);
        }
    }
    nifs.close();

    std::cout << "Namemap loaded!" << std::endl;

    wordmap.clear();
    while (std::getline(wifs, line)) {
        std::istringstream iss(line);
        std::string word;
        int id;
        iss >> word;
        while (iss >> id) {
            wordmap[word].insert(id);
        }
    }
    wifs.close();

    std::cout << "Wordmap loaded!" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Save file read in " << duration.count() << " seconds.\n";

    return true;
}