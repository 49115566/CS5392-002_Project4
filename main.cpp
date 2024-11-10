#include "searchEngine.h"

#include <iostream>
#include <string>

int main() {
    SearchEngine searchEngine("savefile.csv", "docs");


    std::string query;
    std::cout << "Enter search query: ";
    std::getline(std::cin, query);

    auto results = searchEngine.search(query);

    std::cout << results.size() << " search results:\n";
    for (const auto& result : results) {
        std::cout << result << std::endl;
    }

    return 0;
}
