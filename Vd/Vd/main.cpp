#include "fakekbd.h"


int RunServer();
int RunClient();

int main(int argc, char* argv[]) {
    std::cout << "=== Virtual Keyboard Device Emulator ===" << std::endl;
    std::cout << "Usage: " << std::endl;
    std::cout << "  " << argv[0] << " server    - Run as device server" << std::endl;
    std::cout << "  " << argv[0] << " client    - Run as client demo" << std::endl;

    if (argc < 2) {
        std::cerr << "Please specify 'server' or 'client'" << std::endl;
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "server") {
        return RunServer();
    }
    else if (mode == "client") {
        return RunClient();
    }
    else {
        std::cerr << "Invalid mode. Use 'server' or 'client'" << std::endl;
        return 1;
    }
}