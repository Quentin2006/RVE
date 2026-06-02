#pragma once

#include <glm/glm.hpp>

namespace window {
inline constexpr int WIDTH = 800;
inline constexpr int HEIGHT = 800;
inline constexpr int SIZE = WIDTH * HEIGHT;
} // namespace window

namespace camera {
inline constexpr float FOCAL_LENGTH = 1.f;
inline constexpr float VIEWPORT_HEIGHT = 1.f;
inline constexpr float VIEWPORT_WIDTH =
    VIEWPORT_HEIGHT * ((float)window::WIDTH / window::HEIGHT);
inline constexpr int SAMPLES_PER_PIXEL = 1;
inline constexpr int MAX_DEPTH = 3;
} // namespace camera

namespace input {
inline constexpr float LOOK_SPEED = 0.25f;
inline constexpr float MOVE_SPEED = 1.f;
} // namespace input

namespace math {
inline constexpr float K_EPSILON = 1e-6f;
} // namespace math
