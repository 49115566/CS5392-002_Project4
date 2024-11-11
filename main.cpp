#include "searchEngine.h"

#include <iostream>
#include <string>

int main() {
    SearchEngine searchEngine("docs", "fnsavefile.csv", "osavefile.csv", "nsavefile.csv", "wsavefile.csv");

    std::string query;
    char choice;

    do {
        std::cout << "Enter search query: ";
        std::getline(std::cin, query);

        auto results = searchEngine.search(query);

        std::cout << results.size() << " search results:\n";
        for (const auto& result : results) {
            std::cout << result << std::endl;
        }

        std::cout << "Do you want to search again? (y/n): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    } while (choice == 'y' || choice == 'Y');

    return 0;
}
