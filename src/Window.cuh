#include <SDL2/SDL.h>
#include <cstdint>

class Window {
public:
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
