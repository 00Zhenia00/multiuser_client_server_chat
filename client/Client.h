#include <iostream>
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Client {
public:
    Client();
    bool connect(const std::string &host, const uint16_t port);
    bool join_server(const std::string& name);
    bool send(const std::string& msg);
    bool recv(std::string& name, std::string& msg);
    double ping();

private:
    boost::asio::io_context mIoContext;
    tcp::socket mSocket;
    std::mutex mPingMutex;
    std::condition_variable mPingCondVar;
    bool mPingFlag;
};