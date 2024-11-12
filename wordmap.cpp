#include "wordmap.h"

WordMap::WordMap() {} // Constructor

WordMap::~WordMap() {} // Destructor

void WordMap::associateOrg(const std::string &org, const std::string &filepath) {
    auto it = toid.find(filepath);
    if(it == toid.end()) {
        size_t id = toid.size();
        it = toid.emplace(filepath, id).first;
        tofile[id] = filepath;
    }

    orgmap[org].emplace(it->second);
    wordFrequency[org][it->second]++;
}

void WordMap::associateName(const std::string &name, const std::string &filepath) {
    auto it = toid.find(filepath);
    if(it == toid.end()) {
        size_t id = toid.size();
        it = toid.emplace(filepath, id).first;
        tofile[id] = filepath;
    }

    namemap[name].emplace(it->second);
    wordFrequency[name][it->second]++;
}

void WordMap::associateWord(const std::string &word, const std::string &filepath) {
    auto it = toid.find(filepath);
    if(it == toid.end()) {
        size_t id = toid.size();
        it = toid.emplace(filepath, id).first;
        tofile[id] = filepath;
    }

    wordmap[word].emplace(it->second);
    wordFrequency[word][it->second]++;
}

void WordMap::disassociate(const std::string &word, const std::string &filepath) {
    if (orgmap.find(word) != orgmap.end()) { // If word is found in orgmap
        orgmap[word].erase(toid[filepath]); // Remove id associated with filepath
        if (orgmap[word].empty()) { // If no more ids are associated with word
            orgmap.erase(word); // Remove word from orgmap
        }
    }
    if (namemap.find(word) != namemap.end()) { // If word is found in namemap
        namemap[word].erase(toid[filepath]); // Remove id associated with filepath
        if (namemap[word].empty()) { // If no more ids are associated with word
            namemap.erase(word); // Remove word from namemap
        }
    }
    if (wordmap.find(word) != wordmap.end()) { // If word is found in wordmap
        wordmap[word].erase(toid[filepath]); // Remove id associated with filepath
        if (wordmap[word].empty()) { // If no more ids are associated with word
            wordmap.erase(word); // Remove word from wordmap
        }
    }
}

std::unordered_map<std::string, int> WordMap::getFilesByOrg(const std::string &word) const {
    std::unordered_map<std::string, int> files;
    if (orgmap.find(word) != orgmap.end()) {
        for (const auto &id : orgmap.at(word)) {
            files[tofile.at(id)] = wordFrequency.at(word).at(id);
        }
    }
    return files;
}

std::unordered_map<std::string, int> WordMap::getFilesByName(const std::string &name) const {
    std::unordered_map<std::string, int> files;
    if (namemap.find(name) != namemap.end()) {
        for (const auto &id : namemap.at(name)) {
            files[tofile.at(id)] = wordFrequency.at(name).at(id);
        }
    }
    return files;
}

std::unordered_map<std::string, int> WordMap::getOtherFilesByWord(const std::string &word) const {
    std::unordered_map<std::string, int> files; // Map to store filepaths and their weights
    std::unordered_set<int> excluded_ids; // Set to store excluded ids
    
    if (wordmap.find(word) != wordmap.end()) { // If word is found in wordmap
        excluded_ids = wordmap.at(word); // Get ids associated with word
    }
    
    for (const auto &pair : tofile) { // For each id-filepath pair
        if (excluded_ids.find(pair.first) == excluded_ids.end()) { // If id is not in excluded_ids
            files[pair.second] = wordFrequency.at(word).at(pair.first); // Insert filepath and its weight
        }
    }
    
    return files; // Return map of filepaths and their weights
}

std::unordered_map<std::string, int> WordMap::getFilesByWord(const std::string &word) const {
    std::unordered_map<std::string, int> files;
    if (wordmap.find(word) != wordmap.end()) {
        for (const auto &id : wordmap.at(word)) {
            files[tofile.at(id)] = wordFrequency.at(word).at(id);
        }
    }
    return files;
}

