#include "searchEngine.h" // Include the search engine header file

#include <iostream> // Include the iostream library for input and output
#include <string> // Include the string library for string manipulation

int main() { // Main function
    SearchEngine searchEngine("docs", "fnsavefile.csv", "osavefile.csv", "nsavefile.csv", "wsavefile.csv"); // Create a SearchEngine object with specified parameters

    std::string query; // Declare a string to hold the search query
    char choice; // Declare a char to hold the user's choice

    do { // Start a do-while loop
        std::cout << "Enter search query: "; // Prompt the user to enter a search query
        std::getline(std::cin, query); // Get the search query from the user

        auto results = searchEngine.search(query); // Perform the search and store the results

        std::cout << results.size() << " search results:\n"; // Output the number of search results
        for (const auto& result : results) { // Loop through the search results
            std::cout << result << std::endl; // Output each search result
        }

        std::cout << "Do you want to search again? (y/n): "; // Ask the user if they want to search again
        std::cin >> choice; // Get the user's choice
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    } while (choice == 'y' || choice == 'Y'); // Continue the loop if the user wants to search again

    return 0; // Return 0 to indicate successful execution
}
