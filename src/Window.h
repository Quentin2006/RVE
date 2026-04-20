#include "SDL_render.h"
#include "SDL_video.h"
#include <cstdint>

class Window {
public:
  Window(const Window &) = default;
  Window(Window &&) = delete;
  Window &operator=(const Window &) = default;
  Window &operator=(Window &&) = delete;

  Window(uint32_t width, uint32_t height);
  ~Window();

  void present(const uint32_t *pixels);

  [[nodiscard]] bool should_close() const { return !running; }

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  uint32_t width;
  uint32_t height;

  bool running;
};
