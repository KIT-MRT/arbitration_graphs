#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "gui/web_server.hpp"

using namespace arbitration_graphs;


namespace {
// std::atomic is safe, as long as it is lock-free
std::atomic<bool> shutdownRequested = false;
static_assert(std::atomic<bool>::is_always_lock_free);
} // namespace

void requestShutdown(int /*signum*/) {
    shutdownRequested = true;

    const char str[] = "received signal, requesting shutdown\n";
    auto _ = write(STDERR_FILENO, str, sizeof(str) - 1);
}

int main() {
    // setup signal handler
    {
        struct sigaction action {};
        action.sa_handler = requestShutdown;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(SIGINT, &action, nullptr);
    }

    // Running the web server
    std::cout << "Starting WebServer …" << std::endl;
    gui::WebServer server{8080, true};

    std::cout << "WebServer set up, sending some messages" << std::endl;

    while (!shutdownRequested) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate some delay
        server.broadcast("Hello from the server!");
        std::cout << "Hello from the main thread!" << std::endl;
    }

    std::cout << "Closing" << std::endl;

    return EXIT_SUCCESS;
}
