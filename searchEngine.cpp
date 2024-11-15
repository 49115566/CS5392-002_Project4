#include "searchEngine.h" // Include the header file for the SearchEngine class
#include "rapidjson/document.h" // Include the RapidJSON document header
#include "rapidjson/istreamwrapper.h" // Include the RapidJSON input stream wrapper header

namespace fs = std::filesystem; // Create an alias for the std::filesystem namespace

SearchEngine::SearchEngine(const std::string& folderPath, const std::string &filenamepath, const std::string& osavePath, const std::string& nsavePath, const std::string& wsavePath, const std::string& fsavePath) {
    if (!wordMap.load(filenamepath, osavePath, nsavePath, wsavePath, fsavePath)) { // Load the word map, if it fails, build from scratch
        buildFromScratch(folderPath); // Build the word map from scratch
        wordMap.save(filenamepath, osavePath, nsavePath, wsavePath, fsavePath); // Save the word map to the specified paths
    }
}

SearchEngine::~SearchEngine() {} // Destructor for the SearchEngine class

std::vector<std::string> SearchEngine::search(const std::string& searchTerms) const {
    auto start = std::chrono::high_resolution_clock::now();

    std::unordered_map<std::string, int> result; // Map to store file paths and their relevancy scores
    std::unordered_set<std::string> terms = parse(searchTerms); // Parse the search terms

    if (terms.empty()) {
        return {};
    }

    for (const auto& term : terms) {
        std::unordered_map<std::string, int> files;
        if (term.rfind("org:", 0) == 0) {
            files = wordMap.getFilesByOrg(term.substr(4));
        } else if (term.rfind("person:", 0) == 0) {
            files = wordMap.getFilesByName(term.substr(7));
        } else {
            files = wordMap.getFilesByWord(term);
        }

        for (const auto& file : files) {
            result[file.first] += file.second; // Aggregate relevancy scores
        }
    }

    // Convert the result map to a vector and sort by relevancy score
    std::vector<std::pair<std::string, int>> sortedResults(result.begin(), result.end());
    std::sort(sortedResults.begin(), sortedResults.end(), [](const auto& a, const auto& b) {
        return b.second < a.second; // Sort in descending order of relevancy score
    });

    std::vector<std::string> finalResults;
    for (const auto& file : sortedResults) {
        finalResults.push_back(file.first);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << finalResults.size() << " results for " << searchTerms << " in " << duration.count() << " seconds.\n";

    return finalResults;
}

void SearchEngine::buildFromScratch(const std::string& folderPath) {
    std::cout << "Reading JSONs..." << std::endl; // Print a message indicating the start of JSON reading
    auto start = std::chrono::high_resolution_clock::now(); // Start the timer

    for (const auto& entry : fs::recursive_directory_iterator(folderPath)) { // Iterate over each file in the folder
        if (entry.is_regular_file()) { // Check if the entry is a regular file
            std::string filePath = entry.path().string(); // Get the file path as a string

            std::vector<std::unordered_set<std::string>> words = getRelevantData(filePath); // Get the relevant data from the file
            for (const auto& word : words[0]) { // Iterate over the organization words
                std::string lowerWord = word; // Copy the word to a new string
                std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower); // Convert the word to lowercase
                wordMap.associateOrg(lowerWord, filePath); // Associate the word with the file path in the word map
            }

            for (const auto& word : words[1]) { // Iterate over the person words
                std::string lowerWord = word; // Copy the word to a new string
                std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower); // Convert the word to lowercase
                wordMap.associateName(lowerWord, filePath); // Associate the word with the file path in the word map
            }

            for (const auto& word : words[2]) { // Iterate over the other words
                std::string lowerWord = word; // Copy the word to a new string
                std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower); // Convert the word to lowercase
                wordMap.associateWord(lowerWord, filePath); // Associate the word with the file path in the word map
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now(); // End the timer
    std::chrono::duration<double> duration = end - start; // Calculate the duration
    std::cout << "JSONs read in " << duration.count() << " seconds.\n"; // Print the duration
}

std::unordered_set<std::string> SearchEngine::parse(const std::string& searchTerms) const {
    std::unordered_set<std::string> terms; // Initialize the set of terms
    std::istringstream iss(searchTerms); // Create an input string stream from the search terms
    std::string term; // Initialize a string to hold each term
    while (iss >> term) { // Iterate over each term in the input stream
        std::transform(term.begin(), term.end(), term.begin(), ::tolower); // Convert the term to lowercase
        terms.insert(term); // Insert the term into the set
    }
    return terms; // Return the set of terms
}

void extractWords(const rapidjson::Value& value, std::vector<std::unordered_set<std::string>>& words, const std::string& currentPath = "") {
    if (value.IsString()) { // Check if the value is a string
        std::istringstream iss(value.GetString()); // Create an input string stream from the value
        std::string word; // Initialize a string to hold each word
        while (iss >> word) { // Iterate over each word in the input stream
            // Remove punctuation from the beginning and end of the word
            word.erase(word.begin(), std::find_if(word.begin(), word.end(), [](unsigned char ch) {
                return !std::ispunct(ch);
            }));
            word.erase(std::find_if(word.rbegin(), word.rend(), [](unsigned char ch) {
                return !std::ispunct(ch);
            }).base(), word.end());

            if (currentPath.find("entities.organizations") != std::string::npos && currentPath.find("name") != std::string::npos) { // Check if the current path indicates an organization name
                words[0].insert(word); // Insert the word into the organization set
            } else if (currentPath.find("entities.persons") != std::string::npos && currentPath.find("name") != std::string::npos) { // Check if the current path indicates a person name
                words[1].insert(word); // Insert the word into the person set
            }
            words[2].insert(word); // Insert the word into the other words set
        }
    } else if (value.IsObject()) { // Check if the value is an object
        for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) { // Iterate over each member of the object
            extractWords(it->value, words, currentPath.empty() ? it->name.GetString() : currentPath + "." + it->name.GetString()); // Recursively extract words from the member
        }
    } else if (value.IsArray()) { // Check if the value is an array
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) { // Iterate over each element of the array
            extractWords(value[i], words, currentPath + "[" + std::to_string(i) + "]"); // Recursively extract words from the element
        }
    }
}

std::vector<std::unordered_set<std::string>> SearchEngine::getRelevantData(const std::string& filePath) const {
    std::vector<std::unordered_set<std::string>> words(3); // Initialize a vector of sets to hold the words
    std::ifstream file(filePath); // Open the file
    if (!file.is_open()) { // Check if the file failed to open
        std::cerr << "Failed to open file: " << filePath << std::endl; // Print an error message
        return words; // Return the empty vector of sets
    }

    rapidjson::IStreamWrapper isw(file); // Create a RapidJSON input stream wrapper from the file
    rapidjson::Document document; // Initialize a RapidJSON document
    document.ParseStream(isw); // Parse the JSON from the input stream

    if (document.HasParseError()) { // Check if there was a parse error
        std::cerr << "Failed to parse JSON file: " << filePath << std::endl; // Print an error message
        return words; // Return the empty vector of sets
    }

    extractWords(document, words); // Extract words from the document

    return words; // Return the vector of sets
}