void WordMap::save(const std::string &filenamepath, const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath, const std::string &ffilepath) const {
    std::ofstream fnofs(filenamepath); // Open file for saving filemap
    std::ofstream oofs(ofilepath); // Open file for saving orgmap
    std::ofstream nofs(nfilepath); // Open file for saving namemap
    std::ofstream wofs(wfilepath); // Open file for saving wordmap
    std::ofstream fofs(ffilepath); // Open file for saving wordFrequency
    if (!fnofs.is_open() || !oofs.is_open() || !nofs.is_open() || !wofs.is_open() || !fofs.is_open()) { // If any file fails to open
        std::cerr << "Failed to open file for saving: " << filenamepath << ", " << ofilepath << ", " << nfilepath << ", " << wfilepath << ", and/or " << ffilepath << std::endl; // Print error message
        return; // Return from function
    }
    
    std::cout << "Making save files..." << std::endl; // Print message
    auto start = std::chrono::high_resolution_clock::now(); // Get start time

    for (const auto &pair : tofile) { // For each id-filepath pair
        fnofs << pair.first << " " << pair.second << "\n"; // Save id and filepath
    }
    
    for (const auto &pair : orgmap) { // For each org-id set pair
        oofs << pair.first; // Save org
        for (const auto &file : pair.second) { // For each id in set
            oofs << " " << file; // Save id
        }
        oofs << "\n"; // New line
    }
    oofs.close(); // Close orgmap file

    std::cout << "Orgmap saved!" << std::endl; // Print message

    for (const auto &pair : namemap) { // For each name-id set pair
        nofs << pair.first; // Save name
        for (const auto &file : pair.second) { // For each id in set
            nofs << " " << file; // Save id
        }
        nofs << "\n"; // New line
    }
    nofs.close(); // Close namemap file

    std::cout << "Namemap saved!" << std::endl; // Print message

    for (const auto &pair : wordmap) { // For each word-id set pair
        wofs << pair.first; // Save word
        for (const auto &file : pair.second) { // For each id in set
            wofs << " " << file; // Save id
        }
        wofs << "\n"; // New line
    }
    wofs.close(); // Close wordmap file

    std::cout << "Wordmap saved!" << std::endl; // Print message

    for (const auto &pair : wordFrequency) { // For each word-frequency map pair
        fofs << pair.first; // Save word
        for (const auto &freq : pair.second) { // For each file id-frequency pair
            fofs << " " << freq.first << ":" << freq.second; // Save file id and frequency
        }
        fofs << "\n"; // New line
    }
    fofs.close(); // Close wordFrequency file

    std::cout << "WordFrequency saved!" << std::endl; // Print message

    auto end = std::chrono::high_resolution_clock::now(); // Get end time
    std::chrono::duration<double> duration = end - start; // Calculate duration
    std::cout << "Files saved in " << duration.count() << " seconds.\n"; // Print duration
}

bool WordMap::load(const std::string &filenamepath, const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath, const std::string &ffilepath) {
    std::ifstream fnifs(filenamepath); // Open file for loading filemap
    std::ifstream oifs(ofilepath); // Open file for loading orgmap
    std::ifstream nifs(nfilepath); // Open file for loading namemap
    std::ifstream wifs(wfilepath); // Open file for loading wordmap
    std::ifstream fifs(ffilepath); // Open file for loading wordFrequency
    if (!fnifs.is_open() || !oifs.is_open() || !nifs.is_open() || !wifs.is_open() || !fifs.is_open()) { // If any file fails to open
        std::cerr << "Failed to open file for loading: " << filenamepath << ", " << ofilepath << ", " << nfilepath << ", " << wfilepath << ", and/or " << ffilepath << std::endl; // Print error message
        return false; // Return false
    }

    std::cout << "Reading save file..." << std::endl; // Print message
    auto start = std::chrono::high_resolution_clock::now(); // Get start time

    toid.clear(); // Clear toid map
    tofile.clear(); // Clear tofile map
    std::string line; // String to store line
    while (std::getline(fnifs, line)) { // Read each line
        std::istringstream iss(line); // Create string stream
        std::string id, file; // Strings to store id and file
        iss >> id >> file; // Read id and file
        toid[file] = std::stoi(id); // Insert file and id into toid map
        tofile[std::stoi(id)] = file; // Insert id and file into tofile map
    }
    fnifs.close(); // Close filemap file

    std::cout << "Filemap loaded!" << std::endl; // Print message

    orgmap.clear(); // Clear orgmap
    while (std::getline(oifs, line)) { // Read each line
        std::istringstream iss(line); // Create string stream
        std::string word; // String to store word
        int id; // Integer to store id
        iss >> word; // Read word
        while (iss >> id) { // Read each id
            orgmap[word].insert(id); // Insert id into orgmap
        }
    }
    oifs.close(); // Close orgmap file

    std::cout << "Orgmap loaded!" << std::endl; // Print message

    namemap.clear(); // Clear namemap
    while (std::getline(nifs, line)) { // Read each line
        std::istringstream iss(line); // Create string stream
        std::string word; // String to store word
        int id; // Integer to store id
        iss >> word; // Read word
        while (iss >> id) { // Read each id
            namemap[word].insert(id); // Insert id into namemap
        }
    }
    nifs.close(); // Close namemap file

    std::cout << "Namemap loaded!" << std::endl; // Print message

    wordmap.clear(); // Clear wordmap
    while (std::getline(wifs, line)) { // Read each line
        std::istringstream iss(line); // Create string stream
        std::string word; // String to store word
        int id; // Integer to store id
        iss >> word; // Read word
        while (iss >> id) { // Read each id
            wordmap[word].insert(id); // Insert id into wordmap
        }
    }
    wifs.close(); // Close wordmap file

    std::cout << "Wordmap loaded!" << std::endl; // Print message

    wordFrequency.clear(); // Clear wordFrequency
    while (std::getline(fifs, line)) { // Read each line
        std::istringstream iss(line); // Create string stream
        std::string word; // String to store word
        iss >> word; // Read word
        std::string freqPair; // String to store file id and frequency pair
        while (iss >> freqPair) { // Read each file id and frequency pair
            size_t pos = freqPair.find(':'); // Find position of colon
            int id = std::stoi(freqPair.substr(0, pos)); // Extract file id
            int freq = std::stoi(freqPair.substr(pos + 1)); // Extract frequency
            wordFrequency[word][id] = freq; // Insert file id and frequency into wordFrequency
        }
    }
    fifs.close(); // Close wordFrequency file

    std::cout << "WordFrequency loaded!" << std::endl; // Print message

    auto end = std::chrono::high_resolution_clock::now(); // Get end time
    std::chrono::duration<double> duration = end - start; // Calculate duration
    std::cout << "Save file read in " << duration.count() << " seconds.\n"; // Print duration

    return true; // Return true
}