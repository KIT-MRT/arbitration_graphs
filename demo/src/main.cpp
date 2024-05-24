#include <exception>
#include <iostream>

#include "demo/pacman_arbitrator.hpp"
#include "demo/pacman_wrapper.hpp"
#include "demo/types.hpp"

using namespace demo;

int main() {
    try {
        PacmanWrapper demo;
        PacmanArbitrator arbitrator;

        while (!demo.quit()) {
            arbitrator.updateEnvironmentModel(demo.game());

            Command command = arbitrator.getCommand(Clock::now());
            std::cout << arbitrator.to_str(Clock::now()) << '\n';

            demo.progressGame(command);
        }
    } catch (std::exception& e) {
        std::cout << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
