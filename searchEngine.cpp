#include "searchEngine.h"
#include "rapidjson/document.h" // Assuming you are using a JSON library like RapidJSON
#include "rapidjson/istreamwrapper.h"
//#include <json/json.h> // Assuming you are using a JSON library like jsoncpp

namespace fs = std::filesystem;

SearchEngine::SearchEngine(const std::string& savePath, const std::string& folderPath) {
    if (!wordMap.load(savePath)) {
        buildFromScratch(folderPath);
        wordMap.save(savePath);
    }
}

SearchEngine::~SearchEngine() {
    // Destructor logic if needed
}

void SearchEngine::save(const std::string& savePath) const {
    wordMap.save(savePath);
}

std::unordered_set<std::string> SearchEngine::search(const std::string& searchTerms) const {
    std::unordered_set<std::string> result;
    std::unordered_set<std::string> terms = parse(searchTerms);

    if (terms.empty()) {
        return result;
    }

    bool firstTerm = true;
    for (const auto& term : terms) {
        std::unordered_set<std::string> files = wordMap.getFiles(term);
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

    return result;
}

void SearchEngine::buildFromScratch(const std::string& folderPath) {
    std::cout << "Reading JSONs..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            std::unordered_set<std::string> words = getRelevantWords(filePath);
            for (const auto& word : words) {
                wordMap.associate(word, filePath);
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
        terms.insert(term);
    }
    return terms;
}

void extractWords(const rapidjson::Value& value, std::unordered_set<std::string>& words) {
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
            words.insert(word);
        }
    } else if (value.IsObject()) {
        for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
            extractWords(it->value, words);
        }
    } else if (value.IsArray()) {
        for (auto& v : value.GetArray()) {
            extractWords(v, words);
        }
    }
}

std::unordered_set<std::string> SearchEngine::getRelevantWords(const std::string& filePath) const {
    std::unordered_set<std::string> words;
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