#include "wordmap.h"

WordMap::WordMap() {} // Constructor

WordMap::~WordMap() {} // Destructor

void WordMap::associateOrg(const std::string &org, const std::string &filepath) {
    auto it = toid.find(filepath); // Find filepath in toid map
    if(it == toid.end()) { // If filepath is not found
        size_t id = toid.size(); // Get new id
        it = toid.emplace(filepath, id).first; // Insert filepath with new id
        tofile[id] = filepath; // Map id to filepath
    }

    orgmap[org].emplace(it->second); // Associate org with id
}

void WordMap::associateName(const std::string &name, const std::string &filepath) {
    auto it = toid.find(filepath); // Find filepath in toid map
    if(it == toid.end()) { // If filepath is not found
        size_t id = toid.size(); // Get new id
        it = toid.emplace(filepath, id).first; // Insert filepath with new id
        tofile[id] = filepath; // Map id to filepath
    }

    namemap[name].emplace(it->second); // Associate name with id
}

void WordMap::associateWord(const std::string &word, const std::string &filepath) {
    auto it = toid.find(filepath); // Find filepath in toid map
    if(it == toid.end()) { // If filepath is not found
        size_t id = toid.size(); // Get new id
        it = toid.emplace(filepath, id).first; // Insert filepath with new id
        tofile[id] = filepath; // Map id to filepath
    }

    wordmap[word].emplace(it->second); // Associate word with id
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

std::unordered_set<std::string> WordMap::getFilesByOrg(const std::string &word) const {
    std::unordered_set<std::string> files; // Set to store filepaths
    if (orgmap.find(word) != orgmap.end()) { // If word is found in orgmap
        for (const auto &id : orgmap.at(word)) { // For each id associated with word
            files.insert(tofile.at(id)); // Insert filepath associated with id
        }
    }
    return files; // Return set of filepaths
}

std::unordered_set<std::string> WordMap::getFilesByName(const std::string &name) const {
    std::unordered_set<std::string> files; // Set to store filepaths
    if (namemap.find(name) != namemap.end()) { // If name is found in namemap
        for (const auto &id : namemap.at(name)) { // For each id associated with name
            files.insert(tofile.at(id)); // Insert filepath associated with id
        }
    }
    return files; // Return set of filepaths
}

std::unordered_set<std::string> WordMap::getOtherFilesByWord(const std::string &word) const {
    std::unordered_set<std::string> files; // Set to store filepaths
    std::unordered_set<int> excluded_ids; // Set to store excluded ids
    
    if (wordmap.find(word) != wordmap.end()) { // If word is found in wordmap
        excluded_ids = wordmap.at(word); // Get ids associated with word
    }
    
    for (const auto &pair : tofile) { // For each id-filepath pair
        if (excluded_ids.find(pair.first) == excluded_ids.end()) { // If id is not in excluded_ids
            files.insert(pair.second); // Insert filepath
        }
    }
    
    return files; // Return set of filepaths
}

std::unordered_set<std::string> WordMap::getFilesByWord(const std::string &word) const {
    std::unordered_set<std::string> files; // Set to store filepaths
    if (wordmap.find(word) != wordmap.end()) { // If word is found in wordmap
        for (const auto &id : wordmap.at(word)) { // For each id associated with word
            files.insert(tofile.at(id)); // Insert filepath associated with id
        }
    }
    return files; // Return set of filepaths
}

void WordMap::save(const std::string &filenamepath, const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath) const {
    std::ofstream fnofs(filenamepath); // Open file for saving filemap
    std::ofstream oofs(ofilepath); // Open file for saving orgmap
    std::ofstream nofs(nfilepath); // Open file for saving namemap
    std::ofstream wofs(wfilepath); // Open file for saving wordmap
    if (!fnofs.is_open() || !oofs.is_open() || !nofs.is_open() || !wofs.is_open()) { // If any file fails to open
        std::cerr << "Failed to open file for saving: " << filenamepath << ", " << ofilepath << ", " << nfilepath << ", and/or" << wfilepath << std::endl; // Print error message
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

    auto end = std::chrono::high_resolution_clock::now(); // Get end time
    std::chrono::duration<double> duration = end - start; // Calculate duration
    std::cout << "Files saved in " << duration.count() << " seconds.\n"; // Print duration
}

bool WordMap::load(const std::string &filenamepath, const std::string &ofilepath, const std::string &nfilepath, const std::string &wfilepath) {
    std::ifstream fnifs(filenamepath); // Open file for loading filemap
    std::ifstream oifs(ofilepath); // Open file for loading orgmap
    std::ifstream nifs(nfilepath); // Open file for loading namemap
    std::ifstream wifs(wfilepath); // Open file for loading wordmap
    if (!fnifs.is_open() || !oifs.is_open() || !nifs.is_open() || !wifs.is_open()) { // If any file fails to open
        std::cerr << "Failed to open file for loading: " << filenamepath << ", " << ofilepath << ", " << nfilepath << ", and/or " << wfilepath << std::endl; // Print error message
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

    auto end = std::chrono::high_resolution_clock::now(); // Get end time
    std::chrono::duration<double> duration = end - start; // Calculate duration
    std::cout << "Save file read in " << duration.count() << " seconds.\n"; // Print duration

    return true; // Return true
}