#include "utils/pacman_wrapper.hpp"

#include <iostream>

#include <SDL2/SDL_main.h>

#include <pacman/core/constants.hpp>
#include <pacman/util/frame_cap.hpp>
#include <pacman/util/sdl_check.hpp>
#include <pacman/util/sdl_load_texture.hpp>

namespace utils {

int getScaleFactor() {
    // Make the largest window possible with an integer scale factor
    SDL_Rect bounds;
#if SDL_VERSION_ATLEAST(2, 0, 5)
    SDL_CHECK(SDL_GetDisplayUsableBounds(0, &bounds));
#else
#warning SDL 2.0.5 or later is recommended
    SDL_CHECK(SDL_GetDisplayBounds(0, &bounds));
#endif
    const int scaleX = bounds.w / tilesPx.x;
    const int scaleY = bounds.h / tilesPx.y;
    return std::max(1, std::min(scaleX, scaleY));
}

int initVideoAndGetScaleFactor() {
    SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));
    return getScaleFactor();
}

SDL::Window createWindow(int scaleFactor) {
    return SDL::Window{SDL_CHECK(SDL_CreateWindow("Pacman",
                                                  SDL_WINDOWPOS_CENTERED,
                                                  SDL_WINDOWPOS_CENTERED,
                                                  tilesPx.x * scaleFactor,
                                                  tilesPx.y * scaleFactor,
                                                  SDL_WINDOW_SHOWN))};
}

SDL::Renderer createRendererAndSetLogicalSize(const SDL::Window& window) {
    auto renderer = SDL::Renderer{
        SDL_CHECK(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))};
    SDL_CHECK(SDL_RenderSetLogicalSize(renderer.get(), tilesPx.x, tilesPx.y));
    return renderer;
}

PacmanWrapper::PacmanWrapper()
        : scaleFactor_(initVideoAndGetScaleFactor()), window_(createWindow(scaleFactor_)),
          renderer_(createRendererAndSetLogicalSize(window_)),
          maze_(SDL::loadTexture(renderer_.get(), animera::getTextureInfo())), writer_({renderer_.get(), maze_.get()}) {
    game_.init();
    SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_BLEND);
}

void PacmanWrapper::handleUserInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            quit_ = true;
            break;
        }

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                pause_ = !pause_;
                break;
            }
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE || event.key.keysym.scancode == SDL_SCANCODE_Q) {
                quit_ = true;
                break;
            }
            if (event.key.keysym.scancode == SDL_SCANCODE_P) {
                renderPath_ = !renderPath_;
                break;
            }
        }
    }
}

void PacmanWrapper::progressGame(const demo::Command& command,
                                 const demo::EnvironmentModel::ConstPtr& environmentModel) {
    handleUserInput();

    game_.input(command.scancode());

    if (pause_) {
        return;
    }

    // Update game state
    if (frame_ % tileSize == 0) {
        if (!game_.logic()) {
            quit_ = true;
        }
    }

    SDL_CHECK(SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 255));
    SDL_CHECK(SDL_RenderClear(renderer_.get()));
    game_.render(writer_, frame_ % tileSize);

    if (renderPath_) {
        renderPath(environmentModel->toAbsolutePath(command.path));
    }

    frame_++;
    SDL_RenderPresent(renderer_.get());

    FrameCap sync{fps};
}

void PacmanWrapper::printKeybindings() {
    std::cout << "\n"
              << "\033[1;36m=====================================\033[0m\n"
              << "\033[1;37m               CONTROLS              \033[0m\n"
              << "\033[1;36m=====================================\033[0m\n"
              << "  \033[1;32mESC/Q\033[0m - Quit the demo\n"
              << "  \033[1;32mSpace\033[0m - Pause the demo\n"
              << "  \033[1;32mP\033[0m     - Toggle path visualization\n"
              << "\033[1;36m=====================================\033[0m\n"
              << "  \033[1;32mGUI\033[0m   - Open http://localhost:8080\n"
              << "\033[1;36m=====================================\033[0m\n"
              << std::endl;
}

void PacmanWrapper::renderPath(const demo::Positions& path) {
    // Set path color and transparency
    SDL_CHECK(SDL_SetRenderDrawColor(renderer_.get(), 0, 255, 0, 90));

    SDL_Rect rect;
    rect.w = tileSize;
    rect.h = tileSize;

    for (const auto& position : path) {
        rect.x = position.x * tileSize;
        rect.y = position.y * tileSize;

        SDL_RenderFillRect(renderer_.get(), &rect);
    }
}

} // namespace utils
