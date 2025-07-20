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
    PacmanWrapper(const PacmanWrapper&) = delete;
    PacmanWrapper(PacmanWrapper&&) = delete;
    PacmanWrapper& operator=(const PacmanWrapper&) = delete;
    PacmanWrapper& operator=(PacmanWrapper&&) = delete;
    ~PacmanWrapper() {
        SDL_Quit();
    }

    void progressGame(const demo::Command& command, const demo::EnvironmentModel& environmentModel);

    bool quit() const {
        return quit_;
    }
    Game& game() {
        return game_;
    }

    static void printKeybindings();

private:
    void handleUserInput();
    void renderPath(const demo::Positions& path);
    void toggleFullscreen();

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
