#include "demo/pacman_wrapper.hpp"

#include <SDL_main.h>

#include <pacman/core/constants.hpp>
#include <pacman/util/frame_cap.hpp>
#include <pacman/util/sdl_check.hpp>
#include <pacman/util/sdl_load_texture.hpp>

namespace demo {

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

PacmanWrapper::PacmanWrapper() {
    SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));
    scaleFactor_ = getScaleFactor();
    window_ = SDL::Window{SDL_CHECK(SDL_CreateWindow("Pacman",
                                                     SDL_WINDOWPOS_CENTERED,
                                                     SDL_WINDOWPOS_CENTERED,
                                                     tilesPx.x * scaleFactor_,
                                                     tilesPx.y * scaleFactor_,
                                                     SDL_WINDOW_SHOWN))};
    renderer_ = SDL::Renderer{
        SDL_CHECK(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))};
    SDL_CHECK(SDL_RenderSetLogicalSize(renderer_.get(), tilesPx.x, tilesPx.y));

    maze_ = SDL::loadTexture(renderer_.get(), animera::getTextureInfo());
    writer_ = {renderer_.get(), maze_.get()};

    game_.init();
}

void PacmanWrapper::progressGame(const demo::Command& command) {
    game_.input(command.scancode());

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit_ = true;
            break;
        }
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
    frame_++;
    SDL_RenderPresent(renderer_.get());

    FrameCap sync{fps};
}

} // namespace demo
