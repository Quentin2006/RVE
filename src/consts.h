#pragma once

namespace window {
constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
constexpr int SIZE = WIDTH * HEIGHT;
constexpr float ASPECT_RATIO = (float)WIDTH / HEIGHT;
} // namespace window

namespace camera {
constexpr float FOCAL_LENGTH = 1.f;
constexpr float VIEWPORT_HEIGHT = 1.f;
constexpr float VIEWPORT_WIDTH =
    VIEWPORT_HEIGHT * ((float)window::WIDTH / window::HEIGHT);
} // namespace camera
