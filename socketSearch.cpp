#include "searchEngine.h" // Include the search engine header

#include <iostream> // Include input-output stream
#include <string> // Include string library
#include <cstring> // Include C string library
#include <sys/types.h> // Include socket types
#include <sys/socket.h> // Include socket library
#include <netinet/in.h> // Include internet address family
#include <unistd.h> // Include POSIX operating system API

#define PORT 12345 // Define the port number
#define BUFFER_SIZE 1024 // Define the buffer size

int main() {
    SearchEngine searchEngine("docs", "fnsavefile.csv", "osavefile.csv", "nsavefile.csv", "wsavefile.csv", "fsavefile.csv"); // Initialize the search engine

    int server_fd, new_socket; // Declare server and new socket file descriptors
    struct sockaddr_in address; // Declare address structure
    int addrlen = sizeof(address); // Get the size of the address structure
    char buffer[BUFFER_SIZE] = {0}; // Initialize buffer with zeros

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { // Create a socket
        std::cerr << "Socket failed" << std::endl; // Print error message if socket creation fails
        return -1; // Return error code
    }

    address.sin_family = AF_INET; // Set address family to Internet
    address.sin_addr.s_addr = INADDR_ANY; // Set IP address to any available interface
    address.sin_port = htons(PORT); // Set port number

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { // Bind the socket to the address
        std::cerr << "Bind failed" << std::endl; // Print error message if bind fails
        close(server_fd); // Close the socket
        return -1; // Return error code
    }

    if (listen(server_fd, 3) < 0) { // Listen for incoming connections
        std::cerr << "Listen failed" << std::endl; // Print error message if listen fails
        close(server_fd); // Close the socket
        return -1; // Return error code
    }

    while (true) { // Infinite loop to handle incoming connections
        std::cout << "Waiting for connections..." << std::endl; // Print waiting message
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { // Accept a new connection
            std::cerr << "Accept failed" << std::endl; // Print error message if accept fails
            close(server_fd); // Close the socket
            return -1; // Return error code
        }

        int valread = read(new_socket, buffer, BUFFER_SIZE); // Read data from the socket
        if (valread > 0) { // Check if data was read
            std::string query(buffer, valread); // Convert buffer to string
            auto results = searchEngine.search(query); // Perform search with the query

            std::string response; // Initialize response string
            for (const auto& result : results) { // Iterate through search results
                response += result + "\n"; // Append each result to the response string
            }

            send(new_socket, response.c_str(), response.size(), 0); // Send the response back to the client
        }

        close(new_socket); // Close the new socket
    }

    close(server_fd); // Close the server socket
    return 0; // Return success code
}