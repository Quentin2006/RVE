#pragma once

constexpr int BLOCK_SIZE = 256;
constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
constexpr int SIZE = WIDTH * HEIGHT;
constexpr int NUM_BLOCKS = (SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;
