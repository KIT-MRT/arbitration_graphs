#include <exception>
#include <iostream>
#include <thread>

#include "demo/pacman_agent.hpp"
#include "demo/types.hpp"
#include "utils/pacman_wrapper.hpp"
#include "utils/websocket_server.hpp"

using namespace demo;
using namespace utils;

int main() {
    try {
        PacmanWrapper demo;
        PacmanAgent agent(demo.game());

        WebSocketServer server(1256);

        std::thread serverThread([&server]() { server.run(); });
        while (!demo.quit()) {
            agent.updateEnvironmentModel(demo.game());

            const Time time = Clock::now();

            Command command = agent.getCommand(time);
            std::cout << agent.to_str(time) << '\n';

            server.broadcast(agent.yamlString(time));

            demo.progressGame(command);
        }
    } catch (std::exception& e) {
        std::cout << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
