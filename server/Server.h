#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Server {
public:
    Server(int port);
    void run();

private:
    struct ClientConnection {
        ClientConnection(int id, tcp::socket sock)
            : id(id), sock(std::move(sock)) {}
        int id;
        tcp::socket sock;
    };

    void clientHandler(std::shared_ptr<ClientConnection>);

private:
    boost::asio::io_context mIoContext;
    tcp::acceptor mAcceptor;
    std::vector<std::shared_ptr<ClientConnection>> mClientConnections;
    int mClientIdCounter;
    std::mutex mMutex;
};