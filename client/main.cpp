#include "Client.h"

#include <boost/asio.hpp>

namespace {

const int MAX_NAME_LENGTH = 128;
// const std::string CLIENT_ENTER_SYMBOL = "> ";

}  // namespace

int main() {
    Client client;
    if (!client.connect("127.0.0.1", 8001)) {
        std::cout << "Error: Failed to connect to server!\n";
    }
    std::cout << "Connected to server!\n";

    char name[MAX_NAME_LENGTH];
    std::cout << "Enter your name: ";
    std::cin.getline(name, MAX_NAME_LENGTH);
    if (!client.joinServer(name)) {
        std::cout << "Error: Failed to join server!\n";
    }
    std::cout << "Joined server!\n";

    auto messagesHandler = [&client]() {
        while (true) {
            std::string name;
            std::string msg;
            if (!client.recv(name, msg)) {
                std::cout << "Failed to receive message!\n";
                return;
            }
            std::cout << "<" << name << ">: " << msg << std::endl;
        }
    };
    std::thread(messagesHandler).detach();

    double pingTime = 0.0;
    if ((pingTime = client.ping()) < 0.0) {
        std::cout << "Failed to ping server!\n";
        return -1.0;
    }
    std::cout << "Ping: " << pingTime << "\n";

    while (true) {
        char request[1024];
        // std::cout << CLIENT_ENTER_SYMBOL;
        std::cin.getline(request, 1024);

        client.send(request);
    }
    return 0;
}