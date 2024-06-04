#include <exception>
#include <iostream>

#include "demo/pacman_agent.hpp"
#include "demo/types.hpp"
#include "utils/pacman_wrapper.hpp"

using namespace demo;
using namespace utils;

int main() {
    try {
        PacmanWrapper demo;
        PacmanAgent agent(demo.game());

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
