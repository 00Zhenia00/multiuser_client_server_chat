#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Server {
public:
    Server(int port);
    void run();

private:
    struct ClientConnection {
        ClientConnection(int id, const std::string& name, tcp::socket sock)
            : id(id), name(name), sock(std::move(sock)) {}
        int id;
        std::string name;
        tcp::socket sock;
    };

    void clientHandler(int);

private:
    boost::asio::io_context mIoContext;
    tcp::acceptor mAcceptor;
    std::list<std::unique_ptr<ClientConnection>> mClientConnections;
    int mClientIdCounter;
    std::mutex mMutex;
};