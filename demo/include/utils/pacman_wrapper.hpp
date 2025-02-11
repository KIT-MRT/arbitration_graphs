#pragma once

#include <SDL2/SDL.h>

#include <pacman/core/game.hpp>
#include <pacman/util/sdl_delete.hpp>
#include <pacman/util/sdl_quad_writer.hpp>

#include "demo/environment_model.hpp"
#include "demo/types.hpp"

namespace utils {

class PacmanWrapper {
public:
    PacmanWrapper();
    ~PacmanWrapper() {
        SDL_Quit();
    }

    void progressGame(const demo::Command& command, const demo::EnvironmentModel::ConstPtr& environmentModel);

    bool quit() const {
        return quit_;
    }
    Game& game() {
        return game_;
    }

    void printKeybindings();

private:
    void handleUserInput();
    void renderPath(const demo::Positions& path);

    int scaleFactor_;
    SDL::Window window_;
    SDL::Renderer renderer_;
    SDL::Texture maze_;
    SDL::QuadWriter writer_;

    Game game_;
    int frame_{0};

    bool pause_{false};
    bool quit_{false};
    bool renderPath_{false};
    bool fullscreen_{false};
};

} // namespace utils
