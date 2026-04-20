#include "Window.h"
#include "SDL.h"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include <cstdint>

Window::Window(uint32_t _width, uint32_t _height)
    : window(SDL_CreateWindow("RVE", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, (int)_width, (int)_height,
                              SDL_WINDOW_SHOWN)

                 ),
      renderer(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)),
      texture(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, (int)_width,
                                (int)_height)),
      width(_width), height(_height), running(true)

{
  SDL_Init(SDL_INIT_VIDEO);
}

Window::~Window() {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

// NOTE: is is exspected that his will take in an array of size WIDTH*HEIGHT
void Window::present(const uint32_t *pixels) {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      running = false;
    }
    if (SDL_KEYDOWN == event.type && event.key.keysym.sym == SDLK_ESCAPE) {
      running = false;
    }
  }

  SDL_UpdateTexture(texture, nullptr, pixels, (int)(width * sizeof(uint32_t)));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}
