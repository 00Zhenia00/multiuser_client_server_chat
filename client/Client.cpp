#include "Client.h"

#include <chrono>

#include "Message.h"

using namespace boost::asio;
using boost::system::error_code;

Client::Client() : mSocket(mIoContext), mPingFlag(false) {}

bool Client::connect(const std::string& host, const uint16_t port) {
    tcp::endpoint ep(ip::address::from_string(host), port);
    error_code error;
    mSocket.connect(ep, error);
    if (error) return false;

    Message request{MessageType::CONNECT, "", ""};
    write(mSocket, buffer(reinterpret_cast<void*>(&request), sizeof(request)),
          error);
    if (error) return false;

    Message response;
    size_t length = boost::asio::read(
        mSocket, boost::asio::buffer(&response, sizeof(response)), error);
    if (error || length < sizeof(Message)) return false;
    return response.type == MessageType::ACCEPT;
}

bool Client::join_server(const std::string& name) {
    if (name.empty()) return false;

    Message request;
    request.type = MessageType::JOIN;
    strcpy(request.text, name.data());

    error_code error;
    write(mSocket, buffer(reinterpret_cast<void*>(&request), sizeof(request)),
          error);
    if (error) return false;

    Message response;
    size_t length = read(mSocket, buffer(&response, sizeof(response)), error);
    if (error || length < sizeof(response)) return false;
    return response.type == MessageType::OK;
}

bool Client::send(const std::string& msg) {
    if (msg.empty()) return false;

    Message request;
    request.type = MessageType::TEXT;
    strcpy(request.text, msg.data());

    error_code error;
    write(mSocket, buffer(reinterpret_cast<void*>(&request), sizeof(request)),
          error);
    if (error) return false;
    return true;
}

bool Client::recv(std::string& name, std::string& msg) {
    Message response;
    error_code error;
    while (response.type != MessageType::TEXT) {
        size_t length =
            read(mSocket,
                 buffer(reinterpret_cast<void*>(&response), sizeof(response)),
                 error);
        if (error || length < sizeof(response)) return false;
        if (response.type == MessageType::PING) {
            {
                std::lock_guard<std::mutex> lock(mPingMutex);
                mPingFlag = true;
            }
            mPingCondVar.notify_one();
        }
    }

    name = response.name;
    msg = response.text;
    return true;
}

double Client::ping() {
    Message request{MessageType::PING, "", ""};
    error_code error;
    std::chrono::system_clock::time_point pingStart =
        std::chrono::system_clock::now();
    write(mSocket, buffer(reinterpret_cast<void*>(&request), sizeof(request)),
          error);
    if (error) return -1.0;

    std::unique_lock<std::mutex> lock(mPingMutex);
    mPingCondVar.wait(lock, [this] { return mPingFlag; });
    std::chrono::system_clock::time_point pingEnd =
        std::chrono::system_clock::now();
    mPingFlag = false;
    return std::chrono::duration<double>(pingEnd - pingStart).count();
}
