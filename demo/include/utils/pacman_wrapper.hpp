#pragma once

#include <SDL.h>

#include <pacman/core/game.hpp>
#include <pacman/util/sdl_delete.hpp>
#include <pacman/util/sdl_quad_writer.hpp>

#include "demo/types.hpp"

namespace utils {

class PacmanWrapper {
public:
    PacmanWrapper();
    ~PacmanWrapper() {
        SDL_Quit();
    }

    void progressGame(const demo::Command& command);

    bool quit() const {
        return quit_;
    }
    Game& game() {
        return game_;
    }

private:
    void handleUserInput();

    int scaleFactor_;
    SDL::Window window_;
    SDL::Renderer renderer_;
    SDL::Texture maze_;
    SDL::QuadWriter writer_;

    Game game_;
    int frame_{0};
    bool quit_{false};
    bool pause_{false};
};

} // namespace utils
