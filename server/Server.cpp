#include "Server.h"

#include <algorithm>
#include <chrono>

namespace {

const int MAX_MSG_LENGTH = 1024;
const int MAX_CLIENTS_AMOUNT = 5;

}

Server::Server(int port): mAcceptor(mIoContext, tcp::endpoint(tcp::v4(), port)),
                          mClientIdCounter(0) {}

void Server::clientHandler(std::shared_ptr<ClientConnection> connection) {
    boost::system::error_code error;
    int connectionId = connection->id;
    tcp::socket& sock = connection->sock;
    while (true) {
        char data[MAX_MSG_LENGTH];

        size_t length = sock.read_some(boost::asio::buffer(data), error);
        if (error) {
            if (error == boost::asio::error::eof) {
                std::cout << "Server::clientHandler(): Client disconnected!\n";
            }
            else {
                std::cout << "Server::clientHandler(): Failed to read client message! Error:" << error << "\n";
            }
            std::lock_guard<std::mutex> lock(mMutex);
            std::cout << "Remove conection '" << connectionId << "'.\n";
            auto connectionIt = std::find_if(mClientConnections.begin(),
                                             mClientConnections.end(),
                                             [connectionId](const std::shared_ptr<ClientConnection>& client) {
                                                return client->id == connectionId;
                                             });
            mClientConnections.erase(connectionIt);
            return;
        }

        for (const auto& client : mClientConnections) {
            if (client->id != connectionId) {
                boost::asio::write(client->sock, boost::asio::buffer(data, length), error);
                if (error) {
                    std::cout << "Server::clientHandler(): Failed to write message to client '"
                        << client->id << "'!\n";
                    return;
                }
            }
        }

    }

}

void Server::run() {
    std::cout << "Server::run(): Start!\n";
    while (true) {
        if (mClientConnections.size() == MAX_CLIENTS_AMOUNT) {
            // TODO: sleep some time?
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            continue;
        }
        boost::system::error_code error;
        tcp::socket sock = mAcceptor.accept(error);
        if (error) {
            std::cout << "Server::run(): Failed to accept new socket connection!\n";
            return;
        }
        std::cout << "Server::run(): New client connected!\n";
        auto client = std::make_shared<ClientConnection>(mClientIdCounter, std::move(sock));
        mClientConnections.push_back(client);   // copy client (point counter +1)
        std::thread([this, client]() mutable {  // copy client (point counter +1)
            clientHandler(std::move(client)); }).detach();  // move client (point counter the same)
        mClientIdCounter++;
    }   // client point counter = 1 (in the mClientConnections)
}