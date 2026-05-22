CXX := g++
SRC_DIR := src
BIN_DIR := bin
TARGET := $(BIN_DIR)/rve

INCLUDES := -I/usr/include/SDL2 -I/usr/include/glm
LIBS := -lSDL2 -lm

CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -O2 -DNDEBUG

# recursive source discovery (nested dirs supported)
SRCS := $(shell find $(SRC_DIR) -type f -name "*.cpp")

all: $(TARGET)

$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $@ $(LIBS)

$(BIN_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BIN_DIR)

M = "my lazy git commit comment"
git:
	git add $(SRCS)  **/*.h Makefile
	git commit -m $(M)
	git push

.PHONY: all run clean git
