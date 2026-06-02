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
constexpr int SAMPLES_PER_PIXEL = 1;
constexpr int MAX_DEPTH = 3;
} // namespace camera

namespace input {
constexpr float LOOK_SPEED = 0.25f;
constexpr float MOVE_SPEED = 1.f;
} // namespace input

namespace math {
constexpr float K_EPSILON = 1e-6f;
} // namespace math
