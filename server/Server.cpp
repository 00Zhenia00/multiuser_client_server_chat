#include "Server.h"

#include <algorithm>
#include <chrono>

#include "Message.h"

namespace {

const int MAX_CLIENTS_AMOUNT = 5;

}  // namespace

using namespace boost::asio;
using boost::system::error_code;

Server::Server(int port)
    : mAcceptor(mIoContext, tcp::endpoint(tcp::v4(), port)),
      mClientIdCounter(0) {}

void Server::clientHandler(int connectionId) {
    auto connectionIt = std::find_if(
        mClientConnections.begin(), mClientConnections.end(),
        [connectionId](const std::unique_ptr<ClientConnection>& connection) {
            return connection->id == connectionId;
        });

    error_code error;
    tcp::socket& sock = (*connectionIt)->sock;
    while (true) {
        Message inmsg;
        size_t length = read(sock, buffer(&inmsg, sizeof(inmsg)), error);
        if (error) {
            if (error == error::eof) {
                std::cout << "Server::clientHandler(): Client '" << connectionId
                          << "'disconnected!\n";
            } else {
                std::cout << "Server::clientHandler(): Failed to read client '"
                          << connectionId << "' message! Error:" << error
                          << "\n";
            }
            std::lock_guard<std::mutex> lock(mMutex);
            std::cout << "Remove conection '" << connectionId << "'.\n";
            mClientConnections.erase(connectionIt);
            return;
        }

        switch (inmsg.type) {
            case MessageType::CONNECT: {
                Message outmsg{MessageType::ACCEPT, ""};
                write(sock, buffer(reinterpret_cast<void*>(&outmsg), sizeof(outmsg)), error);
                if (error) {
                    std::cout << "Server::clientHandler(): Failed to write"
                                 "message to client '"
                              << connectionId << "'!\n";
                    return;
                }
                break;
            }

            case MessageType::JOIN: {
                (*connectionIt)->name = std::string(inmsg.text);
                Message outmsg{MessageType::OK, ""};
                write(sock, buffer(reinterpret_cast<void*>(&outmsg), sizeof(outmsg)), error);
                if (error) {
                    std::cout << "Server::clientHandler(): Failed to write"
                                 "message to client '"
                              << connectionId << "'!\n";
                    return;
                }
                break;
            }

            case MessageType::TEXT: {
                strcpy(inmsg.name, (*connectionIt)->name.data());
                for (const auto& client : mClientConnections) {
                    if (client->id != connectionId) {
                        write(client->sock, buffer(reinterpret_cast<void*>(&inmsg), sizeof(inmsg)), error);
                        if (error) {
                            std::cout
                                << "Server::clientHandler(): Failed to write"
                                   "message to client '"
                                << client->id << "'!\n";
                            return;
                        }
                    }
                }
                break;
            }

            case MessageType::PING: {
                write(sock, buffer(reinterpret_cast<void*>(&inmsg), sizeof(inmsg)), error);
                if (error) {
                    std::cout << "Server::clientHandler(): Failed to write"
                                 "message to client '"
                              << connectionId << "'!\n";
                    return;
                }
                break;
            }
        }

        std::cout << "MESSAGE:\n";
        std::cout << "\tlength: " << length << "\n";
        std::cout << "\tclient id: " << connectionId << "\n";
        std::cout << "\tclient name: " << (*connectionIt)->name << "\n";
        std::cout << "\tsubject: " << inmsg.text << "\n";
    }
}

void Server::run() {
    std::cout << "Server::run(): Server is running!\n";
    while (true) {
        if (mClientConnections.size() == MAX_CLIENTS_AMOUNT) {
            // Sleep some time
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            continue;
        }
        error_code error;
        tcp::socket sock = mAcceptor.accept(error);
        if (error) {
            std::cout << "Server::run(): Failed to accept new socket"
                         "connection!\n";
            return;
        }
        std::cout << "Server::run(): New client connected!\n";
        auto connection = std::make_unique<ClientConnection>(mClientIdCounter, "", std::move(sock));
        mClientConnections.push_back(std::move(connection));
        std::thread([this](int connectionId) { clientHandler(connectionId); },
                    mClientIdCounter).detach();
        mClientIdCounter++;
    }
}