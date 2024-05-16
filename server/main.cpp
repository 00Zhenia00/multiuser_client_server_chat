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