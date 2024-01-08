#include <iostream>
#include <boost/asio.hpp>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace boost::asio;
using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char** argv) {
    try {
        boost::asio::io_context io_context;

        tcp::socket sock(io_context);
        tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
        sock.connect(ep);

        std::thread([&sock](){
            boost::system::error_code error;
            while (true) {
                char msg[max_length];
                size_t length = sock.read_some(boost::asio::buffer(msg), error);
                if (error == boost::asio::error::eof) {
                    std::cout << "Client: Server disconnected!\n";
                    return; // Connection closed cleanly by peer.
                }
                if (error) {
                    std::cout << "Client: Failed to read message!\n";
                    return;
                }
                msg[length] = '\0';
                std::cout << msg << std::endl;
            }
        }).detach();

        while (true) {
            char request[max_length];
            // std::cout << "Enter message: ";
            std::cin.getline(request, max_length);
            size_t request_length = std::strlen(request);
            boost::asio::write(sock, boost::asio::buffer(request, request_length));
        }

    }
    catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}