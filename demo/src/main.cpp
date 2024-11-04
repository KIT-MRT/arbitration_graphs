#include <exception>
#include <iostream>
#include <thread>

#include <arbitration_graphs/gui/web_server.hpp>

#include "demo/pacman_agent.hpp"
#include "demo/types.hpp"
#include "utils/pacman_wrapper.hpp"

using namespace demo;
using namespace utils;
using namespace arbitration_graphs;

int main() {
    try {
        PacmanWrapper demo;
        PacmanAgent agent(demo.game());

        gui::WebServer server(1256, true);

        while (!demo.quit()) {
            agent.updateEnvironmentModel(demo.game());

            const Time time = Clock::now();

            Command command = agent.getCommand(time);
            std::cout << agent.to_str(time) << '\n';

            server.broadcast(agent.yamlString(time));

            demo.progressGame(command, agent.environmentModel());
        }
    } catch (std::exception& e) {
        std::cout << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
