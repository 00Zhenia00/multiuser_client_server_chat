// #include <cstdlib>
// #include <iostream>
// #include <thread>
// #include <utility>
// #include <boost/asio.hpp>

// using boost::asio::ip::tcp;

// const int max_length = 1024;

// int main(int argc, char** argv) {
//     try {
//         boost::asio::io_context io_context;

//         char data[max_length];
//         boost::system::error_code error;

//         tcp::acceptor accepter(io_context, tcp::endpoint(tcp::v4(), 8001));
//         tcp::socket sock = accepter.accept();

//         size_t length = sock.read_some(boost::asio::buffer(data), error);
//         if (error == boost::asio::error::eof)
//             return 0; // Connection closed cleanly by peer.
//         else if (error)
//             throw boost::system::system_error(error); // Some other error.

//         std::cout << data << "\n";
//         boost::asio::write(sock, boost::asio::buffer(data, length));

//     }
//     catch (std::exception& e) {
//         std::cerr << "Exception: " << e.what() << "\n";
//     }

//     return 0;
// }

#include "Server.h"


int main(int argc, char** argv) {
    try {
        Server server(8001);
        server.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}