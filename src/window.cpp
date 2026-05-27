#include "window.h"

Window::Window(uint32_t _width, uint32_t _height)
    : window(nullptr), renderer(nullptr), texture(nullptr), width(_width),
      height(_height), running(false), lastMouseX(0), lastMouseY(0) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return;
  }

  window = SDL_CreateWindow("RVE", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, (int)_width, (int)_height,
                            SDL_WINDOW_SHOWN);
  if (!window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    return;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    return;
  }

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, (int)_width,
                              (int)_height);
  if (!texture) {
    SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
    return;
  }

  running = true;
}

Window::~Window() {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

/**
 * @brief Will upload and render the passed array of pixels
 *
 * @param pixels pixels to be rendered, must be size WIDHT*HEIGHT
 */
void Window::present(const std::array<uint32_t, window::SIZE> &pixels) {
  if (!running || !window || !renderer || !texture)
    return;

  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      running = false;
    }
    SDL_GetMouseState(&lastMouseX, &lastMouseY);
  }

  SDL_UpdateTexture(texture, nullptr, pixels.data(),
                    (int)(width * sizeof(uint32_t)));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}
