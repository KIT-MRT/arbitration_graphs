#include <exception>
#include <iostream>

#include "demo/pacman_agent.hpp"
#include "demo/pacman_wrapper.hpp"
#include "demo/types.hpp"

using namespace demo;

int main() {
    try {
        PacmanWrapper demo;
        PacmanAgent agent;

        while (!demo.quit()) {
            agent.updateEnvironmentModel(demo.game());

            Command command = agent.getCommand(Clock::now());
            std::cout << agent.to_str(Clock::now()) << '\n';

            demo.progressGame(command);
        }
    } catch (std::exception& e) {
        std::cout << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
