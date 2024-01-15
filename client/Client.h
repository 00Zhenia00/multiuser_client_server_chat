#pragma once

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

    /**
     * @brief Establishes connections to the server.
     *
     * @return false if connection failed or server declined connection, else
     * true.
     */
    bool connect(const std::string& host, const uint16_t port);

    /**
     * @brief Join server with given name.
     *
     * @return false if read/write failed or server declined joining, else true.
     */
    bool joinServer(const std::string& name);

    /**
     * @brief Send given message to server.
     *
     * @return false if write failed, else true.
     */
    bool send(const std::string& msg);

    /**
     * @brief Receive message from server.
     *
     * @param name message author name
     * @param msg message text
     *
     * @return false if write failed, else true.
     */
    bool recv(std::string& name, std::string& msg);

    /**
     * @brief Ping server.
     *
     * @return -1 if ping failed, else return ping time.
     */
    double ping();

private:
    boost::asio::io_context mIoContext;
    tcp::socket mSocket;
    std::mutex mPingMutex;
    std::condition_variable mPingCondVar;
    bool mPingFlag;
};