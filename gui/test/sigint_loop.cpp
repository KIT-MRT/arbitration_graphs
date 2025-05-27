#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "gui/web_server.hpp"

using namespace arbitration_graphs;

int main() {
    // Running the web server
    std::cout << "Starting WebServer …" << std::endl;
    gui::WebServer server{8080, true};

    std::cout << "WebServer set up, sending some messages" << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate some delay
        server.broadcast("Hello from the server!");
        std::cout << "Hello from the main thread!" << std::endl;
    }

    std::cout << "Closing" << std::endl;

    return EXIT_SUCCESS;
}
