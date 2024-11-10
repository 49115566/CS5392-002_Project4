#include "searchEngine.h"

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    SearchEngine searchEngine("docs", "osavefile.csv", "nsavefile.csv", "wsavefile.csv");

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return -1;
    }

    while (true) {
        std::cout << "Waiting for connections..." << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            close(server_fd);
            return -1;
        }

        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            std::string query(buffer, valread);
            auto results = searchEngine.search(query);

            std::string response;
            for (const auto& result : results) {
                response += result + "\n";
            }

            send(new_socket, response.c_str(), response.size(), 0);
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}