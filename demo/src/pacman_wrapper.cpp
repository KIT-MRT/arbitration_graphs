#include "utils/pacman_wrapper.hpp"

#include <SDL_main.h>

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
}

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

} // namespace utils
