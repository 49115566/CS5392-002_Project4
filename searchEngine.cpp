#include "searchEngine.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

namespace fs = std::filesystem;

SearchEngine::SearchEngine(const std::string& folderPath, const std::string &filenamepath, const std::string& osavePath, const std::string& nsavePath, const std::string& wsavePath) {
    if (!wordMap.load(filenamepath, osavePath, nsavePath, wsavePath)) {
        buildFromScratch(folderPath);
        wordMap.save(filenamepath, osavePath, nsavePath, wsavePath);
    }
}

SearchEngine::~SearchEngine() {}

std::unordered_set<std::string> SearchEngine::search(const std::string& searchTerms) const {
    auto start = std::chrono::high_resolution_clock::now();

    std::unordered_set<std::string> result;
    std::unordered_set<std::string> terms = parse(searchTerms);

    if (terms.empty()) {
        return result;
    }

    bool firstTerm = true;
    for (const auto& term : terms) {
        std::unordered_set<std::string> files;
        if (term.rfind("org:", 0) == 0) {
            files = wordMap.getFilesByOrg(term.substr(4));
        } else if (term.rfind("person:", 0) == 0) {
            std::cout << "Searching for " << term.substr(7) << std::endl;
            files = wordMap.getFilesByName(term.substr(7));
        } else if (term.rfind("-", 0) == 0) {
            files = wordMap.getOtherFilesByWord(term.substr(1));
        } else {
            files = wordMap.getFilesByWord(term);
        }

        if (firstTerm) {
            result = files;
            firstTerm = false;
        } else {
            std::unordered_set<std::string> intersection;
            for (const auto& file : result) {
                if (files.find(file) != files.end()) {
                    intersection.insert(file);
                }
            }
            result = intersection;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << result.size() << " results for " << searchTerms << " in " << duration.count() << " seconds.\n";

    return result;
}

void SearchEngine::buildFromScratch(const std::string& folderPath) {
    std::cout << "Reading JSONs..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();

            std::vector<std::unordered_set<std::string>> words = getRelevantData(filePath);
            for (const auto& word : words[0]) {
                std::string lowerWord = word;
                std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
                wordMap.associateOrg(lowerWord, filePath);
            }

            for (const auto& word : words[1]) {
                std::string lowerWord = word;
                std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
                wordMap.associateName(lowerWord, filePath);
            }

            for (const auto& word : words[2]) {
                std::string lowerWord = word;
                std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
                wordMap.associateWord(lowerWord, filePath);
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "JSONs read in " << duration.count() << " seconds.\n";
}

std::unordered_set<std::string> SearchEngine::parse(const std::string& searchTerms) const {
    std::unordered_set<std::string> terms;
    std::istringstream iss(searchTerms);
    std::string term;
    while (iss >> term) {
        std::transform(term.begin(), term.end(), term.begin(), ::tolower);
        terms.insert(term);
    }
    return terms;
}

void extractWords(const rapidjson::Value& value, std::vector<std::unordered_set<std::string>>& words, const std::string& currentPath = "") {
    if (value.IsString()) {
        std::istringstream iss(value.GetString());
        std::string word;
        while (iss >> word) {
            // Remove punctuation from the beginning and end of the word
            word.erase(word.begin(), std::find_if(word.begin(), word.end(), [](unsigned char ch) {
                return !std::ispunct(ch);
            }));
            word.erase(std::find_if(word.rbegin(), word.rend(), [](unsigned char ch) {
                return !std::ispunct(ch);
            }).base(), word.end());

            if (currentPath.find("entities.organizations") != std::string::npos && currentPath.find("name") != std::string::npos) {
                words[0].insert(word);
            } else if (currentPath.find("entities.persons") != std::string::npos && currentPath.find("name") != std::string::npos) {
                words[1].insert(word);
            }
            words[2].insert(word);
        }
    } else if (value.IsObject()) {
        for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
            extractWords(it->value, words, currentPath.empty() ? it->name.GetString() : currentPath + "." + it->name.GetString());
        }
    } else if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            extractWords(value[i], words, currentPath + "[" + std::to_string(i) + "]");
        }
    }
}

std::vector<std::unordered_set<std::string>> SearchEngine::getRelevantData(const std::string& filePath) const {
    std::vector<std::unordered_set<std::string>> words(3);
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return words;
    }

    rapidjson::IStreamWrapper isw(file);
    rapidjson::Document document;
    document.ParseStream(isw);

    if (document.HasParseError()) {
        std::cerr << "Failed to parse JSON file: " << filePath << std::endl;
        return words;
    }

    extractWords(document, words);

    return words;
}